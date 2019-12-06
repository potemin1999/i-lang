/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Codegen.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "Config.h"
#include "Semantics.h"
#include "Utils.h"

using namespace Codegen;

const auto objectHeaderSize = 4;

FILE *moduleOutputFile;
llvm::LLVMContext *llvmContext = nullptr;
llvm::Module *llvmModule = nullptr;
llvm::IRBuilder<> *llvmBuilder = nullptr;
llvm::Function *currentLlvmFunction = nullptr;

// mapping from our types to llvm-declared types
std::map<TypeID, llvm::Type *> llvmTypesMap;
// pure size of types, except header for non-primitive types
std::map<TypeID, size_t> llvmTypeSizeMap;
// system function in the declaration way useful for further access
std::map<std::string, llvm::FunctionCallee> llvmSystemFunctions;

int GenerateScopeBody(Body *body, std::string currentInsertPoint);

void Codegen::InitCodegenContext(std::string moduleName, FILE *output) {
    if (!output) {
        output = stdout;
    }
    moduleOutputFile = output;
    llvmContext = new llvm::LLVMContext();
    llvmModule = new llvm::Module(moduleName, *llvmContext);
    llvmBuilder = new llvm::IRBuilder<>(*llvmContext);
}

void Codegen::DestroyCodegenContext() {
    llvmTypeSizeMap.clear();
    llvmTypesMap.clear();
    delete llvmBuilder;
    delete llvmModule;
    delete llvmContext;
}

llvm::Function *GetCurrentLlvmFunction() {
    return currentLlvmFunction;
}

std::map<TypeID, llvm::Type *> &Codegen::GetTypesMap() {
    return llvmTypesMap;
}

std::map<TypeID, size_t> &Codegen::GetTypesSizeMap() {
    return llvmTypeSizeMap;
}

int AddHeapSystemFunctionDefinitions() {
    // definition of malloc
    auto mallocParams = std::vector<llvm::Type *>();
    mallocParams.push_back(llvmBuilder->getInt32Ty()); // size to allocate
    auto mallocParamsRef = llvm::ArrayRef<llvm::Type *>(mallocParams);
    auto mallocType = llvm::FunctionType::get(llvmBuilder->getInt8PtrTy(), mallocParamsRef, false);
    llvmSystemFunctions["malloc"] = llvmModule->getOrInsertFunction("malloc", mallocType);

    // definition of free
    auto freeParams = std::vector<llvm::Type *>();
    freeParams.push_back(llvmBuilder->getInt8PtrTy());
    auto freeParamsRef = llvm::ArrayRef<llvm::Type *>(freeParams);
    auto freeType = llvm::FunctionType::get(llvmBuilder->getVoidTy(), freeParamsRef, false);
    llvmSystemFunctions["free"] = llvmModule->getOrInsertFunction("free", freeType);
    return 0;
}

int AddSystemFunctionDefinitions() {
    // definition of print (used in builtin functions)
    auto printfParams = std::vector<llvm::Type *>();
    printfParams.push_back(llvmBuilder->getInt8Ty()->getPointerTo());
    auto printfParamsRef = llvm::ArrayRef<llvm::Type *>(printfParams);
    auto printfType = llvm::FunctionType::get(llvmBuilder->getInt32Ty(), printfParamsRef, true);
    llvmSystemFunctions["printf"] = llvmModule->getOrInsertFunction("printf", printfType);
    return 0;
}

int AddBuiltinFunctionDefinitions() {
    { // print int function
        auto printSignature = "printInt";
        auto printParams = std::vector<llvm::Type *>();
        printParams.push_back(llvmBuilder->getInt32Ty());
        auto printRetType = llvm::Type::getVoidTy(*llvmContext);
        auto printFuncLinkage = llvm::Function::ExternalLinkage;
        auto printFuncType = llvm::FunctionType::get(printRetType, printParams, false);
        auto printFunc
            = llvm::Function::Create(printFuncType, printFuncLinkage, printSignature, llvmModule);
        // printFunc->addAttribute(1, llvm::Attribute::NoAlias);
        // printFunc->addAttribute(1, llvm::Attribute::StructRet);
        auto printEntry = llvm::BasicBlock::Create(*llvmContext, "entrypoint", printFunc);
        llvmBuilder->SetInsertPoint(printEntry);
        auto formatStringRef = llvm::StringRef("%d\n");
        auto formatString = llvmBuilder->CreateGlobalStringPtr(formatStringRef, "$_print_int_str");

        auto argsIterator = printFunc->arg_begin();
        auto arg1Value = static_cast<llvm::Value *>(argsIterator++);
        arg1Value->setName("_int_to_print");

        auto printfArgs = std::vector<llvm::Value *>();
        printfArgs.push_back(formatString);
        printfArgs.push_back(arg1Value);
        auto printfFunction = llvmSystemFunctions["printf"];
        llvmBuilder->CreateCall(printfFunction, printfArgs, "printf.ret");
        llvmBuilder->CreateRetVoid();
        Semantics::GetGlobalScope()->funcs()->lookupFunc("printInt")->setLLVMFunction(printFunc);
    }
    return 0;
}

