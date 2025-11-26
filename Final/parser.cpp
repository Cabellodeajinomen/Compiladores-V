#include "parser.h"
using namespace std;

// Inicializa el parser sin tokens pendientes ni traza activa.
Parser::Parser()
        : currentToken(TK_EOF),
            lookaheadToken(TK_EOF),
            hasLookahead(false),
            trace(false),
            hadError(false) {}

void Parser::setTrace(bool enable) {
    trace = enable;
}

bool Parser::hasErrors() const {
    return hadError;
}

// obtiene siguiente token
void Parser::nextToken() {
    if (hasLookahead) {
        currentToken = lookaheadToken;
        currentLexeme = lookaheadLexeme;
        hasLookahead = false;
    } else {
        currentToken = yylex();
        if (currentToken == 0)
            currentToken = TK_EOF;
        currentLexeme = yytext ? yytext : "";
    }

    if (currentToken == TK_EOF)
        currentLexeme.clear();

    if (trace) {
        cout << "[TOKEN] " << tokenName(currentToken) << " -> ";
        if (currentToken == TK_EOF) {
            cout << "<EOF>";
        } else if (currentToken == TK_NL) {
            cout << "\\n";
        } else {
            cout << currentLexeme;
        }
        cout << endl;
    }
}

// mira el proximo token sin consumirlo
int Parser::peekToken() {
    if (!hasLookahead) {
        lookaheadToken = yylex();
        if (lookaheadToken == 0)
            lookaheadToken = TK_EOF;
        lookaheadLexeme = yytext ? yytext : "";
        hasLookahead = true;
    }
    return lookaheadToken;
}

// salta saltos de linea
void Parser::skipNL() {
    while (currentToken == TK_NL)
        nextToken();
}

void Parser::reportError(const std::string& message) {
    hadError = true;
    cerr << message << endl;
}

void Parser::synchronize(std::initializer_list<int> recoveryTokens) {
    if (currentToken == TK_EOF)
        return;

    while (currentToken != TK_EOF) {
        for (int token : recoveryTokens) {
            if (currentToken == token) {
                return;
            }
        }

        if (currentToken == TK_NL) {
            nextToken();
            return;
        }

        nextToken();
    }
}

// verifica token esperado
void Parser::match(int expected) {
    if (currentToken == expected)
        nextToken();
    else {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": se esperaba " + tokenName(expected) +
                     " y se encontro '" + currentLexeme +
                     "' (" + tokenName(currentToken) + ")");

        synchronize({expected, TK_END, TK_ELSE, TK_LOOP, TK_FUN, TK_RETURN,
                     TK_IF, TK_WHILE, TK_RPAREN, TK_RBRACKET, TK_COMMA, TK_NL});

        if (currentToken == expected)
            nextToken();
    }
}

// helpers
bool Parser::is_decl_start() {
    return currentToken == TK_FUN || currentToken == TK_ID;
}

bool Parser::is_type_start() {
    return currentToken == TK_INT || currentToken == TK_BOOL ||
           currentToken == TK_CHAR || currentToken == TK_STRING ||
           currentToken == TK_LBRACKET;
}

bool Parser::is_comando_start() {
    return currentToken == TK_IF || currentToken == TK_WHILE ||
           currentToken == TK_RETURN || currentToken == TK_ID;
}

const char* Parser::tokenName(int token) const {
    switch (token) {
        case TK_ID: return "TK_ID";
        case TK_LITNUM: return "TK_LITNUM";
        case TK_LITSTRING: return "TK_LITSTRING";
        case TK_TRUE: return "TK_TRUE";
        case TK_FALSE: return "TK_FALSE";
        case TK_FUN: return "TK_FUN";
        case TK_IF: return "TK_IF";
        case TK_ELSE: return "TK_ELSE";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_LOOP: return "TK_LOOP";
        case TK_RETURN: return "TK_RETURN";
        case TK_NEW: return "TK_NEW";
        case TK_INT: return "TK_INT";
        case TK_BOOL: return "TK_BOOL";
        case TK_CHAR: return "TK_CHAR";
        case TK_STRING: return "TK_STRING";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_GT: return "TK_GT";
        case TK_LT: return "TK_LT";
        case TK_GE: return "TK_GE";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_NEQ: return "TK_NEQ";
        case TK_LPAREN: return "TK_LPAREN";
        case TK_RPAREN: return "TK_RPAREN";
        case TK_LBRACKET: return "TK_LBRACKET";
        case TK_RBRACKET: return "TK_RBRACKET";
        case TK_COLON: return "TK_COLON";
        case TK_COMMA: return "TK_COMMA";
        case TK_ASSIGN: return "TK_ASSIGN";
        case TK_NL: return "TK_NL";
        case TK_EOF: return "TK_EOF";
        case TK_ERROR: return "TK_ERROR";
        default: return "UNKNOWN";
    }
}

