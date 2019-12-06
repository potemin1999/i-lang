/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Codegen.h"
#include "Semantics.h"
#include "Tree.h"
#include "Utils.h"
#include "generated/Parser.tab.hpp"

using namespace Codegen;

llvm::Value *Codegen::ValueOfExpression(Expression *expression) {
    return static_cast<llvm::Value *>(expression->llvmValuePtr);
}

llvm::Value *GenerateAccessVector(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                                  ModifiablePrimary *primary) {
    auto topLevelName = scope->names()->lookupName(primary->identifier);
    ASSERT_NOT_NULL(topLevelName);
    auto topLevelType = topLevelName->getType();
    auto topLevelTypeLlvm = GetTypesMap()[topLevelType->getType()];
    // llvm::Value *topLevelNameLlvmPtr = nullptr;
    auto topNameScopeOfVar = scope;
    if (topLevelName->isParameter()) {
        auto funcOwner = topLevelName->getParameterOwner();
        auto llvmFuncOwner = funcOwner->getLLVMFunction();
        // TODO: prepare before function body creation
        auto argIterator = llvmFuncOwner->arg_begin();
        for (int i = 0; i < funcOwner->getAllParameters().size(); i++) {
            auto llvmArg = static_cast<llvm::Value *>(argIterator++);
            auto funcArg = funcOwner->getAllParameters()[i];
            if (funcArg.name->getName() == topLevelName->getName()) {
                primary->llvmValuePtr = llvmArg;
                break;
            }
        }
    } else {
        do {
            primary->llvmValuePtr = topLevelName->getLLVMValue(topNameScopeOfVar);
            topNameScopeOfVar = topNameScopeOfVar->parent();
        } while (primary->llvmValuePtr == nullptr && topNameScopeOfVar != nullptr);
    }
    if (!primary->llvmValuePtr) {
        ASSERT_SHOULD_NOT_GO_HERE()
    }
    auto topLevelNamePtr = ValueOfExpression(primary);
    auto currentLlvmValPtr = topLevelNamePtr;
    auto currentLlvmType = topLevelTypeLlvm;
    auto currentType = topLevelType;
    auto currentPrimary = primary;
    for (;;) {
        if (currentPrimary->index) {
            // get array buf pointer
            auto accessBufArray = std::vector<llvm::Value *>();
            accessBufArray.push_back(builder->getInt32(0));
            accessBufArray.push_back(builder->getInt32(1));
            auto bufPtrRef = builder->CreateGEP(currentLlvmValPtr, accessBufArray);
            auto bufPtr = builder->CreateLoad(bufPtrRef);

            // generate index value
            auto accessElemArray = std::vector<llvm::Value *>();
            currentLlvmValPtr = bufPtr;
            InsertLLVMValues(builder, scope, currentPrimary->index);
            auto indexValue = ValueOfExpression(currentPrimary->index);
            currentType = scope->types()->lookupType(currentType->getBaseType());
            currentLlvmType = indexValue->getType();
            // nextLlvmType = indexValue->getType();
            // accessElemArray.push_back(builder->getInt32(0));
            accessElemArray.push_back(indexValue);
            // get location of element under the index value
            currentLlvmValPtr = builder->CreateGEP(bufPtr, accessElemArray);
        }
        if (currentPrimary->member) {
            if (currentPrimary->index) {
                currentLlvmValPtr = builder->CreateLoad(currentLlvmValPtr);
            }
            // find index of member in the struct
            auto memberName = currentPrimary->member->identifier;
            auto memberField = currentType->getMemberByName(memberName);
            auto memberType = memberField->type;
            auto memberLlvmType = GetTypesMap()[memberType->getType()];

            // generate access to the field
            auto accessVectorArray = std::vector<llvm::Value *>();
            accessVectorArray.push_back(builder->getInt32(0));
            accessVectorArray.push_back(builder->getInt32(memberField->index));
            auto accessVectorArrRef = llvm::ArrayRef<llvm::Value *>(accessVectorArray);
            currentLlvmValPtr = builder->CreateGEP(currentLlvmValPtr, accessVectorArrRef);

            ASSERT_NOT_NULL(currentType);
            ASSERT_NOT_NULL(currentLlvmType);
            currentLlvmType = memberLlvmType;
            currentType = memberType;
        }
        auto nextPrimary = currentPrimary->member;
        if (nextPrimary == nullptr) {
            return currentLlvmValPtr;
        }
        if ((nextPrimary->index == nullptr) && (nextPrimary->member == nullptr)) {
            // this is the end of primary sequence
            return currentLlvmValPtr;
        } else {
            currentLlvmValPtr = builder->CreateLoad(currentLlvmValPtr);
            currentPrimary = nextPrimary;
            // currentLlvmType = currentLlvmValPtr->getType();
            continue;
        }
        return currentLlvmValPtr;
        // currentLlvmValPtr = nextValPtr;
    }
    return currentLlvmValPtr;
}