int GenerateType(Semantics::Scope *scope, Semantics::Type *typeToGen);

int GenerateVariable(Semantics::Scope *scope, VariableDeclaration *varDeclaration) {
    if (varDeclaration->initialValue) {
        // initial value is set for all native types and their descendants
        InsertLLVMValues(llvmBuilder, scope, varDeclaration->initialValue);
        auto rLlvmValue = ValueOfExpression(varDeclaration->initialValue);
        Semantics::Name::ofVariableDeclaration(varDeclaration)->setLLVMValue(scope, rLlvmValue);
    } else {
        // for structs and arrays we should allocate space on the heap and save pointer
        auto userTypeId = varDeclaration->type->getTypeId();
        auto userType = scope->types()->lookupType(userTypeId);
        auto llvmType = llvmTypesMap[userTypeId];
        // call type constructor to get pointer to the allocated memory
        auto typeNewName = userType->getName() + "$<new>";
        auto typeNewParams = std::vector<llvm::Type *>();
        auto typeNewParamsRef = llvm::ArrayRef<llvm::Type *>(typeNewParams);
        auto typeNewType
            = llvm::FunctionType::get(llvmType->getPointerTo(), typeNewParamsRef, false);
        auto typeNewFunction = llvmModule->getOrInsertFunction(typeNewName, typeNewType);
        auto llvmValue = llvmBuilder->CreateCall(typeNewFunction);
        // auto llvmValue = llvmBuilder->CreateAlloca(llvmType, nullptr);
        Semantics::Name::ofVariableDeclaration(varDeclaration)->setLLVMValue(scope, llvmValue);
    }
    return 0;
};

int GenerateType(Semantics::Scope *scope, TypeDeclaration *typeDeclaration) {
    auto typeToGen = Semantics::Type::ofTypeDeclaration(typeDeclaration);
    return GenerateType(scope, typeToGen);
}

