#ifndef TYPES_H
#define TYPES_h

#include <set>

enum TokenType{
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum Keyword{
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    _NULL,
    THIS
};

const std::set<std::string> KEYWORDS = { "class", "constructor", "function", "method", "field", "static",
                                         "var", "int", "char", "boolean", "void", "true", "false", "null",
                                         "this", "let", "do", "if", "else", "while", "return" };

#endif