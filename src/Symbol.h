#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

enum Modalidade {
    MOD_VARIAVEL,
    MOD_VETOR,
    MOD_PARAMETRO,
    MOD_PARAMETRO_VETOR,
    MOD_FUNCAO,
    MOD_PROCEDIMENTO,
    MOD_PROGRAMA
};

struct Symbol {
    int id;
    std::string nome;
    std::string tipo;
    Modalidade modalidade;
    std::string escopo;
    bool inicializada;
    bool usada;
    bool parametro;
    int posicao;
    int tamanhoVetor;

    Symbol()
        : id(0), nome(""), tipo(""), modalidade(MOD_VARIAVEL),
          escopo("global"), inicializada(false), usada(false),
          parametro(false), posicao(0), tamanhoVetor(0) {}

    std::string getModalidadeStr() const {
        switch (modalidade) {
            case MOD_VARIAVEL:       return "variavel";
            case MOD_VETOR:          return "vetor";
            case MOD_PARAMETRO:      return "parametro";
            case MOD_PARAMETRO_VETOR:return "param_vetor";
            case MOD_FUNCAO:         return "funcao";
            case MOD_PROCEDIMENTO:   return "procedimento";
            case MOD_PROGRAMA:       return "programa";
            default:                 return "?";
        }
    }
};

#endif