int GenerateMemberWrite(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                        ModifiablePrimary *primary, llvm::Value *value) {
    auto targetPtr = GenerateAccessVector(builder, scope, primary);
    builder->CreateStore(value, targetPtr, false);
    return 0;
}

llvm::Value *GenerateMemberRead(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                                ModifiablePrimary *primary) {
    auto targetPtr = GenerateAccessVector(builder, scope, primary);
    return builder->CreateLoad(targetPtr);
}

int InsertLLVMAssignment(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                         BinaryExpression *expression) {
    InsertLLVMValues(builder, scope, expression->operand2);
    auto rValue = ValueOfExpression(expression->operand2);
    ASSERT_TRUE(expression->oper == OP_ASSIGNMENT)
    auto modifiableLValue = static_cast<ModifiablePrimary *>(expression->operand1);
    if (!modifiableLValue->isAccessMember && modifiableLValue->index == nullptr) {
        // simple variables are stored into registers
        auto isFromThis = true;
        auto name = scope->names()->lookupName(modifiableLValue->identifier, &isFromThis);
        ASSERT_NOT_NULL(name)
        name->setLLVMValue(scope, rValue);
        if (!isFromThis) {
            scope->insertNameFallthrough(name);
        }
        return 0;
    }
    // lValue is not a primitive type
    return GenerateMemberWrite(builder, scope, modifiableLValue, rValue);
}

