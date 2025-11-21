#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include "tokens.h"

using namespace std;

// Estos vienen de Flex
extern int yylex();
extern char* yytext;
extern int yylineno;
extern FILE* yyin;

class Parser {
public:
    void parse(const string& filename);

private:
    int currentToken; // aqui guardamos el tipo de token actual (TokenType)

    void nextToken();
    void match(int expected);

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
};

#endif