int GenerateSpecialFunctions(Semantics::Type *type, llvm::StructType *llvmType) {
    // generates constructor and destructor functions
    auto llvmTypeSize = llvmTypeSizeMap[type->getType()];
    auto llvmTypePtr = llvm::PointerType::get(llvmType, 0);
    auto llvmFuncLinkage = llvm::Function::ExternalLinkage;

    // constructor - returns initialized and allocated on the heap object with refCounter = 1
    auto llvmNewName = type->getName() + "$<new>";
    auto llvmNewParams = std::vector<llvm::Type *>();
    auto llvmNewParamsRef = llvm::ArrayRef<llvm::Type *>(llvmNewParams);
    auto llvmNewType = llvm::FunctionType::get(llvmTypePtr, llvmNewParamsRef, false);
    auto llvmNewFunc
        = llvm::Function::Create(llvmNewType, llvmFuncLinkage, llvmNewName, llvmModule);
    auto newEntry = llvm::BasicBlock::Create(*llvmContext, "entrypoint", llvmNewFunc);
    llvmBuilder->SetInsertPoint(newEntry);
    // now we are generating init function
    auto llvmMallocArgs = std::vector<llvm::Value *>();
    llvmMallocArgs.push_back(llvmBuilder->getInt32(llvmTypeSize));
    auto llvmMallocArgsRef = llvm::ArrayRef<llvm::Value *>(llvmMallocArgs);
    auto mallocRetVal
        = llvmBuilder->CreateCall(llvmSystemFunctions["malloc"], llvmMallocArgsRef, "raw_obj_ptr");
    auto objPtrValue = llvmBuilder->CreateBitCast(mallocRetVal, llvmTypePtr, "obj_ptr");
    if (type->isRecord()) {
        // initialize members one by one
        // auto offset = objectHeaderSize;
        auto members = type->getMembers();
        auto membersIterator = members->begin();
        auto membersIteratorEnd = members->end();
        auto currentOffset = 0;
        for (; membersIterator != membersIteratorEnd; ++membersIterator) {
            auto ptrName = "member_" + std::to_string(currentOffset) + "_ptr";
            auto memberPtr = llvmBuilder->CreateStructGEP(objPtrValue, currentOffset, ptrName);
            llvm::Value *memberDefaultValue = nullptr;
            // TODO: separate to the function
            switch (membersIterator->type->getType()) {
                case Semantics::TYPE_ID_INT: {
                    memberDefaultValue = llvmBuilder->getInt32(0);
                    break;
                }
                case Semantics::TYPE_ID_BOOL: {
                    memberDefaultValue = llvmBuilder->getInt8(0);
                    break;
                }
                case Semantics::TYPE_ID_FLOAT: {
                    memberDefaultValue
                        = llvm::ConstantFP::get(llvmTypesMap[Semantics::TYPE_ID_FLOAT], 0.0);
                    break;
                }
            }
            if (memberDefaultValue == nullptr) {
                // this should be non-primitive type
                auto memberLlvmType = llvmTypesMap[membersIterator->type->getType()];
                auto memberNewName = membersIterator->type->getName() + "$<new>";
                auto memberNewParams = std::vector<llvm::Type *>();
                auto memberNewParamsRef = llvm::ArrayRef<llvm::Type *>(memberNewParams);
                auto memberNewType = llvm::FunctionType::get(memberLlvmType->getPointerTo(),
                                                             memberNewParamsRef, false);
                auto memberNewFunc = llvmModule->getOrInsertFunction(memberNewName, memberNewType);
                auto defName = std::string("member_") + std::to_string(currentOffset) + "_val";
                memberDefaultValue = llvmBuilder->CreateCall(memberNewFunc, llvm::None, defName);
            }
            llvmBuilder->CreateStore(memberDefaultValue, memberPtr);
            currentOffset++;
        }
    } else {
        auto llvmSizeValue = ValueOfExpression(type->getArraySize());
        auto arraySizePtr = llvmBuilder->CreateStructGEP(objPtrValue, 0, "arr_size_ptr");
        llvmBuilder->CreateStore(llvmSizeValue, arraySizePtr);
        auto arrayPtr = llvmBuilder->CreateStructGEP(objPtrValue, 1, "arr_buf_ptr");

        auto llvmElementType = llvmTypesMap[type->getBaseType()]->getPointerTo();
        if (type->getBaseType() > 10) {
            llvmElementType = llvmElementType->getPointerTo();
        }
        auto llvmMallocBufferArgs = std::vector<llvm::Value *>();
        llvmMallocBufferArgs.push_back(llvmSizeValue);
        auto llvmMallocBufferArgsRef = llvm::ArrayRef<llvm::Value *>(llvmMallocBufferArgs);
        auto mallocRetBufRawVal = llvmBuilder->CreateCall(llvmSystemFunctions["malloc"],
                                                          llvmMallocBufferArgsRef, "raw_buf_ptr");
        auto bufPtrValue
            = llvmBuilder->CreateBitCast(mallocRetBufRawVal, llvmElementType, "buf_ptr");
        llvmBuilder->CreateStore(bufPtrValue, arrayPtr);
        // TODO: initialize each member of array
    }
    llvmBuilder->CreateRet(objPtrValue);

    // generate destructor - accepts pointer to the object and deallocates it and its non-primitive
    // members
    auto llvmDelName = type->getName() + "$<del>";
    auto llvmDelParams = std::vector<llvm::Type *>();
    llvmDelParams.push_back(llvmTypePtr);
    auto llvmDelParamsRef = llvm::ArrayRef<llvm::Type *>(llvmDelParams);
    auto llvmDelType = llvm::FunctionType::get(llvmBuilder->getVoidTy(), llvmDelParamsRef, false);
    auto llvmDelFunc
        = llvm::Function::Create(llvmDelType, llvmFuncLinkage, llvmDelName, llvmModule);
    auto delArgsIterator = llvmDelFunc->arg_begin();
    auto delObjValue = static_cast<llvm::Value *>(delArgsIterator++);
    auto delEntry = llvm::BasicBlock::Create(*llvmContext, "entrypoint", llvmDelFunc);
    llvmBuilder->SetInsertPoint(delEntry);
    // TODO: check for null pointer and return immediately if null
    if (type->isRecord()) {
        auto members = type->getMembers();
        auto membersIterator = members->begin();
        auto membersIteratorEnd = members->end();
        auto currentOffset = 0;
        for (; membersIterator != membersIteratorEnd; ++membersIterator, currentOffset++) {
            if (membersIterator->type->isPrimitive()) {
                continue;
            }
            auto ptrName = "member_" + std::to_string(currentOffset) + "_ptr";
            auto memberPtr = llvmBuilder->CreateStructGEP(delObjValue, currentOffset, ptrName);
            auto memberValue = llvmBuilder->CreateLoad(memberPtr);

            auto memberLlvmType = llvmTypesMap[membersIterator->type->getType()];
            auto memberDelName = membersIterator->type->getName() + "$<del>";
            auto memberDelParams = std::vector<llvm::Type *>();
            memberDelParams.push_back(memberLlvmType->getPointerTo());
            auto memberDelParamsRef = llvm::ArrayRef<llvm::Type *>(memberDelParams);
            auto memberDelType
                = llvm::FunctionType::get(llvmBuilder->getVoidTy(), memberDelParamsRef, false);
            auto memberDelFunc = llvmModule->getOrInsertFunction(memberDelName, memberDelType);
            auto delArgs = std::vector<llvm::Value *>();
            delArgs.push_back(memberValue);
            llvmBuilder->CreateCall(memberDelFunc, delArgs);
        }
    } else {
        // TODO: free array resources
    }
    llvmBuilder->CreateRetVoid();
    return 0;
}

