/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#ifndef CC_PROJECT_TREE_H
#define CC_PROJECT_TREE_H

#include <list>
#include <string>

typedef long long TypeID;

enum AstNodeType {
    PROGRAM,
    DECLARATION,
    DEFINITION,
    STATEMENT,
    EXPRESSION,
    LIST,
};

enum DeclarationType {
    VARIABLE_DECLARATION = 1,
    TYPE_DECLARATION = 2,
    PARAMETER_DECLARATION = 3,
    ROUTINE_DECLARATION = 4,
};

enum StatementType {
    EXPRESSION_STATEMENT,
    RETURN_STATEMENT,
    // ASSIGNMENT_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    // ROUTINE_CALL_STATEMENT
};

struct Declaration;
struct Statement;
struct Expression;
struct Primary;

struct AstNode {
    int type : 7;
    bool isSemanticsValid : 1;
    int line;
    int column;

    explicit AstNode(int type) : type(type), isSemanticsValid(true), line(0), column(0) {}
    virtual ~AstNode() = default;

    Statement *toStatement();
    Declaration *toDeclaration();
};

struct Program;
struct Declaration;
struct TypeDeclaration;
struct VariableDeclaration;
struct RoutineDeclaration;
struct TypeDefinition;
struct Body;
struct TypeDefinition;
struct ModifiablePrimary;

void SetProgramTree(Program *program);

Program *GetProgramTree();

struct Program : AstNode {
    std::list<Declaration *> declarations;

    Program() : AstNode(PROGRAM) { SetProgramTree(this); }

    virtual ~Program();

    Program *addSimpleDeclaration(Declaration *declaration) {
        declarations.push_back(declaration);
        return this;
    }

    Program *addRoutineDeclaration(Declaration *declaration) {
        declarations.push_back(declaration);
        return this;
    }
};

struct Declaration : AstNode {
    int declarationType = -1;

    explicit Declaration(int declarationType) :
        AstNode(DECLARATION), declarationType(declarationType) {}

    virtual ~Declaration();

    VariableDeclaration *toVariableDeclaration();

    TypeDeclaration *toTypeDeclaration();

    RoutineDeclaration *toRoutineDeclaration();
};

struct VariableDeclaration : Declaration {
    char *varName;
    TypeDefinition *type;
    Expression *initialValue;
    // pointer to Semantics::Name
    void *namePtr{nullptr};

    VariableDeclaration(char *varName, TypeDefinition *type, Expression *initialValue) :
        Declaration(VARIABLE_DECLARATION), varName(varName), type(type),
        initialValue(initialValue) {}

    virtual ~VariableDeclaration();
};

struct VariableDeclarationList : AstNode {
    std::list<VariableDeclaration *> variableDeclarationList;

    explicit VariableDeclarationList(VariableDeclaration *variableDeclaration) : AstNode(LIST) {
        variableDeclarationList.push_back(variableDeclaration);
    }

    VariableDeclarationList *addVariableDeclaration(VariableDeclaration *variableDeclaration) {
        variableDeclarationList.push_back(variableDeclaration);
        return this;
    }

    virtual ~VariableDeclarationList();
};

struct ParameterDeclaration : Declaration {
    char *identifier;
    char *typeName;

    ParameterDeclaration(char *identifier, char *typeName) :
        Declaration(PARAMETER_DECLARATION), identifier(identifier), typeName(typeName) {}

    virtual ~ParameterDeclaration();
};

struct ParameterList : AstNode {
    ParameterList() : AstNode(LIST) {}

    std::list<ParameterDeclaration *> parameterList;

    explicit ParameterList(ParameterDeclaration *parameterDeclaration) : AstNode(LIST) {
        parameterList.push_back(parameterDeclaration);
    }

    ParameterList *addParameter(ParameterDeclaration *parameterDeclaration) {
        parameterList.push_back(parameterDeclaration);
        return this;
    }

    virtual ~ParameterList();
};

struct RoutineDeclaration : Declaration {
    char *routineName;
    ParameterList *parameters;
    TypeDefinition *returnType;
    Body *body;
    // pointer to Semantics::Function
    void *functionPtr{nullptr};

    RoutineDeclaration(char *routineName, ParameterList *parameters, TypeDefinition *returnType,
                       Body *body) :
        Declaration(ROUTINE_DECLARATION),
        routineName(routineName), parameters(parameters), returnType(returnType), body(body) {}

    virtual ~RoutineDeclaration();
};

struct TypeDeclaration : Declaration {
    char *typeName;
    TypeDefinition *typeDefinition;
    // pointer to Semantics::Type
    void *typePtr{nullptr};

