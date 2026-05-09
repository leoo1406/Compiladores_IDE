#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "Symbol.h"
#include <vector>
#include <string>
#include <stack>

class SymbolTable {
public:
    SymbolTable() : nextId(0), scopeCounter(0) {
        escopoAtual = "global";
        pilhaEscopos.push("global");
    }

    void reset() {
        tabela.clear();
        while (!pilhaEscopos.empty()) pilhaEscopos.pop();
        escopoAtual = "global";
        pilhaEscopos.push("global");
        nextId = 0;
        scopeCounter = 0;
    }

    std::string inserir(const std::string &nome, const std::string &tipo,
                        Modalidade mod, int tamVetor = 0, int pos = 0) {
        for (auto &s : tabela) {
            if (s.nome == nome && s.escopo == escopoAtual)
                return "Identificador '" + nome + "' ja declarado no escopo '" + escopoAtual + "'";
        }
        Symbol sym;
        sym.id = nextId++;
        sym.nome = nome;
        sym.tipo = tipo;
        sym.modalidade = mod;
        sym.escopo = escopoAtual;
        sym.inicializada = (mod == MOD_PARAMETRO || mod == MOD_PARAMETRO_VETOR);
        sym.usada = false;
        sym.parametro = (mod == MOD_PARAMETRO || mod == MOD_PARAMETRO_VETOR);
        sym.posicao = pos;
        sym.tamanhoVetor = tamVetor;
        tabela.push_back(sym);
        return "";
    }

    Symbol* buscar(const std::string &nome) {
        std::stack<std::string> temp = pilhaEscopos;
        while (!temp.empty()) {
            std::string esc = temp.top();
            temp.pop();
            for (auto &s : tabela) {
                if (s.nome == nome && s.escopo == esc) return &s;
            }
        }
        return nullptr;
    }

    void abrirEscopo(const std::string &nomeBase) {
        scopeCounter++;
        escopoAtual = nomeBase;
        pilhaEscopos.push(escopoAtual);
    }

    void fecharEscopo() {
        if (pilhaEscopos.size() > 1) {
            pilhaEscopos.pop();
            escopoAtual = pilhaEscopos.top();
        }
    }

    std::vector<std::string> getAvisos() const {
        std::vector<std::string> avisos;
        for (auto &s : tabela) {
            if (s.modalidade == MOD_FUNCAO || s.modalidade == MOD_PROCEDIMENTO ||
                s.modalidade == MOD_PROGRAMA) continue;
            if (!s.usada)
                avisos.push_back("AVISO: Variavel '" + s.nome +
                    "' declarada no escopo '" + s.escopo + "' nunca foi usada");
            if (s.usada && !s.inicializada)
                avisos.push_back("AVISO: Variavel '" + s.nome +
                    "' usada no escopo '" + s.escopo + "' sem ser inicializada (lixo de memoria)");
        }
        return avisos;
    }

    std::string getEscopoAtual() const { return escopoAtual; }
    const std::vector<Symbol>& getTabela() const { return tabela; }

private:
    std::vector<Symbol> tabela;
    std::stack<std::string> pilhaEscopos;
    std::string escopoAtual;
    int nextId;
    int scopeCounter;
};

#endif