int GenerateType(Semantics::Scope *scope, Semantics::Type *typeToGen) {
    if (typeToGen->isPrimitive() && typeToGen->getNativeType() != -1) {
        // this is native type, it was casted to llvm native type
        return 0;
    }
    // this is struct or array
    if (typeToGen->isRecord()) {
        // TODO: utils fields for struct
        auto memberTypes = std::vector<llvm::Type *>();
        auto memberList = typeToGen->getMembers();
        auto memberListIterator = memberList->begin();
        auto memberListIteratorEnd = memberList->end();
        for (; memberListIterator != memberListIteratorEnd; ++memberListIterator) {
            auto memberType = scope->types()->lookupType(memberListIterator->type->getType());
            auto memberLlvmType = llvmTypesMap[memberType->getType()];
            if (memberType->isPrimitive()) {
                memberTypes.push_back(memberLlvmType);
            } else {
                memberTypes.push_back(llvm::PointerType::get(memberLlvmType, 0));
            }
        }
        auto llvmStructType
            = llvm::StructType::create(*llvmContext, memberTypes, typeToGen->getName());
        auto typeId = typeToGen->getType();
        // auto llvmStructPtrType = llvm::PointerType::get(llvmStructType, 0);
        llvmTypesMap[typeId] = llvmStructType;
        // TODO: fix
        llvmTypeSizeMap[typeId] = typeToGen->getSize();
        return GenerateSpecialFunctions(typeToGen, llvmStructType);
    }
    if (typeToGen->isArray()) {
        auto memberTypes = std::vector<llvm::Type *>();
        InsertLLVMValues(llvmBuilder, scope, typeToGen->getArraySize());
        auto elementTypeId = typeToGen->getBaseType();
        auto elementTypeLlvm = llvmTypesMap[elementTypeId];
        memberTypes.push_back(llvmBuilder->getInt32Ty());
        auto elementType = scope->types()->lookupType(elementTypeId);
        if (elementType->isPrimitive()) {
            memberTypes.push_back(elementTypeLlvm->getPointerTo());
        } else {
            memberTypes.push_back(elementTypeLlvm->getPointerTo()->getPointerTo());
        }
        auto llvmArrayType
            = llvm::StructType::create(*llvmContext, memberTypes, typeToGen->getName());
        auto arrayTypeId = typeToGen->getType();
        llvmTypesMap[arrayTypeId] = llvmArrayType;
        llvmTypeSizeMap[arrayTypeId] = 4 + 8;
        return GenerateSpecialFunctions(typeToGen, llvmArrayType);
    }
    return 0;
}

int GenerateExpressionStatement(Semantics::Scope *scope, ExpressionStatement *statement) {
    if (statement->expression->isBinary) {
        // Only binary expression which can be contained is assignment operation
        return InsertLLVMValues(llvmBuilder, scope, statement->expression);
    }
    if (statement->expression->isPrimary) {
        // Only callable primary are allowed to be written
        return InsertLLVMValues(llvmBuilder, scope,
                                static_cast<CallablePrimary *>(statement->expression));
    }
    ASSERT_SHOULD_NOT_GO_HERE()
    return -1;
}

int GenerateReturnStatement(Semantics::Scope *scope, ReturnStatement *statement) {
    InsertLLVMValues(llvmBuilder, scope, statement->expressionToReturn);
    llvmBuilder->CreateRet(ValueOfExpression(statement->expressionToReturn));
    return 0;
}