// inicio del analisis
// el archivo y lanza el recorrido recursivo.
void Parser::parse(const string& filename) {
    yyin = fopen(filename.c_str(), "r");
    if (!yyin) {
        cerr << "No se pudo abrir archivo\n";
        exit(1);
    }

    hasLookahead = false;
    lookaheadToken = TK_EOF;
    lookaheadLexeme.clear();
    currentLexeme.clear();
    nextToken();
    programa();

    if (currentToken != TK_EOF) {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": tokens extra despues del programa");
        while (currentToken != TK_EOF)
            nextToken();
    }

    if (!hadError)
        cout << "Analisis sintactico exitoso\n";
    else
        cerr << "Analisis completado con errores\n";
    fclose(yyin);
}

// programa 

// programa -> decl decl_list
void Parser::programa() {
    skipNL();
    decl();
    decl_list();
}

// decl_list -> decl decl_list | epsilon
void Parser::decl_list() {
    skipNL();
    while (is_decl_start()) {
        decl();
        skipNL();
    }
}

void Parser::decl() {
    if (currentToken == TK_FUN) funcion();
    else globalDecl();
}

// declaraciones 

void Parser::globalDecl() {
    declvar();
}

// funcion: fun ID() : tipo  NL  bloque  end NL 
// funcion -> 'fun' ID '(' params ')' opt_tipo bloque 'end'
void Parser::funcion() {
    match(TK_FUN);
    match(TK_ID);
    match(TK_LPAREN);
    params();
    match(TK_RPAREN);
    opt_tipo();

    // SOLO avanzar si hay salto de linea
    if (currentToken == TK_NL)
        nextToken();

    bloque();  // NO pongas skipNL antes

    match(TK_END);

    if (currentToken == TK_NL)
        nextToken();
}

// tipo opcional despues de ':' 
void Parser::opt_tipo() {
    if (currentToken == TK_COLON) {
        match(TK_COLON);
        tipo();     // NO LLAMES nextToken() acá
    }
}

// bloque = declaraciones + comandos
void Parser::bloque() {
    skipNL();
    declvar_list();
    skipNL();
    comando_list();
}

// reconoce declaracion solo si es ID ':' 
// declvar_list -> declvar declvar_list | epsilon
void Parser::declvar_list() {
    while (currentToken == TK_ID && peekToken() == TK_COLON) {
        declvar();
        skipNL();
    }
}

// params -> parametro params_tail | epsilon
void Parser::params() {
    if (currentToken == TK_ID) {
        parametro();
        params_tail();
    }
}

// params_tail -> ',' parametro params_tail | epsilon
void Parser::params_tail() {
    while (currentToken == TK_COMMA) {
        match(TK_COMMA);
        parametro();
    }
}

// parametro -> ID ':' tipo
void Parser::parametro() {
    match(TK_ID);
    match(TK_COLON);
    tipo();
}

// x : int 
// declvar -> ID ':' tipo
void Parser::declvar() {
    match(TK_ID);
    match(TK_COLON);
    tipo();
}

// tipo
void Parser::tipo() {
    if (currentToken == TK_LBRACKET) {
        match(TK_LBRACKET);
        match(TK_RBRACKET);  // << ESTA ES LA CORRECCION
        tipo();
    } else tipobase();
}

void Parser::tipobase() {
    if (is_type_start())
        nextToken();
    else {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": tipo base esperado");
        synchronize({TK_COMMA, TK_RPAREN, TK_END, TK_ELSE, TK_LOOP,
                     TK_NL, TK_ASSIGN, TK_RBRACKET});
    }
}

// comandos 

void Parser::comando_list() {
    skipNL();
    while (is_comando_start()) {
        comando();
        skipNL();
    }
}

// comando -> if | while | return | asignacion | llamada
void Parser::comando() {
    if (currentToken == TK_IF) cmdif();
    else if (currentToken == TK_WHILE) cmdwhile();
    else if (currentToken == TK_RETURN) cmdreturn();
    else if (currentToken == TK_ID) cmdatrib();
    else {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": comando invalido");
        synchronize({TK_NL, TK_END, TK_ELSE, TK_LOOP, TK_FUN, TK_EOF});
    }
}

// if ... end
// cmdif -> 'if' exp bloque else_if_list opt_else 'end'
void Parser::cmdif() {
    match(TK_IF);
    exp();
    skipNL();

    bloque();
    skipNL();

    else_if_list();
    opt_else();

    match(TK_END);
}

void Parser::else_if_list() {
    while (currentToken == TK_ELSE && peekToken() == TK_IF) {
        match(TK_ELSE);
        match(TK_IF);
        exp();
        skipNL();
        bloque();
        skipNL();
    }
}

void Parser::opt_else() {
    if (currentToken == TK_ELSE) {
        match(TK_ELSE);
        skipNL();
        bloque();
        skipNL();
    }
}