int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                     BinaryExpression *expression) {
    if (expression->oper == OP_ASSIGNMENT) {
        return InsertLLVMAssignment(builder, scope, expression);
    }
    auto nativeTypesMap = GetTypesMap();
    InsertLLVMValues(builder, scope, expression->operand1);
    InsertLLVMValues(builder, scope, expression->operand2);
    auto lValue = ValueOfExpression(expression->operand1);
    auto rValue = ValueOfExpression(expression->operand2);
    llvm::Value *resultValue = nullptr;
    if (expression->expressionType == Semantics::TYPE_ID_INT) {
        switch (expression->oper) {
            case OP_PLUS: {
                resultValue = builder->CreateAdd(lValue, rValue);
                break;
            }
            case OP_MINUS: {
                resultValue = builder->CreateSub(lValue, rValue);
                break;
            }
            case OP_MULTIPLY: {
                resultValue = builder->CreateMul(lValue, rValue);
                break;
            }
            case OP_DIVIDE: {
                resultValue = builder->CreateSDiv(lValue, rValue);
                break;
            }
        }
    } else if (expression->expressionType == Semantics::TYPE_ID_FLOAT) {
        switch (expression->oper) {
            case OP_PLUS: {
                resultValue = builder->CreateFAdd(lValue, rValue);
                break;
            }
            case OP_MINUS: {
                resultValue = builder->CreateFSub(lValue, rValue);
                break;
            }
            case OP_MULTIPLY: {
                resultValue = builder->CreateFMul(lValue, rValue);
                break;
            }
            case OP_DIVIDE: {
                resultValue = builder->CreateFDiv(lValue, rValue);
                break;
            }
        }
    } else if (expression->expressionType == Semantics::TYPE_ID_BOOL) {
        switch (expression->oper) {
            case OP_OR:
            case OP_PLUS: {
                resultValue = builder->CreateOr(lValue, rValue);
                break;
            }
            case OP_AND:
            case OP_MULTIPLY: {
                resultValue = builder->CreateAnd(lValue, rValue);
                break;
            }
            case OP_XOR: {
                resultValue = builder->CreateXor(lValue, rValue);
                break;
            }
            case OP_GREATER: {
                // they are guaranteed to have the same types
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpSGT(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpOGT(lValue, rValue);
                        break;
                    }
                }
                break;
            }
            case OP_GREATER_OR_EQUAL: {
                // they are guaranteed to have the same types
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpSGE(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpOGE(lValue, rValue);
                        break;
                    }
                }
                break;
            }
            case OP_LESS: {
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpSLT(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpOLT(lValue, rValue);
                        break;
                    }
                }
                break;
            };
            case OP_LESS_OR_EQUAL: {
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpSLE(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpOLE(lValue, rValue);
                        break;
                    }
                }
                break;
            }
            case OP_EQUAL: {
                // they are guaranteed to have the same types
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpEQ(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpOEQ(lValue, rValue);
                        break;
                    }
                }
                break;
            }
            case OP_NOT_EQUAL: {
                // they are guaranteed to have the same types
                switch (expression->operand1->expressionType) {
                    case Semantics::TYPE_ID_INT: {
                        resultValue = builder->CreateICmpNE(lValue, rValue);
                        break;
                    }
                    case Semantics::TYPE_ID_FLOAT: {
                        resultValue = builder->CreateFCmpONE(lValue, rValue);
                        break;
                    }
                }
                break;
            }
        }
    }
    ASSERT_NOT_NULL(resultValue);
    expression->llvmValuePtr = resultValue;
    return 0;
}

int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                     UnaryExpression *expression) {
    InsertLLVMValues(builder, scope, expression->operand);
    auto operandValue = ValueOfExpression(expression->operand);
    auto nativeTypesMap = GetTypesMap();
    switch (expression->oper) {
        case Semantics::OP_CAST_INT_TO_FLOAT: {
            expression->llvmValuePtr
                = builder->CreateCast(llvm::Instruction::CastOps::SIToFP, operandValue,
                                      nativeTypesMap[Semantics::TYPE_ID_FLOAT]);
            break;
        }
        case Semantics::OP_CAST_FLOAT_TO_INT: {
            expression->llvmValuePtr
                = builder->CreateCast(llvm::Instruction::CastOps::FPToSI, operandValue,
                                      nativeTypesMap[Semantics::TYPE_ID_INT]);
            break;
        }
        case Semantics::OP_CAST_INT_TO_BOOL: {
            expression->llvmValuePtr
                = builder->CreateCast(llvm::Instruction::CastOps::Trunc, operandValue,
                                      nativeTypesMap[Semantics::TYPE_ID_BOOL]);
            break;
        }
        case Semantics::OP_CAST_FLOAT_TO_BOOL: {
            auto intermediateIntValue
                = builder->CreateCast(llvm::Instruction::CastOps::FPToSI, operandValue,
                                      nativeTypesMap[Semantics::TYPE_ID_BOOL]);
            expression->llvmValuePtr
                = builder->CreateCast(llvm::Instruction::CastOps::Trunc, intermediateIntValue,
                                      nativeTypesMap[Semantics::TYPE_ID_BOOL]);
            break;
        }
        default: {
            ASSERT_SHOULD_NOT_GO_HERE()
        }
    }
    return 0;
}

