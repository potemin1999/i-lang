/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Tree.h"
#include <generated/Parser.tab.hpp>
#include <iostream>
#include "Config.h"
#include "Semantics.h"
#include "Utils.h"

using namespace Semantics;

Statement *AstNode::toStatement() {
    return static_cast<Statement *>(this);
}

Declaration *AstNode::toDeclaration() {
    return static_cast<Declaration *>(this);
}

Program::~Program() {
    for (auto declaration : declarations) {
        delete declaration;
    }
}

Declaration::~Declaration() = default;

VariableDeclaration *Declaration::toVariableDeclaration() {
    if (declarationType == VARIABLE_DECLARATION) {
        return static_cast<VariableDeclaration *>(this);
    } else {
        return nullptr;
    }
}

TypeDeclaration *Declaration::toTypeDeclaration() {
    if (declarationType == TYPE_DECLARATION) {
        return static_cast<TypeDeclaration *>(this);
    } else {
        return nullptr;
    }
}

RoutineDeclaration *Declaration::toRoutineDeclaration() {
    if (declarationType == ROUTINE_DECLARATION) {
        return static_cast<RoutineDeclaration *>(this);
    } else {
        return nullptr;
    }
}

VariableDeclaration::~VariableDeclaration() {
    std::free(varName);
    delete type;
    delete initialValue;
}

VariableDeclarationList::~VariableDeclarationList() {
    for (auto declaration : variableDeclarationList) {
        delete declaration;
    }
}

ParameterDeclaration::~ParameterDeclaration() {
    std::free(identifier);
    std::free(typeName);
}

ParameterList::~ParameterList() {
    for (auto declaration : parameterList) {
        delete declaration;
    }
}

RoutineDeclaration::~RoutineDeclaration() {
    std::free(routineName);
    delete parameters;
    delete returnType;
    delete body;
}

TypeDeclaration::~TypeDeclaration() {
    std::free(typeName);
    delete typeDefinition;
}

TypeDefinition::~TypeDefinition() = default;

NamedTypeDefinition::~NamedTypeDefinition() {
    std::free(name);
}

RecordTypeDefinition::~RecordTypeDefinition() {
    delete variableDeclarationList;
}

ArrayTypeDefinition::~ArrayTypeDefinition() {
    delete sizeExpression;
    delete arrayElementType;
}

Statement::~Statement() = default;

ExpressionStatement::~ExpressionStatement() {
    delete expression;
}

ReturnStatement::~ReturnStatement() {
    delete expressionToReturn;
}

ArgumentList::~ArgumentList() {
    for (auto expression : expressionList) {
        delete expression;
    }
}

Body::~Body() {
    for (auto node : nodeList) {
        delete node;
    }
}

WhileLoopStatement::~WhileLoopStatement() {
    delete expression;
    delete body;
}

Range::~Range() {
    delete lo;
    delete hi;
}

ForLoopStatement::~ForLoopStatement() {
    std::free(identifier);
    delete range;
    delete body;
}

IfStatement::~IfStatement() {
    delete expression;
    delete trueBody;
    delete falseBody;
}

Expression::~Expression() = default;

UnaryExpression::~UnaryExpression() {
    delete operand;
}

BinaryExpression::~BinaryExpression() {
    delete operand1;
    delete operand2;
}

Primary::~Primary() = default;

ConstPrimary::~ConstPrimary() = default;

ModifiablePrimary::~ModifiablePrimary() {
    std::free(identifier);
    if (isAccessMember) {
        delete member;
    } else {
        delete index;
    }
}

CallablePrimary::~CallablePrimary() {
    delete modifiablePrimary;
    delete argumentList;
}

Program *programModule = nullptr;

void SetProgramTree(Program *program) {
    programModule = program;
}

Program *GetProgramTree() {
    return programModule;
}

// TODO: next three functions should also lazy load their types into type table
TypeID NamedTypeDefinition::obtainTypeId() {
    // this type should be either native or loaded already loaded
    auto baseType = GetCurrentScope()->types()->lookupType(this->name);
    if (baseType == nullptr) {
        ReportSemanticError(std::string("Name type definition failed: unable to lookup base type ")
                            + this->name);
        return -1;
    }
    return baseType->getType();
}
NamedTypeDefinition::NamedTypeDefinition(int yaccKeyword) : TypeDefinition(true) {
    switch (yaccKeyword) {
        case BOOLEAN: {
            STRDUP("bool", &name)
            break;
        }
        case INTEGER: {
            STRDUP("int", &name)
            break;
        }
        case REAL: {
            STRDUP("float", &name)
            break;
        }
    }
}

TypeID RecordTypeDefinition::obtainTypeId() {
    std::string prefix
        = GetCurrentScope() != GetGlobalScope() ? GetCurrentScope()->getScopeName() + "$" : "";
    auto typeNameToGen
        = prefix + "AnonymousStruct_" + std::to_string(GetAndIncrementUniqueCounter(TYPE_COUNT));
    auto type = GetCurrentScope()->types()->defineType(typeNameToGen, this);
    ASSERT_NOT_NULL(type)
    return type->getType();
}

TypeID ArrayTypeDefinition::obtainTypeId() {
    // TODO: load type definition into the type table and return generated id
    return 0;
}

void Body::onTypeDeclared(TypeDeclaration *declaration) {
    // use lazy type loading via getter
    if (declaration->typeDefinition->getTypeId() == -1) {
        // TODO: type was unable to load itself into table, process this error
    }
}

void Body::onVariableDeclared(VariableDeclaration *declaration) {
    // TODO: load name into function local name table
}

void Body::onStatementAdded(Statement *statement) {
    // TODO: start expression type tracing and scope allocation (if needed)
}

void ReturnStatement::traceExpressionTypes() {
    // TODO: trace return expression type
}

void WhileLoopStatement::traceExpressionTypes() {
    // TODO: trace type of the condition expression
}

void ForLoopStatement::traceExpressionTypes() {
    // TODO: trace type of the condition expression
}

void IfStatement::traceExpressionTypes() {
    // TODO: trace type of the condition expression
}
