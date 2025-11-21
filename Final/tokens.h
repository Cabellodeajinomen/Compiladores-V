#ifndef TOKENS_H
#define TOKENS_H

#include <string>
using namespace std;

// Tipos de tokens que devolvera el lexer
enum TokenType {
    TK_ID = 256,      // empezamos en 256 para no chocar con chars simples
    TK_LITNUM,
    TK_LITSTRING,
    TK_TRUE,
    TK_FALSE,

    TK_FUN,
    TK_IF,
    TK_ELSE,
    TK_END,
    TK_WHILE,
    TK_LOOP,
    TK_RETURN,
    TK_NEW,

    TK_INT,
    TK_BOOL,
    TK_CHAR,
    TK_STRING,

    TK_AND,
    TK_OR,
    TK_NOT,

    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,

    TK_GT,
    TK_LT,
    TK_GE,
    TK_LE,
    TK_EQ,
    TK_NEQ,

    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_COLON,
    TK_COMMA,
    TK_ASSIGN,   // '='

    TK_NL,       // salto de linea si decides tokenizarlo
    TK_EOF,
    TK_ERROR
};

struct Token {
    TokenType type;
    string lexeme;
    int line;
};

#endif