int GenerateIfStatement(Semantics::Scope *scope, IfStatement *statement) {
    auto ifBeginningBlock = llvmBuilder->GetInsertBlock();
    InsertLLVMValues(llvmBuilder, scope, statement->expression);
    auto expressionValue = ValueOfExpression(statement->expression);
    auto trueName
        = scope->getScopeName() + "_t_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto trueBlock = llvm::BasicBlock::Create(*llvmContext, trueName, GetCurrentLlvmFunction());
    llvm::BasicBlock *endBlock, *endTrueBlock = nullptr, *endFalseBlock = nullptr;
    llvm::BranchInst *branchInstruction = nullptr;
    llvm::BasicBlock *falseBlock = nullptr;
    std::string falseName;
    if (statement->falseBody) {
        auto falseCounter = GetAndIncrementUniqueCounter(SCOPE_COUNT);
        auto endCounter = GetAndIncrementUniqueCounter(SCOPE_COUNT);
        auto endName = scope->getScopeName() + "_" + std::to_string(endCounter);
        endBlock = llvm::BasicBlock::Create(*llvmContext, endName, GetCurrentLlvmFunction());
        falseName = scope->getScopeName() + "_f_" + std::to_string(falseCounter);
        falseBlock
            = llvm::BasicBlock::Create(*llvmContext, falseName, GetCurrentLlvmFunction(), endBlock);
        branchInstruction = llvmBuilder->CreateCondBr(expressionValue, trueBlock, falseBlock);
    } else {
        auto endName = scope->getScopeName() + "_"
                       + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
        endBlock = llvm::BasicBlock::Create(*llvmContext, endName, GetCurrentLlvmFunction());
        branchInstruction = llvmBuilder->CreateCondBr(expressionValue, trueBlock, endBlock);
    }
    llvmBuilder->SetInsertPoint(trueBlock);
    GenerateScopeBody(statement->trueBody, trueName);
    llvmBuilder->CreateBr(endBlock);
    endTrueBlock = llvmBuilder->GetInsertBlock();
    if (statement->falseBody) {
        llvmBuilder->SetInsertPoint(falseBlock);
        GenerateScopeBody(statement->falseBody, falseName);
        llvmBuilder->CreateBr(endBlock);
        endFalseBlock = llvmBuilder->GetInsertBlock();
    }
    llvmBuilder->SetInsertPoint(endBlock);
    // TODO: phi all parent scope values
    auto valueUpdateFunc = [=](llvm::PHINode *phi, Semantics::Name *fallthroughName) {
        bool fromThisScope = false;
        auto nameToFall = scope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        if (nameToFall != fallthroughName) {
            ASSERT_SHOULD_NOT_GO_HERE()
        }
        nameToFall->setLLVMValue(scope, static_cast<llvm::Value *>(phi));
        if (!fromThisScope) {
            // add this changed value to parent scope fallthrough values
            scope->insertNameFallthrough(fallthroughName);
        }
    };
    // std::vector < std::pair<Semantics::Name *, llvm::PHINode *> phiList;
    auto trueScope = Semantics::Scope::ofBody(statement->trueBody);
    auto trueCallbacksList = trueScope->getAllFallthroughNames();
    if (statement->falseBody) {
        // variable could be changed in both branches, handle this
        auto falseScope = Semantics::Scope::ofBody(statement->falseBody);
        auto falseCallbacksList = falseScope->getAllFallthroughNames();
        std::map<Semantics::Name *, bool> trueCallbackBitmap;
        std::map<Semantics::Name *, bool> falseCallbackBitmap;
        for (auto name : trueCallbacksList) {
            trueCallbackBitmap[name] = true;
        }
        for (auto name : falseCallbacksList) {
            falseCallbackBitmap[name] = true;
        }
        // TODO: generate phi for parent scope and block
        // check true only changed variables, and both changed
        for (auto fallthroughName : trueCallbacksList) {
            auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
            auto trueValue = fallthroughName->getLLVMValue(trueScope);
            ASSERT_NOT_NULL(trueValue)
            auto phi = llvmBuilder->CreatePHI(nameType, 2);
            if (falseCallbackBitmap.count(fallthroughName) > 0) {
                // variable has been changed in both branches
                auto falseValue = fallthroughName->getLLVMValue(falseScope);
                phi->addIncoming(trueValue, endTrueBlock);
                phi->addIncoming(falseValue, falseBlock);
            } else {
                // variable has been changed in true scope only
                auto parentValue = fallthroughName->getLLVMValue(scope);
                phi->addIncoming(parentValue, ifBeginningBlock);
                phi->addIncoming(trueValue, endTrueBlock);
            }
            valueUpdateFunc(phi, fallthroughName);
        }
        // check false only changed variables
        for (auto fallthroughName : trueCallbacksList) {
            if (trueCallbackBitmap.count(fallthroughName) < 1) {
                // variable has been changed in false scope only, otherwise it was handled earlier
                auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
                auto falseValue = fallthroughName->getLLVMValue(falseScope);
                ASSERT_NOT_NULL(falseValue)
                auto parentValue = fallthroughName->getLLVMValue(scope);
                auto phi = llvmBuilder->CreatePHI(nameType, 2);
                phi->addIncoming(parentValue, ifBeginningBlock);
                phi->addIncoming(falseValue, endFalseBlock);
                valueUpdateFunc(phi, fallthroughName);
            }
        }
    } else {
        // this if statement does not have false body
        for (auto fallthroughName : trueCallbacksList) {
            // variable has been changed in true scope only
            auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
            auto trueValue = fallthroughName->getLLVMValue(trueScope);
            auto parentValue = fallthroughName->getLLVMValue(scope);
            auto phi = llvmBuilder->CreatePHI(nameType, 2);
            phi->addIncoming(parentValue, ifBeginningBlock);
            phi->addIncoming(trueValue, endTrueBlock);
            valueUpdateFunc(phi, fallthroughName);
        }
    }
    return 0;
}

