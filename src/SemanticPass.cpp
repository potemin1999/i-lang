/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Config.h"
#include "Semantics.h"
#include "Tree.h"
#include "Utils.h"
#include "generated/Parser.tab.hpp"

using namespace Semantics;

int ProcessBodyElements(Body *body);

Function *currentFunction;

int ProcessSimpleDeclaration(Declaration *declaration) {
    if (declaration->declarationType == DeclarationType::TYPE_DECLARATION) {
        TypeDeclaration *typeDeclaration = static_cast<TypeDeclaration *>(declaration);
        auto type = Semantics::GetCurrentScope()->types()->defineType(typeDeclaration);
        if (IsComponentTraced(COMPONENT_SEMANTICS)) {
            auto typeStr = type != nullptr ? type->toString() : "<type declaration error>";
            GetTraceStream(COMPONENT_SEMANTICS) << "scope " << GetCurrentScope()->getScopeName()
                                                << " new type: " << typeStr << std::endl;
        }
        declaration->isSemanticsValid = type != nullptr;
        return declaration->isSemanticsValid ? 0 : -1;
    } else {
        VariableDeclaration *varDeclaration = static_cast<VariableDeclaration *>(declaration);
        auto name = Semantics::GetCurrentScope()->names()->defineName(varDeclaration);
        if (varDeclaration->initialValue != nullptr) {
            RunTypeTrace(varDeclaration->initialValue);
            // TODO: handle errors
        }
        if (IsComponentTraced(COMPONENT_SEMANTICS)) {
            auto nameStr = name != nullptr ? name->toString() : "<name declaration error>";
            GetTraceStream(COMPONENT_SEMANTICS) << "scope " << GetCurrentScope()->getScopeName()
                                                << " new var : " << nameStr << std::endl;
        }
        declaration->isSemanticsValid = name != nullptr;
        return declaration->isSemanticsValid ? 0 : -1;
    }
}

int ProcessExpressionStatement(ExpressionStatement *statement) {
    auto expression = statement->expression;
    if (expression->isBinary) {
        // auto binaryExpression = static_cast<BinaryExpression*>(expression);
        return RunTypeTrace(expression) != -1 ? 0 : -1;
    } else if (expression->isPrimary) {
        auto primaryExpression = static_cast<Primary *>(expression);
        if (primaryExpression->isCall) {
            // lvalue, function call
            // TODO: process function call (CallablePrimary)
            SetCallStatementStatus(true);
            auto typeTraceResult = RunTypeTrace(primaryExpression) != -1 ? 0 : -1;
            SetCallStatementStatus(false);
            return typeTraceResult;
        }
    } else {
        // unary non-primary expression
        // TODO: print line, column and scope
        Semantics::ReportSemanticWarning("unused expression result, ignoring");
        return -1;
    }
}

int ProcessReturnStatement(ReturnStatement *statement) {
    int typeTraceResult = RunTypeTrace(statement->expressionToReturn);
    ASSERT_NOT_NULL(currentFunction)
    if (currentFunction->getReturnTypeId() == TYPE_ID_VOID) {
        Semantics::ReportSemanticError(
            "Function with void return type should not use return statements");
        return -1;
    }
    auto source = FindNativeBaseType(typeTraceResult);
    auto target = FindNativeBaseType(currentFunction->getReturnTypeId());
    if (source != target) {
        auto castType = InsertTypeCastExpression(
            &statement->expressionToReturn, statement->expressionToReturn, source, target, false);
        if (castType != currentFunction->getReturnTypeId()) {
            Semantics::ReportSemanticError("Cast failed during return statement processing");
            return -1;
        }
    }
    return typeTraceResult != -1 ? 0 : -1;
}

int ProcessIfStatement(IfStatement *statement) {
    int expressionTypeId = RunTypeTrace(statement->expression);
    if (expressionTypeId != TYPE_ID_BOOL) {
        Semantics::ReportSemanticWarning(
            "If expression is not a boolean type, applying implicit casting...");
        if (InsertTypeCastExpression(&statement->expression, statement->expression, TYPE_ID_BOOL)
            == -1) {
            Semantics::ReportSemanticError(
                "Unable to cast condition expression of If statement to boolean type");
            return -1;
        }
        expressionTypeId = TYPE_ID_BOOL;
    }
    auto trueScope = Scope::createInnerScope(GetCurrentScope(), "t");
    statement->trueBody->scopePtr = trueScope;
    Semantics::EnterScope(trueScope);
    // TODO: handle errors
    ProcessBodyElements(statement->trueBody);
    Semantics::ExitCurrentScope();
    if (statement->falseBody) {
        auto falseScope = Scope::createInnerScope(GetCurrentScope(), "f");
        statement->falseBody->scopePtr = falseScope;
        Semantics::EnterScope(falseScope);
        // TODO: handle errors
        ProcessBodyElements(statement->falseBody);
        Semantics::ExitCurrentScope();
    }
    return 0;
}

