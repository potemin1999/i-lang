/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Semantics.h"
#include "Tree.h"
#include "Utils.h"
#include "generated/Parser.tab.hpp"

using namespace Semantics;

bool isSingleCallStatement = false;

void Semantics::SetCallStatementStatus(bool isSingleCall) {
    isSingleCallStatement = isSingleCall;
}

TypeID Semantics::IncrementAndGetTypeID() {
    static TypeID counter = 10;
    return ++counter;
}

TypeID Semantics::FindNativeBaseType(TypeID typeId) {
    auto typeTable = GetCurrentScope()->types();
    auto type = typeTable->lookupType(typeId);
    // ensure exists
    for (;;) {
        if (type == nullptr) {
            Semantics::ReportSemanticError("Native base type lookup failed: logic error");
            return -2;
        }
        if (type->isRecord()) {
            // records have no native type in the base
            return -1;
        }
        if (type->isNative()) {
            return type->getType();
        }
        type = typeTable->lookupType(type->getBaseType());
    }
}

TypeID RunPrimaryTypeTrace(Primary *primary) {
    if (primary->isConst) {
        auto constPrimary = static_cast<ConstPrimary *>(primary);
        switch (constPrimary->type) {
            case INTEGER_PRIMARY_TYPE: return TYPE_ID_INT;
            case REAL_PRIMARY_TYPE: return TYPE_ID_FLOAT;
            case BOOLEAN_PRIMARY_TYPE: return TYPE_ID_BOOL;
        }
        Semantics::ReportSemanticError(std::string("Unable to trace type for primary of type ")
                                       + std::to_string(constPrimary->type));
        return -1;
    }
    if (primary->isCall) {
        auto callablePrimary = static_cast<CallablePrimary *>(primary);
        auto func = GetCurrentScope()->funcs()->lookupFunc(callablePrimary->identifier);
        if (func == nullptr) {
            Semantics::ReportSemanticError(std::string("Function ") + callablePrimary->identifier
                                           + " is not defined");
            return -1;
        }
        auto paramsList = func->getAllParameters();
        auto argList = callablePrimary->argumentList->expressionList;
        auto argIterator = callablePrimary->argumentList->expressionList.begin();
        auto argIteratorEnd = callablePrimary->argumentList->expressionList.end();
        int counter = 1;
        while (argIterator != argIteratorEnd) {
            auto argExpressionRef = &(*argIterator);
            auto argExpression = *argIterator;
            auto argType = RunTypeTrace(argExpression);
            if (argType == -1) {
                Semantics::ReportSemanticError(
                    std::string("Function call ") + callablePrimary->identifier
                    + " can not be performed with invalid type for argument "
                    + std::to_string(counter));
                return -1;
            }
            auto paramType = paramsList[counter - 1].type->getType();
            if (paramType != argType) {
                auto res
                    = InsertTypeCastExpression(argExpressionRef, argExpression, paramType, false);
                if (res == -1) {
                    Semantics::ReportSemanticError(
                        std::string("Function call ") + callablePrimary->identifier
                        + " can not be performed with argument " + std::to_string(counter)
                        + " of non-conforming type");
                    return -1;
                } else {
                    Semantics::ReportSemanticWarning(
                        std::string("Applying implicit casting while function call: ")
                        + func->getName());
                }
            }
            ++argIterator;
            counter++;
        }
        if (func->getReturnTypeId() == TYPE_ID_VOID) {
            if (isSingleCallStatement) {
                return TYPE_ID_VOID;
            }
            Semantics::ReportSemanticError(
                "Function " + func->toString()
                + " does not returns value and cannot be used as operand");
            return -1;
        }
        return func->getReturnTypeId();
    }
    auto nameTable = GetCurrentScope()->names();
    auto modifiablePrimary = static_cast<ModifiablePrimary *>(primary);
    //    if (modifiablePrimary->index != nullptr) {
    //        auto indexType = RunTypeTrace(modifiablePrimary->index);
    //        if (indexType == -1) {
    //            Semantics::ReportSemanticError("unable to trace type for array size");
    //            return -1;
    //        }
    //        if (indexType != TYPE_ID_INT) {
    //            Semantics::ReportSemanticWarning("array index should be of type integer");
    //            InsertTypeCastExpression(&modifiablePrimary->index, modifiablePrimary->index,
    //                                     TYPE_ID_INT, false);
    //        }
    //        auto name = nameTable->lookupName(modifiablePrimary->identifier);
    //        auto elementTypeId = name->getType()->getBaseType();
    //        return elementTypeId;
    //    }
    if (modifiablePrimary->index || modifiablePrimary->member) {
        // this is struct member
        auto name = nameTable->lookupName(modifiablePrimary->identifier);
        auto currentPrimary = modifiablePrimary;
        auto currentType = name->getType();
        while (currentPrimary && (currentPrimary->member || currentPrimary->index)) {
            if (currentPrimary->index) {
                auto indexType = RunTypeTrace(currentPrimary->index);
                if (indexType == -1) {
                    Semantics::ReportSemanticError("unable to trace type for array size");
                    return -1;
                }
                if (indexType != TYPE_ID_INT) {
                    Semantics::ReportSemanticWarning("array index should be of type integer");
                    InsertTypeCastExpression(&modifiablePrimary->index, modifiablePrimary->index,
                                             TYPE_ID_INT, false);
                }
                auto elementType
                    = GetCurrentScope()->types()->lookupType(currentType->getBaseType());
                currentType = elementType;
            }
            if (currentPrimary->member) {
                auto member = currentType->getMemberByName(currentPrimary->member->identifier);
                currentType = member->type;
            }
            currentPrimary = currentPrimary->member;
        }
        return currentType->getType();
    } else {
        // this is a plain variable
        auto name = nameTable->lookupName(modifiablePrimary->identifier);
        if (name == nullptr) {
            Semantics::ReportSemanticError(std::string("No such name \"")
                                           + modifiablePrimary->identifier + "\" in scope "
                                           + GetCurrentScope()->getScopeName());
            return -1;
        }
        return name->getType()->getType();
    }
    Semantics::ReportSemanticError("Types.cpp : trace missing case");
    return -1;
}