int GenerateWhileLoop(Semantics::Scope *scope, WhileLoopStatement *statement) {
    auto beginningBlock = llvmBuilder->GetInsertBlock();
    auto expressionBlockName
        = scope->getScopeName() + "_e_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto expressionBlock
        = llvm::BasicBlock::Create(*llvmContext, expressionBlockName, GetCurrentLlvmFunction());
    auto bodyBlockName
        = scope->getScopeName() + "_b_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto bodyBlock
        = llvm::BasicBlock::Create(*llvmContext, bodyBlockName, GetCurrentLlvmFunction());
    auto endBlockName
        = scope->getScopeName() + "_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto endBlock = llvm::BasicBlock::Create(*llvmContext, endBlockName, GetCurrentLlvmFunction());

    llvmBuilder->CreateBr(expressionBlock);

    // generate body first to found out which outer scope variables are affected inside the loop
    llvmBuilder->SetInsertPoint(bodyBlock);
    llvmBuilder->ClearInsertionPoint();
    GenerateScopeBody(statement->body, bodyBlockName + "_stub");
    llvmBuilder->ClearInsertionPoint();

    auto bodyScope = Semantics::Scope::ofBody(statement->body);
    auto bodyCallbacksList = bodyScope->getAllFallthroughNames();

    // generate name fallthrough and apply them to the expression
    std::map<Semantics::Name *, llvm::PHINode *> fallthroughValues;
    llvmBuilder->SetInsertPoint(expressionBlock);
    for (auto fallthroughName : bodyCallbacksList) {
        // generate phi function
        auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
        // auto bodyValue = fallthroughName->getLLVMValue(bodyScope);
        auto parentValue = fallthroughName->getLLVMValue(scope);
        auto phi = llvmBuilder->CreatePHI(nameType, 2);
        phi->addIncoming(parentValue, beginningBlock);
        // phi->addIncoming(bodyValue, bodyBlock);
        fallthroughValues[fallthroughName] = phi;

        bool fromThisScope = false;
        auto nameToFall
            = bodyScope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        nameToFall->setLLVMValue(bodyScope, static_cast<llvm::Value *>(phi));
    }
    // calculate the expression
    InsertLLVMValues(llvmBuilder, bodyScope, statement->expression);
    auto expressionValue = ValueOfExpression(statement->expression);
    llvmBuilder->CreateCondBr(expressionValue, bodyBlock, endBlock);

    // generate body
    llvmBuilder->SetInsertPoint(bodyBlock);
    GenerateScopeBody(statement->body, bodyBlockName);
    for (auto fallthroughName : bodyCallbacksList) {
        // auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
        auto bodyValue = fallthroughName->getLLVMValue(bodyScope);
        fallthroughValues[fallthroughName]->addIncoming(bodyValue, bodyBlock);
        bool fromThisScope = false;
        auto nameToFall
            = bodyScope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        nameToFall->setLLVMValue(bodyScope, bodyValue);
        if (!fromThisScope) {
            // add this changed value to parent scope fallthrough values
            bodyScope->insertNameFallthrough(fallthroughName);
        }
    }
    llvmBuilder->CreateBr(expressionBlock);

    // finalize variable fallthrough
    llvmBuilder->SetInsertPoint(endBlock);
    for (auto fallthroughName : bodyCallbacksList) {
        auto phiValue = fallthroughValues[fallthroughName];
        bool fromThisScope = false;
        auto nameToFall = scope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        nameToFall->setLLVMValue(scope, phiValue);
        if (!fromThisScope) {
            // add this changed value to parent scope fallthrough values
            scope->insertNameFallthrough(fallthroughName);
        }
    }
    return 0;
}

int GenerateForLoop(Semantics::Scope *scope, ForLoopStatement *statement) {
    auto beginningBlock = llvmBuilder->GetInsertBlock();

    // for statement does not require explicit expression block
    auto bodyBlockName
        = scope->getScopeName() + "_b_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto bodyBlock
        = llvm::BasicBlock::Create(*llvmContext, bodyBlockName, GetCurrentLlvmFunction());
    auto endBlockName
        = scope->getScopeName() + "_" + std::to_string(GetAndIncrementUniqueCounter(SCOPE_COUNT));
    auto endBlock = llvm::BasicBlock::Create(*llvmContext, endBlockName, GetCurrentLlvmFunction());

    InsertLLVMValues(llvmBuilder, scope, statement->range->lo);
    auto loValue = ValueOfExpression(statement->range->lo);
    InsertLLVMValues(llvmBuilder, scope, statement->range->hi);
    auto hiValue = ValueOfExpression(statement->range->hi);

    llvmBuilder->CreateBr(bodyBlock);

    // run variable change trace
    llvmBuilder->SetInsertPoint(bodyBlock);
    llvmBuilder->ClearInsertionPoint();
    auto bodyScope = Semantics::Scope::ofBody(statement->body);
    auto counterScopeName = bodyScope->names()->lookupName(statement->identifier);
    counterScopeName->setLLVMValue(bodyScope, llvmBuilder->getInt32(1));
    GenerateScopeBody(statement->body, bodyBlockName);
    // llvmBuilder->ClearInsertionPoint();

    llvmBuilder->SetInsertPoint(bodyBlock);
    auto bodyCallbacksList = bodyScope->getAllFallthroughNames();
    std::map<Semantics::Name *, llvm::PHINode *> fallthroughValues;
    for (auto fallthroughName : bodyCallbacksList) {
        auto nameType = GetTypesMap()[fallthroughName->getType()->getType()];
        auto bodyValue = fallthroughName->getLLVMValue(bodyScope);
        auto parentValue = fallthroughName->getLLVMValue(scope);
        auto phi = llvmBuilder->CreatePHI(nameType, 2);
        phi->addIncoming(parentValue, beginningBlock);
        phi->addIncoming(bodyValue, bodyBlock);
        fallthroughValues[fallthroughName] = phi;

        bool fromThisScope = false;
        auto nameToFall
            = bodyScope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        nameToFall->setLLVMValue(bodyScope, bodyValue);
        if (!fromThisScope) {
            // add this changed value to parent scope fallthrough values
            bodyScope->insertNameFallthrough(fallthroughName);
        }
    }
    auto counterPhi = llvmBuilder->CreatePHI(llvmTypesMap[Semantics::TYPE_ID_INT], 2);
    counterPhi->addIncoming(loValue, beginningBlock);
    counterScopeName->setLLVMValue(bodyScope, counterPhi);
    GenerateScopeBody(statement->body, bodyBlockName);
    llvm::Value *newCounterValue = nullptr;

    if (!statement->reverse) {
        newCounterValue = llvmBuilder->CreateAdd(counterPhi, llvmBuilder->getInt32(1));
    } else {
        newCounterValue = llvmBuilder->CreateSub(counterPhi, llvmBuilder->getInt32(1));
    }
    counterPhi->addIncoming(newCounterValue, bodyBlock);
    auto exitCond = llvmBuilder->CreateICmpEQ(newCounterValue, hiValue);
    llvmBuilder->CreateCondBr(exitCond, endBlock, bodyBlock);

    llvmBuilder->SetInsertPoint(endBlock);
    for (auto fallthroughName : bodyCallbacksList) {
        auto phiValue = fallthroughValues[fallthroughName];
        bool fromThisScope = false;
        auto nameToFall = scope->names()->lookupName(fallthroughName->getName(), &fromThisScope);
        nameToFall->setLLVMValue(scope, phiValue);
        if (!fromThisScope) {
            // add this changed value to parent scope fallthrough values
            scope->insertNameFallthrough(fallthroughName);
        }
    }
    return 0;
}

