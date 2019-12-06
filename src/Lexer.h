/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include <istream>
#ifndef I_LANG_LEXER_H
    #define I_LANG_LEXER_H

#endif // I_LANG_LEXER_H

int GetToken();

void Preparation();

void SetLexerInput(FILE *input);

FILE *GetLexerInput();

extern int yydebug;

int yyparse(void);

int yylex();

void yyerror(const char *errorMessage);

void lexerror(const char *errorMessage);