    TypeDeclaration(char *typeName, TypeDefinition *typeDefinition) :
        Declaration(TYPE_DECLARATION), typeName(typeName), typeDefinition(typeDefinition) {}

    virtual ~TypeDeclaration();
};

struct TypeDefinition : AstNode {
    bool isReady;
    bool isNamed;
    bool isArray;
    TypeID typeId;

    TypeDefinition(bool isNamed, bool isArray = false) :
        AstNode(DEFINITION), isReady(false), isNamed(isNamed), isArray(isArray), typeId(-1) {}

    TypeID getTypeId() {
        if (!isReady) {
            typeId = obtainTypeId();
            isReady = true;
        }
        return typeId;
    }

    virtual ~TypeDefinition();

  protected:
    virtual TypeID obtainTypeId() = 0;
};

struct NamedTypeDefinition : TypeDefinition {
    // stores value of parent type name
    // type <new_type_name> is **name**
    char *name;

    explicit NamedTypeDefinition(int yaccKeyword);

    explicit NamedTypeDefinition(char *typeName) : TypeDefinition(true) { this->name = typeName; }

    TypeID obtainTypeId() override;

    virtual ~NamedTypeDefinition();
};

struct RecordTypeDefinition : TypeDefinition {
    VariableDeclarationList *variableDeclarationList;

    explicit RecordTypeDefinition(VariableDeclarationList *variableDeclarationList) :
        TypeDefinition(false, false), variableDeclarationList(variableDeclarationList) {}

    TypeID obtainTypeId() override;

    virtual ~RecordTypeDefinition();
};

struct ArrayTypeDefinition : TypeDefinition {
    Expression *sizeExpression;
    TypeDefinition *arrayElementType;

    explicit ArrayTypeDefinition(Expression *constSizeExpression, TypeDefinition *type) :
        TypeDefinition(false, true), sizeExpression(constSizeExpression), arrayElementType(type) {}

    TypeID obtainTypeId() override;

    virtual ~ArrayTypeDefinition();
};

struct Statement : AstNode {
    StatementType statementType;

    explicit Statement(StatementType statementType) :
        AstNode(STATEMENT), statementType(statementType) {}

    virtual ~Statement();
};

struct ExpressionStatement : Statement {
    Expression *expression;
    ExpressionStatement(Expression *expression) :
        Statement(EXPRESSION_STATEMENT), expression(expression) {}

    virtual ~ExpressionStatement();
};

struct ReturnStatement : Statement {
    Expression *expressionToReturn;
    ReturnStatement(Expression *expression) :
        Statement(RETURN_STATEMENT), expressionToReturn(expression) {
        traceExpressionTypes();
    }

    void traceExpressionTypes();

    virtual ~ReturnStatement();
};

struct ArgumentList : AstNode {
    ArgumentList() : AstNode(LIST) {}

    std::list<Expression *> expressionList;

    explicit ArgumentList(Expression *expression) : AstNode(LIST) {
        expressionList.push_back(expression);
    }

    ArgumentList *addArgument(Expression *expression) {
        expressionList.push_back(expression);
        return this;
    }

    virtual ~ArgumentList();
};

struct Body : AstNode {
    Body() : AstNode(STATEMENT) {}

    // pointer to Semantics::Scope
    void *scopePtr{nullptr};
    std::list<AstNode *> nodeList;

    void onTypeDeclared(TypeDeclaration *declaration);

    void onVariableDeclared(VariableDeclaration *declaration);

    void onStatementAdded(Statement *statement);

    Body *addNode(AstNode *node) {
        nodeList.push_back(node);
        if (node->type == AstNodeType::DECLARATION) {
            auto declaration = static_cast<Declaration *>(node);
            if (declaration->declarationType == DeclarationType::TYPE_DECLARATION) {
                onTypeDeclared(static_cast<TypeDeclaration *>(node));
            } else {
                onVariableDeclared(static_cast<VariableDeclaration *>(node));
            }
        } else {
            onStatementAdded(static_cast<Statement *>(node));
        }
        return this;
    }

    virtual ~Body();
};

struct WhileLoopStatement : Statement {
    Expression *expression;
    Body *body;

    WhileLoopStatement(Expression *expression, Body *body) :
        Statement(WHILE_STATEMENT), expression(expression), body(body) {
        traceExpressionTypes();
    }

    void traceExpressionTypes();

    virtual ~WhileLoopStatement();
};

struct Range : AstNode {
    Expression *lo;
    Expression *hi;

    Range(Expression *lo, Expression *hi) : AstNode(LIST), lo(lo), hi(hi) {}