int GenerateScopeBody(Body *body, std::string currentInsertPoint) {
    auto bodyScope = Semantics::Scope::ofBody(body);
    for (auto node : body->nodeList) {
        if (!node->isSemanticsValid) {
            Semantics::ReportSemanticError("node of scope " + bodyScope->getScopeName()
                                           + " is no be skipped due to semantic error");
            continue;
        }
        if (node->type == AstNodeType::DECLARATION) {
            auto declaration = node->toDeclaration();
            switch (declaration->declarationType) {
                case DeclarationType::VARIABLE_DECLARATION: {
                    GenerateVariable(bodyScope, declaration->toVariableDeclaration());
                    break;
                }
                    // case DeclarationType::TYPE_DECLARATION: {
                    //    GenerateType(bodyScope, declaration->toTypeDeclaration());
                    //}
            }
        }
        if (node->type == AstNodeType::STATEMENT) {
            auto statement = node->toStatement();
            switch (statement->statementType) {
                case StatementType::EXPRESSION_STATEMENT: {
                    GenerateExpressionStatement(bodyScope,
                                                static_cast<ExpressionStatement *>(statement));
                    break;
                }
                case StatementType::RETURN_STATEMENT: {
                    GenerateReturnStatement(bodyScope, static_cast<ReturnStatement *>(statement));
                    break;
                }
                case StatementType::IF_STATEMENT: {
                    GenerateIfStatement(bodyScope, static_cast<IfStatement *>(statement));
                    break;
                }
                case StatementType ::WHILE_STATEMENT: {
                    GenerateWhileLoop(bodyScope, static_cast<WhileLoopStatement *>(statement));
                    break;
                }
                case StatementType ::FOR_STATEMENT: {
                    GenerateForLoop(bodyScope, static_cast<ForLoopStatement *>(statement));
                    break;
                }
            }
        }
    }
    return 0;
}