int ProcessForStatement(ForLoopStatement *statement) {
    int rangeLoTypeId = RunTypeTrace(statement->range->lo);
    if (rangeLoTypeId != TYPE_ID_INT) {
        Semantics::ReportSemanticWarning(
            "For expression range LO is not an integer type, applying implicit casting...");
        if (InsertTypeCastExpression(&statement->range->lo, statement->range->lo, TYPE_ID_INT)
            == -1) {
            Semantics::ReportSemanticError(
                "Unable to cast range LO expression of for loop to integer type");
            return -1;
        }
        rangeLoTypeId = TYPE_ID_INT;
    }
    int rangeHiTypeId = RunTypeTrace(statement->range->hi);
    if (rangeHiTypeId != TYPE_ID_INT) {
        Semantics::ReportSemanticWarning(
            "For expression range HI is not an integer type, applying implicit casting...");
        if (InsertTypeCastExpression(&statement->range->hi, statement->range->hi, TYPE_ID_INT)
            == -1) {
            Semantics::ReportSemanticError(
                "Unable to cast range HI expression of for loop to integer type");
            return -1;
        }
        rangeHiTypeId = TYPE_ID_INT;
    }
    auto bodyScope = Scope::createInnerScope(GetCurrentScope(), "b");
    auto counterType = bodyScope->types()->lookupType(TYPE_ID_INT);
    auto counterName = new Name(statement->identifier, counterType);
    bodyScope->names()->defineName(counterName);
    statement->body->scopePtr = bodyScope;
    Semantics::EnterScope(bodyScope);
    // TODO: handle errors
    ProcessBodyElements(statement->body);
    Semantics::ExitCurrentScope();
    return 0;
}

int ProcessWhileStatement(WhileLoopStatement *statement) {
    int expressionTypeId = RunTypeTrace(statement->expression);
    if (expressionTypeId != TYPE_ID_BOOL) {
        Semantics::ReportSemanticWarning(
            "While expression is not a boolean type, applying implicit casting...");
        if (InsertTypeCastExpression(&statement->expression, statement->expression, TYPE_ID_BOOL)
            == -1) {
            Semantics::ReportSemanticError(
                "Unable to cast condition expression of While statement to boolean type");
            return -1;
        }
        expressionTypeId = TYPE_ID_BOOL;
    }
    auto bodyScope = Scope::createInnerScope(GetCurrentScope(), "b");
    statement->body->scopePtr = bodyScope;
    Semantics::EnterScope(bodyScope);
    // TODO: handle errors
    ProcessBodyElements(statement->body);
    Semantics::ExitCurrentScope();
    return 0;
}

int ProcessBodyElement(AstNode *node) {
    if (node->type == AstNodeType::DECLARATION) {
        auto declaration = static_cast<Declaration *>(node);
        return ProcessSimpleDeclaration(declaration);
    }
    // only statements are expected to left at this point of code (due to grammar rules)
    auto statementNode = static_cast<Statement *>(node);
    switch (statementNode->statementType) {
        case StatementType::EXPRESSION_STATEMENT: {
            return ProcessExpressionStatement(static_cast<ExpressionStatement *>(statementNode));
        }
        case StatementType::RETURN_STATEMENT: {
            return ProcessReturnStatement(static_cast<ReturnStatement *>(statementNode));
        }
        case StatementType::IF_STATEMENT: {
            return ProcessIfStatement(static_cast<IfStatement *>(statementNode));
        }
        case StatementType::FOR_STATEMENT: {
            return ProcessForStatement(static_cast<ForLoopStatement *>(statementNode));
        }
        case StatementType::WHILE_STATEMENT: {
            return ProcessWhileStatement(static_cast<WhileLoopStatement *>(statementNode));
        }
    }
    Semantics::ReportSemanticError("ProcessBodyElement: function end, should not go here");
    return -1;
}

int ProcessBodyElements(Body *body) {
    for (AstNode *node : body->nodeList) {
        auto result = ProcessBodyElement(node);
        if (result != 0) {
            Semantics::ReportSemanticError(std::string("Unable to process body element for scope ")
                                           + GetCurrentScope()->getScopeName());
            node->isSemanticsValid = false;
        }
    }
    return 0;
}

int ProcessRoutineDeclaration(Declaration *declaration) {
    RoutineDeclaration *routineDeclaration = static_cast<RoutineDeclaration *>(declaration);
    auto routine = Semantics::GetGlobalScope()->funcs()->defineFunc(routineDeclaration);
    if (IsComponentTraced(COMPONENT_SEMANTICS)) {
        GetTraceStream(COMPONENT_SEMANTICS) << "scope " << GetCurrentScope()->getScopeName()
                                            << " declaration: " << routine->toString() << "\n";
    }
    currentFunction = routine;
    Semantics::EnterScope(routine->getScope());
    ProcessBodyElements(routineDeclaration->body);
    Semantics::ExitCurrentScope();
    currentFunction = routine;
    return 0;
}

void Semantics::RunSemanticPass(Program *program) {
    if (IsComponentTraced(COMPONENT_SEMANTICS)) {
        GetTraceStream(COMPONENT_SEMANTICS) << " << SEMANTIC PASS >>" << std::endl;
    }
    auto declarations = program->declarations;
    for (auto declaration : declarations) {
        if (declaration->declarationType == DeclarationType::TYPE_DECLARATION) {
            ProcessSimpleDeclaration(declaration);
            continue;
        }
        if (declaration->declarationType == DeclarationType::VARIABLE_DECLARATION) {
            ProcessSimpleDeclaration(declaration);
            continue;
        }
        if (declaration->declarationType == DeclarationType::ROUTINE_DECLARATION) {
            ProcessRoutineDeclaration(declaration);
            continue;
        }
    }
}