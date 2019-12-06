// I Lang YACC grammar
// Written by:
//   Ilya Potemin <i.potemin@innopolis.ru>
//   Kamilla Kryachkina <k.kryachkina@innopolis.ru>

%{
#include "Tree.h"
#include "Lexer.h"
%}

%union{
	char *stringValue;
	int intValue;
	float realValue;
	bool booleanValue;

	AstNode *astNode;
	Program *program;
	Declaration *declaration;
	VariableDeclaration *variableDeclaration;
	VariableDeclarationList *variableDeclarationList;
	ParameterDeclaration *parameterDeclaration;
	ParameterList *parameterList;
	TypeDefinition *typeDefinition;
	Statement *statement;
	ArgumentList *argumentList;
	Body *body;
	Range *range;
	Expression *expression;
	Primary *primary;
	ModifiablePrimary *modifiablePrimary;
	CallablePrimary *callablePrimary;
}

%start Program

%token VAR IS TYPE IDENTIFIER
%token INTEGER REAL BOOLEAN
%token RECORD END ARRAY NEW_LINE
%token WHILE LOOP FOR IN REVERSE IF THEN ELSE ROUTINE RETURN
%token COLON OPEN_PAREN CLOSE_PAREN OPEN_BRACKET CLOSE_BRACKET DOT ELLIPSIS COMMA
%token OP_ASSIGNMENT OP_AND OP_OR OP_XOR
%token OP_EQUAL OP_NOT_EQUAL OP_LESS OP_GREATER OP_LESS_OR_EQUAL OP_GREATER_OR_EQUAL
%token OP_MULTIPLY OP_DIVIDE OP_MODULE OP_PLUS OP_MINUS
%token INTEGER_LITERAL REAL_LITERAL BOOLEAN_LITERAL

%type <stringValue> IDENTIFIER
%type <intValue> INTEGER_LITERAL
%type <realValue> REAL_LITERAL
%type <booleanValue> BOOLEAN_LITERAL
//%type <astNode> SimpleDeclaration TypeDeclaration RoutineDeclaration
%type <typeDefinition> PrimitiveType UserType RecordType ArrayType
%type <astNode> Statement //Assignment RoutineCall
%type <statement> ExpressionStatement WhileLoop ForLoop IfStatement ReturnStatement
%type <parameterDeclaration> ParameterDeclaration
%type <range> Range
%type <expression> Expression Relation Simple Factor Summand
%type <program> Program
%type <typeDefinition> Type
%type <declaration> SimpleDeclaration RoutineDeclaration TypeDeclaration
%type <variableDeclarationList> VariableDeclarationList
%type <variableDeclaration> VariableDeclaration
%type <body> Body
%type <primary> Primary
%type <modifiablePrimary> ModifiablePrimary
%type <callablePrimary> CallablePrimary
%type <parameterList> NonEmptyParameters Parameters
%type <argumentList> NonEmptyArgumentList ArgumentList
%%

NewLine 
	: NEW_LINE 


Program 
	:
	{ $$ = new Program();			}
	| Program NewLine	
	{ $$ = $1;				}
	| Program SimpleDeclaration
	{ $$ = $1->addSimpleDeclaration($2);	}
	| Program RoutineDeclaration
	{ $$ = $1->addRoutineDeclaration($2);	}

SimpleDeclaration
	: VariableDeclaration
	{ $$ = $1;				}
	| TypeDeclaration NewLine
	{ $$ = $1;				}

VariableDeclaration
	: VAR IDENTIFIER COLON Type NewLine
	{ $$ = new VariableDeclaration($2,$4,nullptr); 	}
	| VAR IDENTIFIER COLON Type IS Expression NewLine
	{ $$ = new VariableDeclaration($2,$4,$6); 	}
	| VAR IDENTIFIER IS Expression NewLine
	{ $$ = new VariableDeclaration($2,nullptr,$4); 	}

VariableDeclarationList
	: VariableDeclaration
	{ $$ = new VariableDeclarationList($1);		}
	| VariableDeclarationList VariableDeclaration
	{ $$ = $1->addVariableDeclaration($2);		}

TypeDeclaration
	: TYPE IDENTIFIER IS Type
	{ $$ = new TypeDeclaration($2,$4);		}

