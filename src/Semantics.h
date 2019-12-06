/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#ifndef CC_PROJECT_SEMANTICS_H
#define CC_PROJECT_SEMANTICS_H

#include <iostream>
#include <llvm/IR/Function.h>
#include <map>
#include <string>
#include <vector>
#include "Tree.h"

namespace Semantics {

const int TYPE_ID_VOID = 0;
const int TYPE_ID_BOOL = 1;
const int TYPE_ID_INT = 2;
const int TYPE_ID_FLOAT = 3;

// type widening operations
const int OP_CAST_BOOL_TO_FLOAT = 10;
const int OP_CAST_BOOL_TO_INT = 11;
const int OP_CAST_INT_TO_FLOAT = 12;
// type narrowing operations
const int OP_CAST_FLOAT_TO_INT = 13;
const int OP_CAST_INT_TO_BOOL = 14;
const int OP_CAST_FLOAT_TO_BOOL = 15;

enum SpecialFunctionType {
    CONSTRUCTOR_FUNC,
    DESTRUCTOR_FUNC,
};

struct Type;
struct Name;
struct Function;
class Scope;

// this struct describes struct member
struct RecordField {
    // struct type to which this field belongs
    Type *parent;
    // type of the field
    Type *type;
    // index of field inside struct declaration
    int index;
    // name of the field
    std::string name;
};

TypeID FindNativeBaseType(TypeID typeId);

struct Type {
    friend class TypeTable;

  protected:
    // if this type will be passed by reference or by value
    // structs and arrays are passed by reference (false)
    // integer, real, bool are passed by value (true)
    bool primitive{false};
    // if type is not primitive, then it can be array or record
    bool array{false};
    // if type is an alias of primitive type, then this is set to false
    // only default primitive types are native ones
    bool native{false};
    // name of the type
    // can be generated, if type is used anonymous
    std::string name;
    // size of the type in bytes
    int size;
    // type given id
    TypeID typeId{-1};
    // for primitive non-native types this stores parent primitive type (maybe native)
    // for arrays stores array element type
    // for struct types is not defined
    TypeID baseTypeId{-1};
    // for struct types stores member description
    // not defined for other types
    union {
        // primitive and native types
        struct {
            // for primitive user-defined types this field stores root native type
            TypeID nativeTypeID{-2};
        };
        // struct types
        struct {
            std::map<std::string, RecordField *> *membersMap;
            std::vector<RecordField> *members;
        };
        // array types
        struct {
            Expression *arraySize;
            // int arraySize;
        };
    };

  public:
    // Type(bool primitive, bool array, std::string name, TypeID typeId) :
    //    Type(primitive, array, name, typeId, -1,0) {}

    Type(bool primitive, bool array, std::string name, TypeID typeId, TypeID baseTypeId, int size) :
        primitive(primitive), array(array), native(baseTypeId == -1), name(name), size(size),
        typeId(typeId), baseTypeId(baseTypeId) {}

    Type(std::string name, TypeID typeId, std::vector<RecordField> *members) :
        name(name), typeId(typeId), members(members) {
        //        std::cout << "Defined record type " << name << std::endl;
        membersMap = new std::map<std::string, RecordField *>();
        auto iterator = members->begin();
        auto endIterator = members->end();
        for (; iterator != endIterator; ++iterator) {
            iterator->parent = this;
            membersMap->insert(std::pair<std::string, RecordField *>(iterator->name, &(*iterator)));
            //            std::cout << "Member " << iterator->name << ":" <<
            //            iterator->type->toString()
            //                      << std::endl;
        }
    }

    Type(std::string name, TypeID typeId, TypeID elementId, Expression *size) :
        array(true), name(name), typeId(typeId), baseTypeId(elementId), arraySize(size) {}

    ~Type() {
        if (this->isRecord()) {
            delete members;
            delete membersMap;
        }
    }

    bool isNative() const { return native; }

    bool isPrimitive() const { return primitive; }

    bool isArray() const { return array; }

    bool isRecord() const { return !primitive && !array; }

    const std::string getName() const { return std::string(name); }

    const int getSize() const { return size; }

    Expression *getArraySize() { return arraySize; }

    const TypeID getType() const { return typeId; }

    const TypeID getBaseType() const { return baseTypeId; }

