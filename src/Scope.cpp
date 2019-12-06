/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include <algorithm>
#include <map>
#include "Config.h"
#include "Semantics.h"
#include "Utils.h"

using namespace Semantics;

FuncTable *globalFunctionTable;
NameTable *globalNameTable;
TypeTable *globalTypeTable;

Scope *globalScope;
Scope *currentScope;

class ScopeNameTable : public NameTable {
    Scope *owner;

    std::map<std::string, Name *> nameMap;

    ~ScopeNameTable() {
        for (auto pair : nameMap) {
            delete pair.second;
        }
    };

    Name *lookupName(const std::string varName, bool *fromThis) override {
        if (nameMap.count(varName) != 0) {
            *fromThis = true;
            return nameMap[varName];
        } else {
            *fromThis = false;
            auto parentTable = getParentTable();
            if (parentTable == nullptr) {
                return nullptr;
            } else {
                return parentTable->lookupName(varName);
            }
        }
    }

    Name *defineName(Name *name) override {
        nameMap[name->getName()] = name;
        if (IsComponentTraced(COMPONENT_SCOPE)) {
            GetTraceStream(COMPONENT_SCOPE) << "(" << this->owner->getScopeName() + ") "
                                            << "defined name: " << name->toString() << std::endl;
        }
        return name;
    }

    NameTable *getParentTable() override {
        if (this->owner->isGlobal()) {
            return nullptr;
        } else {
            return this->owner->parent()->names();
        }
    }

    Scope *getOwnerScope() override {
        if (this->owner->isGlobal()) {
            return nullptr;
        } else {
            return this->owner;
        }
    }

    void setOwnerScope(Semantics::Scope *scope) override { this->owner = scope; }

    std::vector<Name *> getAllNames() override {
        std::vector<Name *> nameList;
        for (auto it = nameMap.begin(); it != nameMap.end(); ++it) {
            nameList.push_back(it->second);
        }
        return nameList;
    }
};

class ScopeTypeTable : public TypeTable {
    Scope *owner;
    std::map<std::string, Type *> typeNameMap;
    std::map<TypeID, Type *> typeIDMap;

    ~ScopeTypeTable() {
        for (auto pair : typeIDMap) {
            delete pair.second;
        }
    }

    Type *lookupType(const std::string typeName) override {
        if (typeNameMap.count(typeName) != 0) {
            return typeNameMap[typeName];
        } else {
            auto parentTable = getParentTable();
            if (parentTable == nullptr) {
                return nullptr;
            } else {
                return parentTable->lookupType(typeName);
            }
        }
    }

    Type *lookupType(const TypeID typeID) override {
        if (typeIDMap.count(typeID) != 0) {
            return typeIDMap[typeID];
        } else {
            auto parentTable = getParentTable();
            if (parentTable == nullptr) {
                return nullptr;
            } else {
                return parentTable->lookupType(typeID);
            }
        }
    }

    Type *defineType(Type *type) override {
        typeNameMap[type->getName()] = type;
        typeIDMap[type->getType()] = type;
        if (IsComponentTraced(COMPONENT_SCOPE)) {
            GetTraceStream(COMPONENT_SCOPE) << "(" << this->owner->getScopeName() << ") "
                                            << "defined type: " << type->toString() << std::endl;
        }
        return type;
    }

    TypeTable *getParentTable() override {
        if (this->owner->isGlobal()) {
            return nullptr;
        } else {
            return this->owner->parent()->types();
        }
    }

    void setOwnerScope(Semantics::Scope *scope) override { this->owner = scope; }

    std::vector<Type *> getAllTypes() override {
        std::vector<Type *> typeList;
        for (auto it = typeIDMap.begin(); it != typeIDMap.end(); ++it) {
            typeList.push_back(it->second);
        }
        return typeList;
    }
};

class GlobalFunctionTable : public FuncTable {
    std::map<std::string, Function *> funcMap;

    ~GlobalFunctionTable() {
        for (auto pair : funcMap) {
            delete pair.second;
        }
    }

    Function *lookupFunc(const std::string funcName) override {
        if (funcMap.count(funcName) == 0) {
            return nullptr;
        }

        return funcMap[funcName];
    }

    Function *defineFunc(Function *func) override {
        funcMap[func->getName()] = func;
        func->onFunctionDefined();
        return func;
    }

    void setOwnerScope(Semantics::Scope *scope) override {}

    std::vector<Function *> getAllFuncs() override {
        std::vector<Function *> funcList;
        for (auto it = funcMap.begin(); it != funcMap.end(); ++it) {
            funcList.push_back(it->second);
        }
        return funcList;
    }
};

class FunctionTableWrap : public FuncTable {
    Scope *owner{};
    FuncTable *wrapTable;