// while ... loop 
// cmdwhile -> 'while' exp bloque 'loop'
void Parser::cmdwhile() {
    match(TK_WHILE);
    exp();
    skipNL();
    bloque();
    skipNL();
    match(TK_LOOP);
}

// return exp? 
void Parser::cmdreturn() {
    match(TK_RETURN);
    if (currentToken != TK_NL &&
        currentToken != TK_END &&
        currentToken != TK_ELSE &&
        currentToken != TK_LOOP &&
        currentToken != TK_EOF)
        exp();
}

// asignacion o llamada
// cmdatrib -> var '=' exp | llamada
void Parser::cmdatrib() {
    if (peekToken() == TK_LPAREN) {
        llamada();
        return;
    }

    var();

    if (currentToken == TK_ASSIGN) {
        match(TK_ASSIGN);
        exp();
    }
    else {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": se esperaba '=' o una llamada a funcion");
        synchronize({TK_NL, TK_END, TK_ELSE, TK_LOOP, TK_FUN, TK_EOF});
    }
}

// lista de expresiones
// listaexp -> exp listaexp_tail | epsilon
void Parser::listaexp() {
    if (currentToken == TK_ID || currentToken == TK_LITNUM ||
        currentToken == TK_LITSTRING || currentToken == TK_TRUE ||
        currentToken == TK_FALSE || currentToken == TK_NEW ||
        currentToken == TK_LPAREN || currentToken == TK_MINUS ||
        currentToken == TK_NOT) {
        exp();
        listaexp_tail();
    }
}

void Parser::listaexp_tail() {
    while (currentToken == TK_COMMA) {
        match(TK_COMMA);
        exp();
    }
}

void Parser::llamada() {
    match(TK_ID);
    match(TK_LPAREN);
    listaexp();
    match(TK_RPAREN);
}

// variable con indices 
void Parser::var() {
    match(TK_ID);
    var_sufijo();
}

void Parser::var_sufijo() {
    while (currentToken == TK_LBRACKET) {
        match(TK_LBRACKET);
        exp();
        match(TK_RBRACKET);
    }
}

// expresiones 

void Parser::exp() { exp_or(); }

// or
void Parser::exp_or() {
    exp_and();
    exp_or_p();
}

void Parser::exp_or_p() {
    while (currentToken == TK_OR) {
        match(TK_OR);
        exp_and();
    }
}

// and
void Parser::exp_and() {
    exp_eq();
    exp_and_p();
}

void Parser::exp_and_p() {
    while (currentToken == TK_AND) {
        match(TK_AND);
        exp_eq();
    }
}

// == <>
void Parser::exp_eq() {
    exp_rel();
    exp_eq_p();
}

void Parser::exp_eq_p() {
    while (currentToken == TK_EQ ||
           currentToken == TK_NEQ ||
           currentToken == TK_ASSIGN) {
        nextToken();
        exp_rel();
    }
}

// < <= > >=
void Parser::exp_rel() {
    exp_add();
    exp_rel_p();
}

void Parser::exp_rel_p() {
    while (currentToken == TK_LT || currentToken == TK_LE ||
           currentToken == TK_GT || currentToken == TK_GE) {
        nextToken();
        exp_add();
    }
}

// + - 
void Parser::exp_add() {
    exp_mul();
    exp_add_p();
}

void Parser::exp_add_p() {
    while (currentToken == TK_PLUS || currentToken == TK_MINUS) {
        nextToken();
        exp_mul();
    }
}

// * / 
void Parser::exp_mul() {
    exp_unary();
    exp_mul_p();
}

void Parser::exp_mul_p() {
    while (currentToken == TK_MUL || currentToken == TK_DIV) {
        nextToken();
        exp_unary();
    }
}

// unarios 
void Parser::exp_unary() {
    if (currentToken == TK_MINUS || currentToken == TK_NOT) {
        nextToken();
        exp_unary();
    }
    else exp_primary();
}

// primarios 
void Parser::exp_primary() {
    if (currentToken == TK_LITNUM) match(TK_LITNUM);
    else if (currentToken == TK_LITSTRING) match(TK_LITSTRING);
    else if (currentToken == TK_TRUE) match(TK_TRUE);
    else if (currentToken == TK_FALSE) match(TK_FALSE);
    else if (currentToken == TK_NEW) {
        match(TK_NEW);
        match(TK_LBRACKET);
        exp();
        match(TK_RBRACKET);
        tipo();
    }
    else if (currentToken == TK_LPAREN) {
        match(TK_LPAREN);
        exp();
        match(TK_RPAREN);
    }
    else if (currentToken == TK_ID) {
        if (peekToken() == TK_LPAREN) {
            llamada();
        } else {
            var();
        }
    }
    else {
        reportError(string("Error sintactico en linea ") + to_string(yylineno) +
                     ": expresion invalida");
        synchronize({TK_COMMA, TK_RPAREN, TK_RBRACKET, TK_END, TK_ELSE,
                     TK_LOOP, TK_NL, TK_EOF});
    }
}