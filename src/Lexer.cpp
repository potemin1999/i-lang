/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include "Lexer.h"
#include <iostream>
#include <map>
#include <memory.h>
#include <string>
#include "Config.h"
#include "Tree.h"
#include "generated/Parser.tab.hpp"

using namespace std;

char currentChar;
bool comitted = true;

// в начале используется 1 строка
int currentLine = 1;
// в начале делается ++
int currentPosition = 0;
int lastWasNewLine = false;

FILE *file;

map<string, int> symbolMap = {{"var", VAR},
                              {"is", IS},
                              {"type", TYPE},
                              {"integer", INTEGER},
                              {"real", REAL},
                              {"boolean", BOOLEAN},
                              {"record", RECORD},
                              {"end", END},
                              {"array", ARRAY},
                              {"loop", LOOP},
                              {"for", FOR},
                              {"in", IN},
                              {"reverse", REVERSE},
                              {"if", IF},
                              {"then", THEN},
                              {"else", ELSE},
                              {"\n", NEW_LINE},
                              {"while", WHILE},
                              {"+", OP_PLUS},
                              {"-", OP_MINUS},
                              {"*", OP_MULTIPLY},
                              {"/", OP_DIVIDE},
                              {"routine", ROUTINE},
                              {"return", RETURN},
                              {":", COLON},
                              {"(", OPEN_PAREN},
                              {")", CLOSE_PAREN},
                              {"[", OPEN_BRACKET},
                              {"]", CLOSE_BRACKET},
                              {".", DOT},
                              {"..", ELLIPSIS},
                              {":=", OP_ASSIGNMENT},
                              {"and", OP_AND},
                              {"or", OP_OR},
                              {"xor", OP_XOR},
                              {"=", OP_EQUAL},
                              {"!=", OP_NOT_EQUAL},
                              {"<", OP_LESS},
                              {">", OP_GREATER},
                              {"<=", OP_LESS_OR_EQUAL},
                              {">=", OP_GREATER_OR_EQUAL},
                              {"%", OP_MODULE},

                              {"struct", RECORD},
                              {"func", ROUTINE},

                              {" ", 111},
                              {"\"", 228},
                              {",", COMMA}};

void SetLexerInput(FILE *input) {
    file = input;
}

FILE *GetLexerInput() {
    return file;
}

char ReadChar() {
    if (!comitted) {
        return currentChar;
    }

    int status = fread(&currentChar, sizeof(char), 1, file);
    comitted = false;
    currentPosition++;

    if (lastWasNewLine) {
        currentLine++;
        currentPosition = 1;
        lastWasNewLine = false;
    }

    if (currentChar == '\n') {
        lastWasNewLine = true;
    }

    currentChar = status == 1 ? currentChar : EOF;

    return currentChar;
}

inline void CommitChar() {
    comitted = true;
}

inline bool IsDigit(char symb) {
    return '0' <= symb && symb <= '9';
}

inline bool IsLetter(char symb) {
    return ('a' <= symb && symb <= 'z') || ('A' <= symb && symb <= 'Z');
}

inline bool IsGoodForStartIdentifier(char symb) {
    return IsLetter(symb) || symb == '$' || symb == '_';
}

inline bool IsGoodForIdentifier(char symb) {
    return IsDigit(symb) || IsGoodForStartIdentifier(symb);
}

inline bool IsReserved(string str) {
    int a = symbolMap.count(str);
    return a > 0;
}

inline int GetReserved(string str, int tokenPosition, int tokenLine) {
    if (str == "true" || str == "false") {
        yylval.booleanValue = (str == "true");
        return BOOLEAN_LITERAL;
    }
    if (str == " ") {
        return GetToken();
    }
    // это зарезервированное слово, выдать тип из мапы
    return symbolMap[str];
}

inline int GetIdentifier(string str, int tokenPosition, int tokenLine) {
    yylval.stringValue = strdup(str.c_str());
    return IDENTIFIER;
}