  public:
    FunctionTableWrap(FuncTable *wrap) : wrapTable(wrap) {}

    virtual ~FunctionTableWrap() = default;

    Function *lookupFunc(const std::string funcName) override {
        return wrapTable->lookupFunc(funcName);
    }

    Function *defineFunc(Function *func) override {
        TypeTable *typeTable = owner->types();
        if (typeTable->lookupType(func->getName())) {
            // TODO: function name is used for type name, error
            return nullptr;
        }
        NameTable *nameTable = owner->names();
        if (nameTable->lookupName(func->getName())) {
            // TODO: function name is used for variable name, error
            return nullptr;
        }
        return wrapTable->defineFunc(func);
    }

    void setOwnerScope(Semantics::Scope *scope) override { owner = scope; }

    std::vector<Function *> getAllFuncs() override { return wrapTable->getAllFuncs(); }
};

Function::Function(Scope *visibilityScope, std::string functionName, Body *body,
                   TypeID returnType) :
    name(functionName),
    returnType(returnType), functionBody(body) {
    auto typeTable = new ScopeTypeTable();
    auto nameTable = new ScopeNameTable();
    innerScope = new Scope(visibilityScope, nameTable, typeTable, "func$" + functionName);
}

Function::Function(Scope *visibilityScope, std::string functionName, TypeID returnType) :
    name(functionName), returnType(returnType), functionBody(nullptr) {
    // this constructor is used for intrinsic functions, no need in inner scope
    innerScope = nullptr;
}

Function::~Function() {
    // scope will be removed by scope tree traverse
    // delete innerScope;
}

void Function::onFunctionDefined() {}

std::string Function::toString() {
    Type *retType;
    if (getScope()) {
        retType = getScope()->parent()->types()->lookupType(returnType);
    } else {
        retType = GetGlobalScope()->types()->lookupType(returnType);
    }
    auto strBeg = "Function(name=\"" + this->name + "\", parameters=(";
    auto strParams = std::string();
    for (auto parameter : this->parameters) {
        if (parameter.name) {
            strParams
                += std::string(parameter.name->getName()) + ":" + parameter.type->toString() + ",";
        } else {
            strParams += parameter.type->toString() + ",";
        }
    }
    auto strEnd = "), returnType=" + retType->toString() + ")";
    return strBeg + strParams + strEnd;
}

Name *NameTable::defineName(VariableDeclaration *varDeclaration) {
    std::string varName(varDeclaration->varName);
    Type *type;
    if (varDeclaration->type != nullptr) {
        auto typeId = varDeclaration->type->getTypeId();
        auto typeTable = this->getOwnerScope()->types();
        type = typeTable->lookupType(typeId);
        if (type == nullptr) {
            ReportSemanticError(std::string("Unknown type used in variable declaration"));
        }
    } else {
        if (varDeclaration->initialValue == nullptr) {
            ReportSemanticError(std::string("Unable to deduce type of variable ")
                                + varDeclaration->varName + ": no initial value");
        }
        auto typeId = RunTypeTrace(varDeclaration->initialValue);
        type = this->getOwnerScope()->types()->lookupType(typeId);
    }
    if (type == nullptr) {
        // no type - no variables
        return nullptr;
    }
    if (varDeclaration->initialValue == nullptr) {
        auto nativeInitialTypeId = type->getNativeType();
        switch (nativeInitialTypeId) {
            case TYPE_ID_INT: {
                varDeclaration->initialValue = new ConstPrimary(0);
                // varDeclaration->initialValue->expressionType = TYPE_ID_INTEGER;
                break;
            }
            case TYPE_ID_BOOL: {
                varDeclaration->initialValue = new ConstPrimary(false);
                // varDeclaration->initialValue->expressionType = TYPE_ID_BOOLEAN;
                break;
            }
            case TYPE_ID_FLOAT: {
                varDeclaration->initialValue = new ConstPrimary(0.0F);
                // varDeclaration->initialValue->expressionType = TYPE_ID_REAL;
                break;
            }
        }
    } else {
    }
    Name *name = new Name(varName, type);
    varDeclaration->namePtr = name;
    return this->defineName(name);
}

Type *TypeTable::defineType(TypeDeclaration *typeDeclaration) {
    auto type = this->defineType(typeDeclaration->typeName, typeDeclaration->typeDefinition);
    typeDeclaration->typePtr = type;
    return type;
}

