#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"
#include "SymbolTable.h"
#include "SemanticTable.h"

#include <string>
#include <vector>
#include <stack>

class Semantico
{
public:
    Semantico();
    void executeAction(int action, const Token *token);
    const SymbolTable& getTabelaSimbolos() const { return tabela; }
    const std::vector<std::string>& getErros() const { return erros; }
    const std::vector<std::string>& getAvisos() const { return avisos; }
    void finalizarAnalise();

private:
    SymbolTable tabela;
    std::string tipoAtual;
    std::string nomeAtribId;
    std::string tipoAtribId;
    int paramPos;
    std::stack<int> pilhaTipos;
    std::stack<int> pilhaOperacoes;
    std::vector<std::string> erros;
    std::vector<std::string> avisos;
    void addErro(const std::string &msg);
    void addAviso(const std::string &msg);
    int mapOperacao(const std::string &lexema);
};

#endif
