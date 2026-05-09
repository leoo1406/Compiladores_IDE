#ifndef CONSTANTS_H
#define CONSTANTS_H

enum TokenId 
{
    EPSILON  = 0,
    DOLLAR   = 1,
    t_PROGRAMA = 2,
    t_VAR = 3,
    t_SE = 4,
    t_ENTAO = 5,
    t_SENAO = 6,
    t_FIM = 7,
    t_TIPO_INTEIRO = 8,
    t_TIPO_REAL = 9,
    t_TIPO_CHAR = 10,
    t_TIPO_STRING = 11,
    t_TIPO_BOOLEANO = 12,
    t_BOOL_VERDADEIRO = 13,
    t_BOOL_FALSO = 14,
    t_ENQUANTO = 15,
    t_PARA = 16,
    t_FACA = 17,
    t_LEIA = 18,
    t_ESCREVA = 19,
    t_PROCEDIMENTO = 20,
    t_FUNCAO = 21,
    t_RETORNA = 22,
    t_OP_MAIOR_IGUAL = 23,
    t_OP_MENOR_IGUAL = 24,
    t_OP_IGUAL = 25,
    t_OP_DIFERENTE = 26,
    t_OP_MAIOR = 27,
    t_OP_MENOR = 28,
    t_OP_SOMA = 29,
    t_OP_SUB = 30,
    t_OP_MULT = 31,
    t_OP_DIV = 32,
    t_OP_MOD = 33,
    t_OP_AND = 34,
    t_OP_OR = 35,
    t_OP_NOT = 36,
    t_BIT_SHIFT_R = 37,
    t_BIT_SHIFT_L = 38,
    t_BIT_AND = 39,
    t_BIT_OR = 40,
    t_BIT_NOT = 41,
    t_BIT_XOR = 42,
    t_DELIM_ESQ_PAREN = 43,
    t_DELIM_DIR_PAREN = 44,
    t_DELIM_ESQ_COLC = 45,
    t_DELIM_DIR_COLC = 46,
    t_DELIM_ESQ_CHAVE = 47,
    t_DELIM_DIR_CHAVE = 48,
    t_PONT_PONTO_VIRGULA = 49,
    t_PONT_VIRGULA = 50,
    t_PONT_PONTO = 51,
    t_PONT_DOIS_PONTOS = 52,
    t_INT_LITERAL = 53,
    t_REAL_LITERAL = 54,
    t_BIN_LITERAL = 55,
    t_HEX_LITERAL = 56,
    t_CHAR_LITERAL = 57,
    t_STRING_LITERAL = 58,
    t_IDENTIFICADOR = 59
};

const int STATES_COUNT = 156;

extern int SCANNER_TABLE[STATES_COUNT][256];

extern int TOKEN_STATE[STATES_COUNT];

extern const char *SCANNER_ERROR[STATES_COUNT];

const int FIRST_SEMANTIC_ACTION = 117;

const int SHIFT  = 0;
const int REDUCE = 1;
const int ACTION = 2;
const int ACCEPT = 3;
const int GO_TO  = 4;
const int ERROR  = 5;

extern const int PARSER_TABLE[263][117][2];

extern const int PRODUCTIONS[135][2];

extern const char *PARSER_ERROR[263];

#endif