Type *TypeTable::defineType(const std::string typeName, TypeDefinition *src) {
    // getTypeId will initialize lazy loading of type by calling current type table methods
    /*auto typeId = src->getTypeId();
    if (typeId < 0) {
        return nullptr;
    }*/
    TypeID baseTypeId = -1;
    if (src->isNamed) {
        auto namedSrc = static_cast<NamedTypeDefinition *>(src);
        auto nameType = this->lookupType(namedSrc->name);
        if (nameType == nullptr) {
            ReportSemanticError(std::string("unable to define type ") + typeName
                                + ": no such base type: " + namedSrc->name);
            return nullptr;
        }
        baseTypeId = nameType->getType();
        auto newType = new Type(true, false, typeName, IncrementAndGetTypeID(), baseTypeId,
                                nameType->getSize());
        auto definedType = this->defineType(newType);
        ASSERT_NOT_NULL(definedType)
        src->typeId = definedType->getType();
        return definedType;
    }
    if (src->isArray) {
        auto arraySrc = static_cast<ArrayTypeDefinition *>(src);
        auto elementSrc = arraySrc->arrayElementType;
        Type *elementType = nullptr;
        if (elementSrc->isNamed) {
            auto fieldTypeName = static_cast<NamedTypeDefinition *>(elementSrc)->name;
            elementType = lookupType(fieldTypeName);
        } else {
            std::string namePrefix = GetCurrentScope() != GetGlobalScope()
                                         ? GetCurrentScope()->getScopeName() + "$"
                                         : "";
            auto tn = namePrefix + typeName + "$ElemT"
                      + std::to_string(GetAndIncrementUniqueCounter(TYPE_COUNT));
            elementType = defineType(tn, elementSrc);
        }
        auto indexType = RunTypeTrace(arraySrc->sizeExpression);
        if (indexType == -1) {
            Semantics::ReportSemanticError("unable to trace type for array size");
            return nullptr;
        }
        if (indexType != TYPE_ID_INT) {
            Semantics::ReportSemanticWarning("array index should be of type integer");
            InsertTypeCastExpression(&arraySrc->sizeExpression, arraySrc->sizeExpression,
                                     TYPE_ID_INT, false);
        }
        auto newType = new Type(typeName, IncrementAndGetTypeID(), elementType->getType(),
                                arraySrc->sizeExpression);
        auto definedType = this->defineType(newType);
        ASSERT_NOT_NULL(definedType)
        src->typeId = definedType->getType();
        AddUserType(definedType);
        return definedType;
    } else {
        auto recordSrc = static_cast<RecordTypeDefinition *>(src);
        auto fieldList = recordSrc->variableDeclarationList->variableDeclarationList;
        auto recordFieldVector = new std::vector<RecordField>();
        auto index = 0;
        for (auto field : fieldList) {
            Type *fieldType = nullptr;
            auto fieldTypeDefinition = field->type;
            std::string namePrefix = GetCurrentScope() != GetGlobalScope()
                                         ? GetCurrentScope()->getScopeName() + "$"
                                         : "";
            if (fieldTypeDefinition->isNamed) {
                auto fieldTypeName = static_cast<NamedTypeDefinition *>(fieldTypeDefinition)->name;
                fieldType = lookupType(fieldTypeName);
            } else {
                auto tn = namePrefix + typeName + "$T"
                          + std::to_string(GetAndIncrementUniqueCounter(TYPE_COUNT));
                fieldType = defineType(tn, fieldTypeDefinition);
            }
            ASSERT_NOT_NULL(fieldType)
            recordFieldVector->push_back(RecordField{
                .type = fieldType,
                .index = index++,
                .name = field->varName,
            });
        }
        auto newType = new Type(typeName, IncrementAndGetTypeID(), recordFieldVector);
        auto definedType = this->defineType(newType);
        src->typeId = definedType->getType();
        AddUserType(definedType);
        return definedType;
    }
}

Function *FuncTable::defineFunc(RoutineDeclaration *routineDeclaration) {
    std::string name(routineDeclaration->routineName);
    TypeID returnValueTypeId = 0;
    if (routineDeclaration->returnType != nullptr) {
        returnValueTypeId = routineDeclaration->returnType->getTypeId();
    } else {
        returnValueTypeId = TYPE_ID_VOID;
    }
    auto *func = new Function(GetCurrentScope(), name, routineDeclaration->body, returnValueTypeId);
    int index = 0;
    for (auto paramNode : routineDeclaration->parameters->parameterList) {
        auto paramType = GetCurrentScope()->types()->lookupType(paramNode->typeName);
        if (!paramType) {
            Semantics::ReportSemanticError(std::string("Unknown parameter type name ")
                                           + paramNode->typeName + " for function " + name);
            return nullptr;
        }
        auto paramName = new Name(paramNode->identifier, paramType, true, func);
        func->getScope()->names()->defineName(paramName);
        func->addParameter(
            FunctionParameter{.index = index++, .name = paramName, .type = paramType});
    }
    routineDeclaration->functionPtr = func;
    routineDeclaration->body->scopePtr = func->getScope();
    return this->defineFunc(func);
}

