/**
 * Created by Ilya Potemin on 10/19/19.
 */

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include "testing/Testing.h"

/**
 * Call of external functions, such as printf
 * Use of constant string variables
 * Constant/literal declaration
 * Bitcode emission
 */
UNIT_TEST(LLVM_Test_1) {
    llvm::LLVMContext context;
    auto module = llvm::Module("test_module_1", context);
    auto builder = llvm::IRBuilder<>(context);
    auto mainFuncType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    auto mainFuncLinkage = llvm::Function::ExternalLinkage;
    auto mainFunc = llvm::Function::Create(mainFuncType, mainFuncLinkage, "main", module);
    auto mainEntry = llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
    builder.SetInsertPoint(mainEntry);

    auto helloWorldRef = llvm::StringRef("hello world! %f %d\n");
    auto helloWorld = builder.CreateGlobalStringPtr(helloWorldRef, "hw_const");

    auto printfParams = std::vector<llvm::Type *>();
    printfParams.push_back(builder.getInt8Ty()->getPointerTo());
    auto printfParamsRef = llvm::ArrayRef<llvm::Type *>(printfParams);
    auto printfType = llvm::FunctionType::get(builder.getInt32Ty(), printfParamsRef, true);
    auto printfFunc = module.getOrInsertFunction("printf", printfType);

    const auto intReturnValue = 42;
    const auto fpRetValue = 42.0;
    auto fpValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), fpRetValue);
    auto retValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), intReturnValue);

    // auto printfArgs = std::vector<llvm::Value *>();
    llvm::Value *printfArgs[3];
    printfArgs[0] = helloWorld;
    printfArgs[1] = fpValue;
    printfArgs[2] = retValue;

    builder.CreateCall(printfFunc, printfArgs, "printf_result");
    builder.CreateRet(retValue);
    module.print(llvm::outs(), nullptr);

    std::error_code errorCode;
    auto moduleOStream = llvm::raw_fd_ostream("testLLVM.bc", errorCode);
    const auto &moduleRef = module;
    llvm::WriteBitcodeToFile(moduleRef, moduleOStream);
}

/**
 * Creation of struct/record of type HW
 * Implementation of function-getter for record instance of type HW
 * NRVO (Named Return Value Optimization) is applied
 */
UNIT_TEST(LLVM_SimpleStruct) {
    llvm::LLVMContext context;
    auto module = llvm::Module("test_module_2", context);
    auto builder = llvm::IRBuilder<>(context);

    auto structMemberTypes = std::vector<llvm::Type *>();
    structMemberTypes.push_back(llvm::Type::getInt32Ty(context));
    structMemberTypes.push_back(llvm::Type::getFloatTy(context));
    structMemberTypes.push_back(llvm::Type::getDoubleTy(context));
    auto structType = llvm::StructType::create(context, structMemberTypes, "record.HW");
    auto structPtrType = llvm::PointerType::get(structType, 0);

    auto printfParams = std::vector<llvm::Type *>();
    printfParams.push_back(builder.getInt8Ty()->getPointerTo());
    auto printfParamsRef = llvm::ArrayRef<llvm::Type *>(printfParams);
    auto printfType = llvm::FunctionType::get(builder.getInt32Ty(), printfParamsRef, true);
    auto printfFunc = module.getOrInsertFunction("printf", printfType);

    auto printSignature = "print";
    auto printParams = std::vector<llvm::Type *>();
    printParams.push_back(structPtrType);
    auto printRetType = llvm::Type::getVoidTy(context);
    auto printFuncLinkage = llvm::Function::ExternalLinkage;
    auto printFuncType = llvm::FunctionType::get(printRetType, printParams, false);
    auto printFunc
        = llvm::Function::Create(printFuncType, printFuncLinkage, printSignature, module);
    printFunc->addAttribute(1, llvm::Attribute::NoAlias);
    printFunc->addAttribute(1, llvm::Attribute::StructRet);
    auto printEntry = llvm::BasicBlock::Create(context, "entrypoint", printFunc);
    builder.SetInsertPoint(printEntry);

    auto formatStringRef = llvm::StringRef("hello world! %f\n");
    auto formatString = builder.CreateGlobalStringPtr(formatStringRef, "hw_const");

    auto argsIterator = printFunc->arg_begin();
    auto arg1Value = static_cast<llvm::Value *>(argsIterator++);
    arg1Value->setName("_ret");
    auto structOffsetValues = std::vector<llvm::Value *>();
    const auto intBitWidth = 32;
    // llvm::APInt zeroInt(intBitWidth,0);
    llvm::APInt oneInt(intBitWidth, 1);
    // structOffsetValues.push_back(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context),zeroInt));
    structOffsetValues.push_back(
        llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), oneInt));
    auto arg1APtrValue = builder.CreateGEP(arg1Value, structOffsetValues, "HW.fp_ptr");
    auto arg1AValue = builder.CreateLoad(arg1APtrValue, "HW.fp");
    auto printfArgs = std::vector<llvm::Value *>();
    printfArgs.push_back(formatString);
    printfArgs.push_back(arg1AValue);
    builder.CreateCall(printfFunc, printfArgs, "printf.ret");
    builder.CreateRetVoid();

    auto mainFuncType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    auto mainFuncLinkage = llvm::Function::ExternalLinkage;
    auto mainFunc = llvm::Function::Create(mainFuncType, mainFuncLinkage, "main", module);
    auto mainEntry = llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
    builder.SetInsertPoint(mainEntry);
    auto structAlloca = builder.CreateAlloca(structType, nullptr);
    auto printArgs = std::vector<llvm::Value *>();
    printArgs.push_back(structAlloca);
    builder.CreateCall(printFunc, printArgs);

    auto mainRetValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    builder.CreateRet(mainRetValue);

    module.print(llvm::outs(), nullptr);
}
