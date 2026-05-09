#ifndef SEMANTIC_TABLE_H
#define SEMANTIC_TABLE_H

#include <string>

class SemanticTable {
public:
    enum Types  { INT = 0, FLO, CHA, STR, BOO };
    enum Operations { SUM = 0, SUB, MUL, DIV, REL, MOD_, POT, ROO, AND_, OR__ };
    enum Status { ERR = -1, WAR, OK_ };

    static int const expTable[5][5][10];
    static int const atribTable[5][5];

    static int resultType(int TP1, int TP2, int OP) {
        if (TP1 < 0 || TP2 < 0 || OP < 0 || TP1 > 4 || TP2 > 4 || OP > 9) return ERR;
        return expTable[TP1][TP2][OP];
    }

    static int atribType(int TP1, int TP2) {
        if (TP1 < 0 || TP2 < 0 || TP1 > 4 || TP2 > 4) return ERR;
        return atribTable[TP1][TP2];
    }

    static int tipoParaEnum(const std::string &tipo) {
        if (tipo == "inteiro")   return INT;
        if (tipo == "real")      return FLO;
        if (tipo == "char")      return CHA;
        if (tipo == "string")    return STR;
        if (tipo == "booleano")  return BOO;
        return ERR;
    }

    static std::string enumParaTipo(int t) {
        switch (t) {
            case INT: return "inteiro";
            case FLO: return "real";
            case CHA: return "char";
            case STR: return "string";
            case BOO: return "booleano";
            default:  return "erro";
        }
    }
};

#endif