Type
	: PrimitiveType
	{ $$ = $1;					}
	| UserType
	{ $$ = $1;					}
	| IDENTIFIER
	{ $$ = new NamedTypeDefinition($1);		}

PrimitiveType
	: INTEGER
	{ $$ = new NamedTypeDefinition(INTEGER);	}
	| REAL
	{ $$ = new NamedTypeDefinition(REAL);		}
	| BOOLEAN
	{ $$ = new NamedTypeDefinition(BOOLEAN);	}

UserType
	: ArrayType
	{ $$ = $1;					}
	| RecordType
	{ $$ = $1;					}

RecordType
	: RECORD NewLine VariableDeclarationList END
	{ $$ = new RecordTypeDefinition($3);		}

ArrayType
	: ARRAY OPEN_BRACKET Expression CLOSE_BRACKET Type
	{ $$ = new ArrayTypeDefinition($3,$5);		}

Statement
	: ExpressionStatement
	{ $$ = $1;	}
//	: Assignment
//	{ $$ = $1; 	}
	| ReturnStatement
	{ $$ = $1;      }
//	| RoutineCall
//	{ $$ = $1; 	}
	| WhileLoop
	{ $$ = $1; 	}
	| ForLoop
	{ $$ = $1; 	}
	| IfStatement
	{ $$ = $1; 	}

ExpressionStatement
	: ModifiablePrimary OP_ASSIGNMENT Expression NewLine
	{ auto binExpr = new BinaryExpression(OP_ASSIGNMENT, $1, $3);
	  $$ = new ExpressionStatement(binExpr); }
	| CallablePrimary NewLine
	{ $$ = new ExpressionStatement($1);	}

//Assignment
//	: ModifiablePrimary OP_ASSIGNMENT Expression
//	{ $$ = new AssignmentStatement($1,$3);	}



ReturnStatement
	: RETURN Expression
	{ $$ = new ReturnStatement($2);		}

//RoutineCall
//	: IDENTIFIER OPEN_PAREN ArgumentList CLOSE_PAREN
//	{ $$ = new RoutineCallStatement($1,$3);	}

ArgumentList
	:
	{ $$ = new ArgumentList(); 		}
	| NonEmptyArgumentList
	{ $$ = $1;				}

NonEmptyArgumentList
	: Expression
	{ $$ = new ArgumentList($1); 		}
	| NonEmptyArgumentList COMMA Expression
	{ $$ = $1->addArgument($3); 		}

WhileLoop
	: WHILE Expression LOOP Body END
	{ $$ = new WhileLoopStatement($2,$4);	 }

ForLoop
	: FOR IDENTIFIER IN Range LOOP Body END
	{ $$ = new ForLoopStatement($2,$4,false,$6);	}
	| FOR IDENTIFIER IN REVERSE Range LOOP Body END
	{ $$ = new ForLoopStatement($2,$5,true, $7); 	}

Range
	: Expression ELLIPSIS Expression
	{ $$ = new Range($1,$3); 		}

IfStatement
	: IF Expression THEN Body END
	{ $$ = new IfStatement($2,$4,nullptr);	}
	| IF Expression THEN Body ELSE Body END
	{ $$ = new IfStatement($2,$4,$6);	}

RoutineDeclaration
	: ROUTINE IDENTIFIER OPEN_PAREN Parameters CLOSE_PAREN IS Body END
	{ $$ = new RoutineDeclaration($2,$4, nullptr, $7);	}
	| ROUTINE IDENTIFIER OPEN_PAREN Parameters CLOSE_PAREN COLON Type IS Body END
	{ $$ = new RoutineDeclaration($2, $4, $7, $9);}

Parameters
	:
	{ $$ = new ParameterList(); 		}
	| NonEmptyParameters
	{ $$ = $1;				}

NonEmptyParameters 
	: ParameterDeclaration
	{ $$ = new ParameterList($1); 		}
	| NonEmptyParameters COMMA ParameterDeclaration
	{ $$ = $1->addParameter($3);		}

//type definitions are not allowed inside function parameters
ParameterDeclaration
	: IDENTIFIER COLON IDENTIFIER
	{ $$ = new ParameterDeclaration($1,$3);	}