int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                     ConstPrimary *constPrimary) {
    auto constType = GetTypesMap()[constPrimary->expressionType];
    ASSERT_NOT_NULL(constType);
    switch (constPrimary->expressionType) {
        case Semantics::TYPE_ID_INT: {
            constPrimary->llvmValuePtr = llvm::ConstantInt::get(constType, constPrimary->intValue);
            break;
        }
        case Semantics::TYPE_ID_FLOAT: {
            constPrimary->llvmValuePtr = llvm::ConstantFP::get(constType, constPrimary->realValue);
            break;
        }
        case Semantics::TYPE_ID_BOOL: {
            constPrimary->llvmValuePtr = llvm::ConstantInt::get(constType, constPrimary->boolValue);
            break;
        }
        default: {
            ASSERT_SHOULD_NOT_GO_HERE()
        }
    }
    return 0;
}

int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                     ModifiablePrimary *primary) {
    auto name = scope->names()->lookupName(primary->identifier);
    // if name is a parameter, lookup its value in the table

    // name is not a parameter
    if (primary->member == nullptr && primary->index == nullptr) {
        // simple variable
        if (name->isParameter()) {
            auto funcOwner = name->getParameterOwner();
            auto llvmFuncOwner = funcOwner->getLLVMFunction();
            // TODO: prepare before function body creation
            auto argIterator = llvmFuncOwner->arg_begin();
            for (int i = 0; i < funcOwner->getAllParameters().size(); i++) {
                auto llvmArg = static_cast<llvm::Value *>(argIterator++);
                auto funcArg = funcOwner->getAllParameters()[i];
                if (funcArg.name->getName() == name->getName()) {
                    primary->llvmValuePtr = llvmArg;
                    break;
                }
            }
            if (primary->llvmValuePtr != nullptr) {
                return 0;
            }
        } else {
            auto scopeOfVar = scope;
            do {
                primary->llvmValuePtr = name->getLLVMValue(scopeOfVar);
                scopeOfVar = scopeOfVar->parent();
            } while (primary->llvmValuePtr == nullptr && scopeOfVar != nullptr);
            if (!primary->llvmValuePtr) {
                ASSERT_SHOULD_NOT_GO_HERE()
            }
            // TODO: handle var not found, if needed
            return 0;
        }
    } else {
        // non-primitive type
        primary->llvmValuePtr = GenerateMemberRead(builder, scope, primary);
        return 0;
    }
    return -1;
}

int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                     CallablePrimary *primary) {
    auto func = scope->funcs()->lookupFunc(primary->identifier);
    auto llvmFuncToCall = func->getLLVMFunction();
    // fetch parameter values
    auto llvmFuncArgs = std::vector<llvm::Value *>();
    for (auto expression : primary->argumentList->expressionList) {
        Codegen::InsertLLVMValues(builder, scope, expression);
        llvmFuncArgs.push_back(ValueOfExpression(expression));
    }
    auto callInstr = builder->CreateCall(llvmFuncToCall, llvmFuncArgs);
    primary->llvmValuePtr = static_cast<llvm::Value *>(callInstr);
    return 0;
}

int Codegen::InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope,
                              Expression *expression) {
    if (!expression->isPrimary) {
        if (expression->isBinary) {
            return InsertLLVMValues(builder, scope, static_cast<BinaryExpression *>(expression));
        } else {
            return InsertLLVMValues(builder, scope, static_cast<UnaryExpression *>(expression));
        }
    } else {
        auto primaryExpression = static_cast<Primary *>(expression);
        if (primaryExpression->isConst) {
            return InsertLLVMValues(builder, scope, static_cast<ConstPrimary *>(expression));
        }
        if (primaryExpression->isCall) {
            return InsertLLVMValues(builder, scope, static_cast<CallablePrimary *>(expression));
        } else {
            return InsertLLVMValues(builder, scope, static_cast<ModifiablePrimary *>(expression));
        }
    }
}