TypeID RunUnaryTypeTrace(UnaryExpression *unaryExpression) {
    unaryExpression->operand->expressionType = RunTypeTrace(unaryExpression->operand);
    return 0;
}

TypeID RunBinaryTypeTrace(BinaryExpression *binaryExpression) {
    RunTypeTrace(binaryExpression->operand1);
    RunTypeTrace(binaryExpression->operand2);
    if (binaryExpression->oper == OP_ASSIGNMENT) {
        if (binaryExpression->operand1->expressionType
            != binaryExpression->operand2->expressionType) {
            return InsertTypeCastExpression(&binaryExpression->operand2, binaryExpression->operand2,
                                            binaryExpression->operand2->expressionType,
                                            binaryExpression->operand1->expressionType, false);
        } else {
            return binaryExpression->operand1->expressionType;
        }
    }
    TypeID commonTypeId = -1;
    if (binaryExpression->operand1->expressionType == binaryExpression->operand2->expressionType) {
        commonTypeId = binaryExpression->operand1->expressionType;
    } else {
        commonTypeId = ApplyTypeWidening(binaryExpression);
    }
    switch (binaryExpression->oper) {
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_EQUAL:
        case OP_NOT_EQUAL:
        case OP_GREATER:
        case OP_GREATER_OR_EQUAL:
        case OP_LESS:
        case OP_LESS_OR_EQUAL: return TYPE_ID_BOOL;
        default: return commonTypeId;
    }
}

TypeID Semantics::RunTypeTrace(Expression *expressionNode) {
    // TODO: write type id into expression field using its children, literals and current scope
    // TODO: insert cast nodes to make code generator insert implicit type casting operations
    if (expressionNode->expressionType != -1) {
        // type have already been computed, return it
        return expressionNode->expressionType;
    }
    if (expressionNode->isBinary) {
        auto binaryExpression = static_cast<BinaryExpression *>(expressionNode);
        binaryExpression->expressionType = RunBinaryTypeTrace(binaryExpression);
        return binaryExpression->expressionType;
    }
    if (expressionNode->isPrimary) {
        auto primaryExpression = static_cast<Primary *>(expressionNode);
        primaryExpression->expressionType = RunPrimaryTypeTrace(primaryExpression);
        return primaryExpression->expressionType;
    } else {
        auto unaryExpression = static_cast<UnaryExpression *>(expressionNode);
        unaryExpression->expressionType = RunUnaryTypeTrace(unaryExpression);
        return unaryExpression->expressionType;
    }
}

TypeID Semantics::InsertTypeCastExpression(Expression **refFromParent, Expression *child,
                                           TypeID targetType, bool isExplicit) {
    auto childSourceType = RunTypeTrace(child);
    if (childSourceType == -1) {
        return -1;
    }
    auto nativeChildType = FindNativeBaseType(childSourceType);
    if (nativeChildType == -1) {
        Semantics::ReportSemanticError("Non-primitive types can not participate in type casting");
        return -1;
    }
    return Semantics::InsertTypeCastExpression(refFromParent, child, nativeChildType, targetType,
                                               isExplicit);
}