Body 
	:
	{ $$ = new Body();			}
	| Body NewLine
	{ $$ = $1; 				}
	| Body SimpleDeclaration
	{ $$ = $1->addNode($2); 		}
	| Body Statement
	{ $$ = $1->addNode($2); 		}

Expression 
	: Relation
	{ $$ = $1;					}
	| Expression OP_AND Relation
	{ $$ = new BinaryExpression(OP_AND,$1,$3);	}
	| Expression OP_OR Relation
	{ $$ = new BinaryExpression(OP_OR,$1,$3);	}
	| Expression OP_XOR Relation
	{ $$ = new BinaryExpression(OP_XOR,$1,$3);	}

Relation 
	: Simple
	{ $$ = $1;						}
	| Simple OP_EQUAL Simple
	{ $$ = new BinaryExpression(OP_EQUAL,$1,$3);		}
	| Simple OP_NOT_EQUAL Simple
	{ $$ = new BinaryExpression(OP_NOT_EQUAL,$1,$3);	}
	| Simple OP_LESS Simple
	{ $$ = new BinaryExpression(OP_LESS,$1,$3);		}
	| Simple OP_GREATER Simple
	{ $$ = new BinaryExpression(OP_GREATER,$1,$3);		}
	| Simple OP_LESS_OR_EQUAL Simple
	{ $$ = new BinaryExpression(OP_LESS_OR_EQUAL,$1,$3);	}
	| Simple OP_GREATER_OR_EQUAL Simple
	{ $$ = new BinaryExpression(OP_GREATER_OR_EQUAL,$1,$3);	}

Simple 
	: Factor
	{ $$ = $1;					}
	| Simple OP_MULTIPLY Factor
	{ $$ = new BinaryExpression(OP_MULTIPLY,$1,$3);	}
	| Simple OP_DIVIDE Factor
	{ $$ = new BinaryExpression(OP_DIVIDE,$1,$3);	}
	| Simple OP_MODULE Factor
	{ $$ = new BinaryExpression(OP_MODULE,$1,$3);	}

Factor 
	: Summand
	{ $$ = $1;					}
	| Factor OP_PLUS Summand
	{ $$ = new BinaryExpression(OP_PLUS,$1,$3);	}
	| Factor OP_MINUS Summand
	{ $$ = new BinaryExpression(OP_MINUS,$1,$3);	}

Summand 
	: Primary
	{ $$ = $1;				}
	| OPEN_PAREN Expression CLOSE_PAREN
	{ $$ = $2; 				}

Primary
	: INTEGER_LITERAL
	{ $$ = new ConstPrimary($1);		}
	| OP_MINUS INTEGER_LITERAL
	{ $$ = new ConstPrimary(0-($2)); 	}
	| OP_PLUS INTEGER_LITERAL
	{ $$ = new ConstPrimary(0+($2));	}
	| REAL_LITERAL
	{ $$ = new ConstPrimary($1);		}
	| OP_MINUS REAL_LITERAL
	{ $$ = new ConstPrimary(0.0f-($2)); 	}
	| OP_PLUS REAL_LITERAL
	{ $$ = new ConstPrimary(0.0f+($2));	}
	| BOOLEAN_LITERAL
	{ $$ = new ConstPrimary($1);		}
	| ModifiablePrimary
	{ $$ = $1;				}
	| CallablePrimary
	{ $$ = $1;				}

ModifiablePrimary 
	: IDENTIFIER
	{ $$ = new ModifiablePrimary($1);	}
	| IDENTIFIER DOT ModifiablePrimary
	{ $$ = new ModifiablePrimary($1,$3);	}
	| IDENTIFIER OPEN_BRACKET Expression CLOSE_BRACKET
	{ $$ = new ModifiablePrimary($1,$3);	}
	| IDENTIFIER OPEN_BRACKET Expression CLOSE_BRACKET DOT ModifiablePrimary
    { $$ = new ModifiablePrimary($1,$3,$6);	}

CallablePrimary
	: ModifiablePrimary OPEN_PAREN ArgumentList CLOSE_PAREN
        { $$ = new CallablePrimary($1,$3);	}

%%