int GenerateFunctionDeclaration(RoutineDeclaration *routineDeclaration) {
    // auto globalFunctionTable = Semantics::GetGlobalScope()->funcs();
    // auto globalFunction = globalFunctionTable->lookupFunc(routineDeclaration->routineName);
    if (IsComponentTraced(COMPONENT_CODEGEN)) {
        GetTraceStream(COMPONENT_CODEGEN)
            << "generating function "
            << Semantics::Function::ofRoutineDeclaration(routineDeclaration)->toString() << "\n";
    }
    auto globalFunction = Semantics::Function::ofRoutineDeclaration(routineDeclaration);
    auto globalFunctionScope = globalFunction->getScope();
    auto llvmFuncName = globalFunction->getName();

    // get parameter types
    auto llvmFuncParams = std::vector<llvm::Type *>();
    for (auto &param : globalFunction->getAllParameters()) {
        auto primitive = true;
        auto rootParamType = Semantics::FindNativeBaseType(param.type->getType());
        if (rootParamType == -1) {
            // TODO: non-primitive type?
            rootParamType = param.type->getType();
            primitive = false;
        }
        auto llvmParamType = llvmTypesMap[rootParamType];
        if (primitive) {
            llvmFuncParams.push_back(llvmParamType);
        } else {
            llvmFuncParams.push_back(llvmParamType->getPointerTo());
        }
    }
    auto llvmParamsRef = llvm::ArrayRef<llvm::Type *>(llvmFuncParams);

    // get return type
    // TODO: handle non-primitive return types
    auto returnPrimitive = true;
    auto returnTypeId = Semantics::FindNativeBaseType(globalFunction->getReturnTypeId());
    if (returnTypeId == -1) {
        returnTypeId = globalFunction->getReturnTypeId();
        returnPrimitive = false;
    }
    auto llvmRetType = llvmTypesMap[returnTypeId];
    if (!returnPrimitive) {
        llvmRetType = llvmRetType->getPointerTo();
    }
    auto llvmFuncType = llvm::FunctionType::get(llvmRetType, llvmParamsRef, false);
    auto llvmFuncLinkage = llvm::Function::ExternalLinkage;
    auto llvmFunc = llvm::Function::Create(llvmFuncType, llvmFuncLinkage, llvmFuncName, llvmModule);
    Semantics::Function::ofRoutineDeclaration(routineDeclaration)->setLLVMFunction(llvmFunc);
    auto funcEntryPoint = llvm::BasicBlock::Create(*llvmContext, "entrypoint", llvmFunc);
    llvmBuilder->SetInsertPoint(funcEntryPoint);
    currentLlvmFunction = llvmFunc;
    //    auto argsIterator = llvmFunc->arg_begin();
    //    auto argsSize = globalFunction->getAllParameters().size();
    //    for (auto &param : globalFunction->getAllParameters()) {
    //        auto argName = globalFunctionScope->names()->lookupName(param.name);
    //        argName->getType()->
    //    }
    //    auto arg1Value = static_cast<llvm::Value *>(argsIterator++);

    // process body
    auto bodyGenRes = GenerateScopeBody(routineDeclaration->body, "entrypoint");
    if (globalFunction->getReturnTypeId() == Semantics::TYPE_ID_VOID) {
        llvmBuilder->CreateRetVoid();
    }
    currentLlvmFunction = nullptr;
    return bodyGenRes;
}

void Codegen::RunCodegenPass(Program *program) {
    // fetch current native type mapping
    if (IsComponentTraced(COMPONENT_CODEGEN)) {
        GetTraceStream(COMPONENT_CODEGEN) << " << CODEGEN PASS >>" << std::endl;
    }
    llvmTypesMap[Semantics::TYPE_ID_VOID] = static_cast<llvm::Type *>(llvmBuilder->getVoidTy());
    llvmTypesMap[Semantics::TYPE_ID_BOOL] = static_cast<llvm::Type *>(llvmBuilder->getInt1Ty());
    llvmTypesMap[Semantics::TYPE_ID_INT] = static_cast<llvm::Type *>(llvmBuilder->getInt32Ty());
    llvmTypesMap[Semantics::TYPE_ID_FLOAT] = static_cast<llvm::Type *>(llvmBuilder->getFloatTy());
    llvmTypeSizeMap[Semantics::TYPE_ID_VOID] = 0;
    llvmTypeSizeMap[Semantics::TYPE_ID_BOOL] = 1;
    llvmTypeSizeMap[Semantics::TYPE_ID_INT] = 4;
    llvmTypeSizeMap[Semantics::TYPE_ID_FLOAT] = 4;

    AddSystemFunctionDefinitions();
    AddBuiltinFunctionDefinitions();
    // generate definitions for user defined types
    auto areSystemFunctionsDefined = false;
    auto userTypes = Semantics::GetUserTypesList();
    for (auto userType : userTypes) {
        // TODO: fix scopes, not global only
        if (!areSystemFunctionsDefined) {
            AddHeapSystemFunctionDefinitions();
            areSystemFunctionsDefined = true;
        }
        GenerateType(Semantics::GetGlobalScope(), userType);
    }
    // run over function declarations
    for (auto node : program->declarations) {
        // TODO: global variable and type declarations
        // if (node->declarationType == DeclarationType::TYPE_DECLARATION) {
        //    GenerateType(Semantics::GetGlobalScope(), node->toTypeDeclaration());
        //}
        if (node->declarationType == DeclarationType::VARIABLE_DECLARATION) {
            GenerateVariable(Semantics::GetGlobalScope(), node->toVariableDeclaration());
        }
        if (node->declarationType == DeclarationType::ROUTINE_DECLARATION) {
            GenerateFunctionDeclaration(node->toRoutineDeclaration());
        }
    }

    // print the result
    llvmModule->print(llvm::outs(), nullptr);

    int fd = fileno(moduleOutputFile);
    auto moduleOStream = llvm::raw_fd_ostream(fd, true, false);
    const auto &moduleRef = *llvmModule;
    llvm::WriteBitcodeToFile(moduleRef, moduleOStream);
    llvmSystemFunctions.clear();
}

void Codegen::ReportCodegenWarning(std::string msg) {
    std::cerr << "[ CODEGEN ] WARN  : " << msg << std::endl;
}

void Codegen::ReportCodegenError(std::string msg) {
    std::cerr << "[ CODEGEN ] ERROR : " << msg << std::endl;
}