TypeID Semantics::InsertTypeCastExpression(Expression **refFromParent, Expression *child,
                                           TypeID sourceType, TypeID targetType, bool isExplicit) {
    int castOperator = 0;
    if (sourceType == TYPE_ID_BOOL && targetType == TYPE_ID_INT) {
        castOperator = OP_CAST_BOOL_TO_INT;
    } else if (sourceType == TYPE_ID_BOOL && targetType == TYPE_ID_FLOAT) {
        castOperator = OP_CAST_BOOL_TO_FLOAT;
    } else if (sourceType == TYPE_ID_INT && targetType == TYPE_ID_FLOAT) {
        castOperator = OP_CAST_INT_TO_FLOAT;
    }
    if (castOperator == 0) {
        if (!isExplicit) {
            // TODO: add expression position
            Semantics::ReportSemanticWarning(
                "Implicit type casting requires type narrowing, beware of bugs");
        }
        if (sourceType == TYPE_ID_FLOAT && targetType == TYPE_ID_INT) {
            castOperator = OP_CAST_FLOAT_TO_INT;
        } else if (sourceType == TYPE_ID_FLOAT && targetType == TYPE_ID_BOOL) {
            castOperator = OP_CAST_FLOAT_TO_BOOL;
        } else if (sourceType == TYPE_ID_INT && targetType == TYPE_ID_BOOL) {
            castOperator = OP_CAST_INT_TO_BOOL;
        }
    }
    if (castOperator == 0) {
        ASSERT_SHOULD_NOT_GO_HERE()
    }
    auto castExpression = new UnaryExpression(castOperator, child);
    castExpression->expressionType = targetType;
    *refFromParent = castExpression;
    return targetType;
}

TypeID Semantics::ApplyTypeWidening(BinaryExpression *binaryExpression) {
    auto left = binaryExpression->operand1;
    auto right = binaryExpression->operand2;
    auto leftType = RunTypeTrace(left);
    auto rightType = RunTypeTrace(right);
    leftType = FindNativeBaseType(leftType);
    rightType = FindNativeBaseType(rightType);
    if (leftType == -1 || rightType == -1) {
        auto type1 = GetCurrentScope()->types()->lookupType(left->expressionType);
        auto type2 = GetCurrentScope()->types()->lookupType(right->expressionType);
        Semantics::ReportSemanticError(
            std::string("Unable to perform implicit type casting for non-native derived types ")
            + type1->toString() + " and " + type2->toString());
        return -1;
    }
    if (leftType == rightType) {

        return leftType;
    }
    auto lRef = &binaryExpression->operand1;
    auto rRef = &binaryExpression->operand2;
    if (leftType == TYPE_ID_INT) {
        if (rightType == TYPE_ID_FLOAT) {
            return InsertTypeCastExpression(lRef, left, TYPE_ID_INT, TYPE_ID_FLOAT, false);
        }
        if (rightType == TYPE_ID_BOOL) {
            return InsertTypeCastExpression(rRef, right, TYPE_ID_BOOL, TYPE_ID_INT, false);
        }
        ASSERT_SHOULD_NOT_GO_HERE()
    }
    if (leftType == TYPE_ID_FLOAT) {
        if (rightType == TYPE_ID_INT) {
            return InsertTypeCastExpression(rRef, right, TYPE_ID_INT, TYPE_ID_FLOAT, false);
        }
        if (rightType == TYPE_ID_BOOL) {
            return InsertTypeCastExpression(rRef, right, TYPE_ID_BOOL, TYPE_ID_FLOAT, false);
        }
        ASSERT_SHOULD_NOT_GO_HERE()
    }
    if (leftType == TYPE_ID_BOOL) {
        if (rightType == TYPE_ID_INT) {
            return InsertTypeCastExpression(lRef, left, TYPE_ID_BOOL, TYPE_ID_INT, false);
        }
        if (rightType == TYPE_ID_FLOAT) {
            return InsertTypeCastExpression(lRef, left, TYPE_ID_BOOL, TYPE_ID_FLOAT, false);
        }
        ASSERT_SHOULD_NOT_GO_HERE()
    }
    ASSERT_SHOULD_NOT_GO_HERE()
    // auto typeTable = GetCurrentScope()->types();
    // ASSERT_SHOULD_NOT_GO_HERE()
    //    auto intType = typeTable->lookupType("int");
    //    auto realType = typeTable->lookupType("real");
    //    auto boolType = typeTable->lookupType("bool");
}

TypeID Semantics::ApplyTypeWidening(UnaryExpression *unaryExpression) {}