inline int GetNumberLiteral(string str, int tokenPosition, int tokenLine) {
    for (char i : str) {
        if (i == '.') {
            // TODO: conversion errors? two or more dots in a literal will crash this code
            double realValue = ::atof(str.c_str());
            yylval.realValue = realValue;
            return REAL_LITERAL;
        }
    }

    int integerValue = stoi(str);
    yylval.intValue = integerValue;
    return INTEGER_LITERAL;
}

#ifndef STRING_LITERAL
    #define STRING_LITERAL 0
#endif

inline int GetStringLiteral(string &str, int tokenPosition, int tokenLine) {
    yylval.stringValue = strdup(str.c_str());
    return STRING_LITERAL;
}

int GetToken() {
    char symb = ReadChar();
    int tokenLine = currentLine;
    int tokenPosition = currentPosition;
    string currentString(1, symb);

    if (symb == EOF) {
        CommitChar();
        //        fclose(file);
        return -1;
        // return "end";
    }

    if (!IsReserved(currentString) && !IsGoodForIdentifier(symb)) {
        // что-то неправильное
        CommitChar();
        // TODO: handle this "something wrong" cases
        auto msg = "\"" + currentString
                   + "\"is not suitable for reserved keyword and for identifier, panic";
        lexerror(msg.c_str());
        return -2;
    }

    bool isPotentialIdentifier = IsGoodForStartIdentifier(symb);
    bool isNumberLiteral = IsDigit(symb);
    bool wasDotInNumberLiteral = false;
    bool isStringLiteral = symb == '"';
    bool isComment = false;

    while (true) {
        CommitChar();
        char newSymb = ReadChar();

        if (isComment) {
            if (newSymb != '\n') {
                currentString += newSymb;
            } else {
                return GetToken();
            }

            continue;
        }

        if (isStringLiteral) {
            if (newSymb != '"') {
                currentString += newSymb;
                continue;
            } else {
                CommitChar();
                currentString += newSymb;
                return GetStringLiteral(currentString, tokenPosition, tokenLine);
            }
        }

        if (isPotentialIdentifier) {
            if (!IsGoodForIdentifier(newSymb)) {
                return IsReserved(currentString)
                           ? GetReserved(currentString, tokenPosition, tokenLine)
                           : GetIdentifier(currentString, tokenPosition, tokenLine);
            }
        } else if (IsReserved(currentString) && !IsReserved(currentString + newSymb)) {
            if (currentString + newSymb != "//") {
                return GetReserved(currentString, tokenPosition, tokenLine);
            }

            isComment = true;
        }

        if (isNumberLiteral && !IsDigit(newSymb)) {
            if (newSymb == '.' && !wasDotInNumberLiteral) {
                wasDotInNumberLiteral = true;
            } else {
                return GetNumberLiteral(currentString, tokenPosition, tokenLine);
            }
        }

        currentString += newSymb;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-identifier-naming"

int yylex() {
    int tokenType = GetToken();
    if (IsComponentTraced(COMPONENT_LEXER)) {
        bool found = false;
        std::string reverseMapping = "";
        for (auto pair : symbolMap) {
            if (pair.second == tokenType) {
                found = true;
                reverseMapping = pair.first;
            }
        }
        GetTraceStream(COMPONENT_LEXER)
            << "[lexer] token obtained {type=" << tokenType
            << (found ? ",name=" + reverseMapping : "") << "}" << std::endl;
    }
    return tokenType;
}

__attribute__((used)) void yyerror(const char *errorMessage) {
    std::cerr << "Parsing error : " << errorMessage << std::endl;
    // TODO: skip all tokens till sync token (\n) instead of fataling
    std::exit(3);
}

__attribute__((used)) void lexerror(const char *errorMessage) {
    std::cerr << "Lexing error : " << errorMessage << std::endl;
}

#pragma clang diagnostic pop
