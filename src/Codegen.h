/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#ifndef CC_PROJECT_CODEGEN_H
#define CC_PROJECT_CODEGEN_H

#include <llvm/IR/IRBuilder.h>
#include "Semantics.h"
#include "Tree.h"

namespace Codegen {

void InitCodegenContext(std::string moduleName, FILE *output);
void DestroyCodegenContext();
void RunCodegenPass(Program *program);
std::map<TypeID, llvm::Type *> &GetTypesMap();
std::map<TypeID, size_t> &GetTypesSizeMap();

llvm::Value *ValueOfExpression(Expression *expression);

// traverses tree of expressions and adds llvm::Value* to each node in context of builder
int InsertLLVMValues(llvm::IRBuilder<> *builder, Semantics::Scope *scope, Expression *expression);

void ReportCodegenWarning(std::string msg);
void ReportCodegenError(std::string msg);

} // namespace Codegen

#endif // CC_PROJECT_CODEGEN_H