int scopeNameCounter = 0;

Scope::Scope(Scope *parentScope, NameTable *nameTable, TypeTable *typeTable, std::string name) :
    parentScope(parentScope), nameTable(nameTable), typeTable(typeTable),
    isGlobalScope(parentScope == nullptr), scopeName(name) {
    this->funcTable = new FunctionTableWrap(globalFunctionTable);
    this->funcTable->setOwnerScope(this);
    if (this->typeTable) {
        this->typeTable->setOwnerScope(this);
    }
    if (this->nameTable) {
        this->nameTable->setOwnerScope(this);
    }
    if (this->scopeName == "<gen>") {
        this->scopeName = (parentScope ? parentScope->scopeName : "scope_")
                          + std::to_string(scopeNameCounter++);
    }
    if (this->parentScope != nullptr) {
        this->parentScope->onChildScopeAttached(this);
    }
}

Scope::~Scope() {
    for (auto child : children) {
        delete child;
    }
    delete funcTable;
    delete nameTable;
    delete typeTable;
}

void Scope::onChildScopeAttached(Scope *child) {
    children.push_back(child);
}

Scope *Scope::createInnerScope(Scope *parent, std::string name) {
    auto typeTable = new ScopeTypeTable();
    auto nameTable = new ScopeNameTable();
    auto innerScope = new Scope(parent, nameTable, typeTable, parent->getScopeName() + "_" + name);
    return innerScope;
}

std::vector<Type *> userTypes;

void Semantics::AddUserType(Semantics::Type *type) {
    userTypes.push_back(type);
}

std::vector<Type *> &Semantics::GetUserTypesList() {
    return userTypes;
}

bool Semantics::EnterScope(Scope *scope) {
    // TODO: add scope ancestors check
    currentScope = scope;
    if (IsComponentTraced(COMPONENT_SEMANTICS)) {
        GetTraceStream(COMPONENT_SEMANTICS)
            << ">>> entering scope " << scope->getScopeName() << std::endl;
    }
    return true;
}

bool Semantics::ExitCurrentScope() {
    if (currentScope == globalScope) {
        return false;
    }
    if (IsComponentTraced(COMPONENT_SEMANTICS)) {
        GetTraceStream(COMPONENT_SEMANTICS)
            << "<<< exiting scope " << currentScope->getScopeName() << std::endl;
    }
    currentScope = currentScope->parent();
    return false;
}

Scope *Semantics::GetCurrentScope() {
    return currentScope;
}

Scope *Semantics::GetGlobalScope() {
    return globalScope;
}

void InitBuiltinTypes() {
    TypeTable *typeTable = globalScope->types();
    typeTable->defineType(new Type(true, false, "void", TYPE_ID_VOID, -1, 0));
    typeTable->defineType(new Type(true, false, "bool", TYPE_ID_BOOL, -1, 1));
    typeTable->defineType(new Type(true, false, "int", TYPE_ID_INT, -1, 4));
    typeTable->defineType(new Type(true, false, "float", TYPE_ID_FLOAT, -1, 4));
}

void InitBuiltinFunctions() {
    auto typeTable = globalScope->types();
    auto funcTable = globalScope->funcs();
    auto intType = typeTable->lookupType(TYPE_ID_INT);
    char *printIntName = nullptr;
    STRDUP("printInt", &printIntName)
    auto printIntFunc = new Function(globalScope, printIntName, TYPE_ID_VOID);
    printIntFunc->addParameter(FunctionParameter{.index = 0, .name = nullptr, .type = intType});
    funcTable->defineFunc(printIntFunc);
}

int Semantics::InitSemanticContext() {
    globalFunctionTable = new GlobalFunctionTable();
    auto moduleTypeTable = new ScopeTypeTable();
    auto moduleNameTable = new ScopeNameTable();
    globalScope = new Scope(nullptr, moduleNameTable, moduleTypeTable, "global");
    InitBuiltinTypes();
    InitBuiltinFunctions();
    currentScope = globalScope;
    return 0;
}

int Semantics::DestroySemanticContext() {
    delete globalScope;
    delete globalFunctionTable;
    delete GetProgramTree();
    userTypes.clear();
    SetProgramTree(nullptr);
    return 0;
}

void Semantics::ReportSemanticWarning(std::string message) {
    std::cerr << "[SEMANTICS] WARN  : " << message << std::endl;
}

void Semantics::ReportSemanticError(std::string message) {
    std::cerr << "[SEMANTICS] ERROR : " << message << std::endl;
}