    TypeID getNativeType() {
        if (nativeTypeID == -2) {
            // to cache value
            nativeTypeID = FindNativeBaseType(getType());
        }
        return nativeTypeID;
    }

    RecordField *getMemberByName(std::string memberName) {
        if (!isRecord()) {
            return nullptr;
        }
        if (membersMap->count(memberName) == 0) {
            return nullptr;
        }
        return membersMap->at(memberName);
    }

    std::vector<RecordField> *getMembers() {
        if (isRecord()) {
            return members;
        } else {
            return nullptr;
        }
    }

    const std::string toString() const {
        return std::string("Type(primitive=") + (primitive ? "true" : "false")
               + ", array=" + (array ? "true" : "false") + ", name=" + name + ", typeId="
               + std::to_string(typeId) + ", baseTypeId=" + std::to_string(baseTypeId) + ")";
    }

  public:
    static Type *ofTypeDeclaration(TypeDeclaration *typeDeclaration) {
        return static_cast<Type *>(typeDeclaration->typePtr);
    }
};

struct Name {
    friend class NameTable;

  protected:
    std::string name;
    Type *type;
    bool parameter{false};
    Function *parameterOwner{nullptr};
    // llvm::Value *llvmValue{nullptr};
    std::map<Scope *, llvm::Value *> llvmValueMap;

  public:
    Name(std::string name, Type *type) : Name(name, type, false, nullptr) {}
    Name(std::string name, Type *type, bool isParameter, Function *owner) :
        name(name), type(type), parameter(isParameter), parameterOwner(owner) {}

    virtual ~Name() = default;
    void setLLVMValue(Scope *referrer, llvm::Value *value) { llvmValueMap[referrer] = value; }
    void resetLLVMValue(Scope *referrer) { llvmValueMap.erase(referrer); }
    bool isParameter() { return parameter; }
    llvm::Value *getLLVMValue(Scope *referrer) { return llvmValueMap[referrer]; }
    Function *getParameterOwner() { return parameterOwner; }
    virtual std::string &getName() { return name; }
    virtual Type *getType() { return type; }
    virtual std::string toString() {
        return "Name(name=\"" + this->name + "\", " + "type=" + this->type->toString() + ")";
    }

  public:
    static Name *ofVariableDeclaration(VariableDeclaration *variableDeclaration) {
        return static_cast<Name *>(variableDeclaration->namePtr);
    }
};

struct FunctionParameter {
    int index;
    Name *name;
    Type *type;
};

struct Function {
    friend class FuncTable;

  protected:
    Scope *innerScope;
    std::string name;
    std::vector<FunctionParameter> parameters;
    TypeID returnType{0};
    Body *functionBody;
    llvm::Function *llvmFunction{nullptr};

  public:
    Function(Scope *visibilityScope, std::string functionName, Body *body) :
        Function(visibilityScope, functionName, body, TypeID(0)) {}

    Function(Scope *visibilityScope, std::string functionName, Body *body, TypeID returnType);

    Function(Scope *visibilityScope, std::string functionName, TypeID returnType);

    virtual ~Function();

    void setLLVMFunction(llvm::Function *function) { llvmFunction = function; }
    void addParameter(FunctionParameter parameter) { parameters.push_back(parameter); }
    void onFunctionDefined();

    Scope *getScope() { return innerScope; }
    TypeID getReturnTypeId() { return returnType; }
    llvm::Function *getLLVMFunction() { return llvmFunction; }
    std::vector<FunctionParameter> &getAllParameters() { return parameters; }
    virtual std::string &getName() { return name; }
    virtual std::string toString();

    static Function *ofRoutineDeclaration(RoutineDeclaration *declaration) {
        return static_cast<Function *>(declaration->functionPtr);
    }
};

class NameTable {
    friend Scope;

  public:
    virtual ~NameTable() = default;

  public:
    Name *defineName(VariableDeclaration *varDeclaration);
    virtual Name *lookupName(const std::string varName, bool *fromThis) = 0;
    virtual Name *lookupName(const std::string varName) {
        bool unused = true;
        return this->lookupName(varName, &unused);
    };

    virtual Name *defineName(Name *src) = 0;
    virtual std::vector<Name *> getAllNames() = 0;

  protected:
    virtual void setOwnerScope(Scope *scope) = 0;
    virtual NameTable *getParentTable() = 0;
    virtual Scope *getOwnerScope() = 0;
};

class TypeTable {
    friend Scope;