    virtual ~Range();
};

struct ForLoopStatement : Statement {
    char *identifier;
    Range *range;
    bool reverse;
    Body *body;

    ForLoopStatement(char *identifier, Range *range, bool reverse, Body *body) :
        Statement(FOR_STATEMENT), identifier(identifier), range(range), reverse(reverse),
        body(body) {
        traceExpressionTypes();
    }

    void traceExpressionTypes();

    virtual ~ForLoopStatement();
};

struct IfStatement : Statement {
    Expression *expression;
    Body *trueBody;
    Body *falseBody;

    IfStatement(Expression *expression, Body *trueBody, Body *falseBody) :
        Statement(IF_STATEMENT), expression(expression), trueBody(trueBody), falseBody(falseBody) {
        traceExpressionTypes();
    }

    void traceExpressionTypes();

    virtual ~IfStatement();
};

struct Expression : AstNode {
    // Unary: false, false
    // Binary: false, true
    // Primary: true, false
    bool isPrimary;
    bool isBinary;
    TypeID expressionType{-1};
    // pointer to llvm::Value
    // used while code generation
    void *llvmValuePtr;

    explicit Expression(bool isPrimary, bool isBinary) :
        AstNode(EXPRESSION), isPrimary(isPrimary), isBinary(isBinary) {}

    virtual ~Expression();
};

// unary or synthetic cast operation
struct UnaryExpression : Expression {
    int oper;
    int nativeOper{-1};
    Expression *operand;

    UnaryExpression(int oper, Expression *operand) :
        Expression(false, false), oper(oper), operand(operand) {}

    virtual ~UnaryExpression();
};

struct BinaryExpression : Expression {
    int oper;
    int nativeOper{-1};
    Expression *operand1;
    Expression *operand2;

    BinaryExpression(int oper, Expression *operand1, Expression *operand2) :
        Expression(false, true), oper(oper), operand1(operand1), operand2(operand2) {}

    virtual ~BinaryExpression();
};

struct Primary : Expression {
    // whether primary value is known in compile-time
    bool isConst;
    // whether primary value can be written or read-only
    bool isCall;

    explicit Primary(bool isConst, bool isCall) :
        Expression(true, false), isConst(isConst), isCall(isCall) {}

    virtual ~Primary();
};

enum ConstPrimaryType {
    INTEGER_PRIMARY_TYPE,
    REAL_PRIMARY_TYPE,
    BOOLEAN_PRIMARY_TYPE,
};

struct ConstPrimary : Primary {
    ConstPrimaryType type{};
    union {
        int intValue;
        float realValue;
        bool boolValue;
    };

    explicit ConstPrimary(int intValue) :
        Primary(true, false), type(INTEGER_PRIMARY_TYPE), intValue(intValue) {}

    explicit ConstPrimary(bool boolValue) :
        Primary(true, false), type(BOOLEAN_PRIMARY_TYPE), boolValue(boolValue) {}

    explicit ConstPrimary(float realValue) :
        Primary(true, false), type(REAL_PRIMARY_TYPE), realValue(realValue) {}

    virtual ~ConstPrimary();
};

struct ModifiablePrimary : Primary {
    char *identifier{};
    bool isAccessMember;
    ModifiablePrimary *member = nullptr;
    Expression *index = nullptr;

    explicit ModifiablePrimary(char *identifier) :
        Primary(false, false), identifier(identifier), isAccessMember(false) {}

    ModifiablePrimary(char *identifier, ModifiablePrimary *member) :
        Primary(false, false), identifier(identifier), isAccessMember(true), member(member) {}

    ModifiablePrimary(char *identifier, Expression *index) :
        Primary(false, false), identifier(identifier), isAccessMember(false), index(index) {}

    ModifiablePrimary(char *identifier, Expression *index, ModifiablePrimary *member) :
        Primary(false, false), identifier(identifier), isAccessMember(true), member(member),
        index(index) {}

    std::string getHierarchicalName() {
        if (isAccessMember && member != nullptr) {
            return std::string(identifier) + "." + member->getHierarchicalName();
        } else {
            return std::string(identifier);
        }
    }

    virtual ~ModifiablePrimary();
};

struct CallablePrimary : Primary {
    ModifiablePrimary *modifiablePrimary;
    char *identifier;
    ArgumentList *argumentList;

    CallablePrimary(ModifiablePrimary *identifier, ArgumentList *argumentList) :
        Primary(false, true), modifiablePrimary(identifier), identifier(identifier->identifier),
        argumentList(argumentList) {}

    virtual ~CallablePrimary();
};

#endif // CC_PROJECT_TREE_H
