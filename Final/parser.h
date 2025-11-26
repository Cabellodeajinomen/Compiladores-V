#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <initializer_list>
#include "tokens.h"

using namespace std;

// Estos vienen de Flex
extern int yylex();
extern char* yytext;
extern int yylineno;
extern FILE* yyin;

class Parser {
public:
    Parser();
    void parse(const string& filename);
    void setTrace(bool enable);
    bool hasErrors() const;

private:
    int currentToken; // aqui guardamos el tipo de token actual (TokenType)
    int lookaheadToken; // buffer para lookahead simple
    bool hasLookahead;
    bool trace;
    bool hadError;
    std::string currentLexeme;
    std::string lookaheadLexeme;

    void nextToken();
    int peekToken();
    void match(int expected);
    void skipNL();
    void reportError(const std::string& message);
    void synchronize(std::initializer_list<int> recoveryTokens);

    // No terminales principales
    void programa();
    void decl_list();
    void decl();
    void globalDecl();
    void funcion();
    void opt_tipo();
    void bloque();
    void declvar_list();
    void comando_list();

    // declaraciones y tipos
    void declvar();
    void params();
    void params_tail();
    void parametro();
    void tipo();
    void tipobase();

    // comandos
    void comando();
    void cmdif();
    void else_if_list();
    void opt_else();
    void cmdwhile();
    void cmdatrib();
    void cmdreturn();
    void llamada();
    void listaexp();
    void listaexp_tail();

    // variables
    void var();
    void var_sufijo();

    // expresiones con precedencia
    void exp();        // alias de exp_or
    void exp_or();
    void exp_or_p();
    void exp_and();
    void exp_and_p();
    void exp_eq();
    void exp_eq_p();
    void exp_rel();
    void exp_rel_p();
    void exp_add();
    void exp_add_p();
    void exp_mul();
    void exp_mul_p();
    void exp_unary();
    void exp_primary();

    // ayuda
    bool is_type_start();
    bool is_decl_start();
    bool is_comando_start();

    const char* tokenName(int token) const;
};

#endif