  public:
    virtual ~TypeTable() = default;

  public:
    Type *defineType(TypeDeclaration *typeDeclaration);

    Type *defineType(const std::string typeName, TypeDefinition *src);

    virtual Type *lookupType(const std::string typeName) = 0;

    virtual Type *lookupType(const TypeID typeId) = 0;
    virtual Type *defineType(Type *src) = 0;
    virtual std::vector<Type *> getAllTypes() = 0;

  protected:
    virtual void setOwnerScope(Scope *scope) = 0;
    virtual TypeTable *getParentTable() = 0;
};

class FuncTable {
    friend Scope;

  public:
    virtual ~FuncTable() = default;

  public:
    Function *defineFunc(RoutineDeclaration *funcDeclaration);
    virtual Function *lookupFunc(const std::string funcName) = 0;
    virtual Function *defineFunc(Function *src) = 0;
    virtual std::vector<Function *> getAllFuncs() = 0;

  protected:
    virtual void setOwnerScope(Scope *scope) = 0;
};

class Scope {
    Scope *parentScope;
    // scope owns next three tables and their deallocations
    NameTable *nameTable;
    TypeTable *typeTable;
    FuncTable *funcTable;
    bool isGlobalScope;
    std::string scopeName;
    std::vector<Scope *> children;
    std::vector<Name *> namesFallthroughList;

  public:
    Scope(Scope *parentScope, NameTable *nameTable, TypeTable *typeTable) :
        Scope(parentScope, nameTable, typeTable, "<gen>") {}

    Scope(Scope *parentScope, NameTable *nameTable, TypeTable *typeTable, std::string name);

    ~Scope();

    std::string getScopeName() { return scopeName; }
    bool isGlobal() { return isGlobalScope; }
    Scope *parent() { return parentScope; }
    NameTable *names() { return nameTable; }
    TypeTable *types() { return typeTable; }
    FuncTable *funcs() { return funcTable; }

    void insertNameFallthrough(Name *name) { namesFallthroughList.push_back(name); }

    std::vector<Name *> &getAllFallthroughNames() { return namesFallthroughList; }

  private:
    void onChildScopeAttached(Scope *child);

  public:
    static Scope *ofBody(Body *body) { return static_cast<Scope *>(body->scopePtr); }
    static Scope *createInnerScope(Scope *parent, std::string name);
};

/**
 * Initializes global scope, loads predefined types and functions
 * @return 0 if succeeded, error code otherwise
 */
int InitSemanticContext();

/**
 * Destroys global scope, type tables and functions
 * @return 0 if cleanup succeeded
 */
int DestroySemanticContext();

/**
 * @return Scope for global types and variables
 */
Scope *GetGlobalScope();

/**
 * @return Scope according to the current semantic analysis state
 */
Scope *GetCurrentScope();

/**
 * Enters new scope
 * @param scope should have current scope as one of ancestors
 * @return true if succeeded
 */
bool EnterScope(Scope *scope);

/**
 * Exits current scope
 * @return true if succeeded
 */
bool ExitCurrentScope();

void SetCallStatementStatus(bool isSingleCall);

/**
 * @return Function table for global functions
 */
FuncTable *GetFunctionTable();

void AddUserType(Type *type);

/**
 * @return List of special function for current module
 */
std::vector<Type *> &GetUserTypesList();

void ReportSemanticWarning(std::string message);
/**
 * Prints out message about semantic problem and exits from the compiler with code 1
 * @param message
 */
void ReportSemanticError(std::string message);

void RunSemanticPass(Program *program);

/**
 * @param expressionNode root expression node to begin type tracing
 * @return resulting type id, or -1 if trace failed
 */
TypeID RunTypeTrace(Expression *expressionNode);

TypeID InsertTypeCastExpression(Expression **refFromParent, Expression *child, TypeID targetType,
                                bool isExplicit = false);

TypeID InsertTypeCastExpression(Expression **refFromParent, Expression *child, TypeID sourceType,
                                TypeID targetType, bool isExplicit = false);

TypeID ApplyTypeWidening(BinaryExpression *binaryExpression);

TypeID ApplyTypeWidening(UnaryExpression *unaryExpression);

TypeID IncrementAndGetTypeID();

TypeID FindNativeBaseType(TypeID typeId);

} // namespace Semantics

#endif // CC_PROJECT_SEMANTICS_H
