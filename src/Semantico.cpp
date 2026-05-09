#include "Semantico.h"
#include "Constants.h"

Semantico::Semantico()
    : tipoAtual(""), nomeAtribId(""), tipoAtribId(""), paramPos(0) {}

void Semantico::addErro(const std::string &msg) {
    erros.push_back("ERRO SEMANTICO: " + msg);
}

void Semantico::addAviso(const std::string &msg) {
    avisos.push_back("AVISO: " + msg);
}

int Semantico::mapOperacao(const std::string &lexema) {
    if (lexema == "+")  return SemanticTable::SUM;
    if (lexema == "-")  return SemanticTable::SUB;
    if (lexema == "*")  return SemanticTable::MUL;
    if (lexema == "/")  return SemanticTable::DIV;
    if (lexema == "%")  return SemanticTable::MOD_;
    if (lexema == ">" || lexema == "<" || lexema == ">=" ||
        lexema == "<=" || lexema == "==" || lexema == "!=")
        return SemanticTable::REL;
    if (lexema == "&&") return SemanticTable::AND_;
    if (lexema == "||") return SemanticTable::OR__;
    return -1;
}

void Semantico::finalizarAnalise() {
    auto avs = tabela.getAvisos();
    for (auto &a : avs) avisos.push_back(a);
}

void Semantico::executeAction(int action, const Token *token)
{
    std::string lexema = token ? token->getLexeme() : "";

    switch (action)
    {
        case 1: // Registrar tipo atual
            tipoAtual = lexema;
            break;

        case 2: { // Inserir variavel escalar
            std::string erro = tabela.inserir(lexema, tipoAtual, MOD_VARIAVEL);
            if (!erro.empty()) addErro(erro);
            break;
        }
        case 3: // Armazenar nome do vetor
            nomeAtribId = lexema;
            break;

        case 4: { // Inserir vetor com tamanho
            int tam = 0;
            try { tam = std::stoi(lexema); } catch (...) {}
            std::string erro = tabela.inserir(nomeAtribId, tipoAtual, MOD_VETOR, tam);
            if (!erro.empty()) addErro(erro);
            break;
        }
        case 5: { // ID lado esquerdo (atribuicao/for)
            Symbol *sym = tabela.buscar(lexema);
            if (!sym) {
                addErro("Variavel '" + lexema + "' nao declarada");
                nomeAtribId = lexema;
                tipoAtribId = "";
            } else {
                sym->usada = true;
                nomeAtribId = lexema;
                tipoAtribId = sym->tipo;
            }
            break;
        }
        case 6: { // ID no leia (inicializa)
            Symbol *sym = tabela.buscar(lexema);
            if (!sym) {
                addErro("Variavel '" + lexema + "' nao declarada");
            } else {
                sym->inicializada = true;
                sym->usada = true;
            }
            break;
        }
        case 7: { // ID usado em escreva/indice
            Symbol *sym = tabela.buscar(lexema);
            if (!sym) {
                addErro("Variavel '" + lexema + "' nao declarada");
            } else {
                sym->usada = true;
            }
            break;
        }
        case 8: { // ID em expressao: push tipo
            Symbol *sym = tabela.buscar(lexema);
            if (!sym) {
                addErro("Variavel '" + lexema + "' nao declarada");
                pilhaTipos.push(SemanticTable::ERR);
            } else {
                sym->usada = true;
                pilhaTipos.push(SemanticTable::tipoParaEnum(sym->tipo));
            }
            break;
        }
        case 9:  pilhaTipos.push(SemanticTable::INT); break;
        case 10: pilhaTipos.push(SemanticTable::FLO); break;
        case 11: pilhaTipos.push(SemanticTable::CHA); break;
        case 12: pilhaTipos.push(SemanticTable::STR); break;
        case 13: pilhaTipos.push(SemanticTable::BOO); break;

        case 14: // Preparar atribuicao
            break;

        case 15: { // Verificar compatibilidade de atribuicao
            if (!pilhaTipos.empty()) {
                int tipoExp = pilhaTipos.top(); pilhaTipos.pop();
                int tipoEsq = SemanticTable::tipoParaEnum(tipoAtribId);
                if (tipoEsq >= 0 && tipoExp >= 0) {
                    int res = SemanticTable::atribType(tipoEsq, tipoExp);
                    if (res == SemanticTable::ERR) {
                        addErro("Tipos incompativeis na atribuicao: '" + nomeAtribId +
                                "' (" + tipoAtribId + ") recebe " +
                                SemanticTable::enumParaTipo(tipoExp));
                    } else if (res == SemanticTable::WAR) {
                        addAviso("Possivel perda de dados: '" + nomeAtribId +
                                 "' (" + tipoAtribId + ") recebe " +
                                 SemanticTable::enumParaTipo(tipoExp));
                    }
                }
                Symbol *sym = tabela.buscar(nomeAtribId);
                if (sym) sym->inicializada = true;
            }
            break;
        }
        case 16: // Push operacao
            pilhaOperacoes.push(mapOperacao(lexema));
            break;

        case 17: { // Verificar tipos na expressao
            if (pilhaTipos.size() >= 2 && !pilhaOperacoes.empty()) {
                int t2 = pilhaTipos.top(); pilhaTipos.pop();
                int t1 = pilhaTipos.top(); pilhaTipos.pop();
                int op = pilhaOperacoes.top(); pilhaOperacoes.pop();
                int res = SemanticTable::resultType(t1, t2, op);
                if (res == SemanticTable::ERR) {
                    addErro("Operacao incompativel entre " +
                            SemanticTable::enumParaTipo(t1) + " e " +
                            SemanticTable::enumParaTipo(t2));
                    pilhaTipos.push(SemanticTable::ERR);
                } else {
                    pilhaTipos.push(res);
                }
            }
            break;
        }
        case 20: { // Programa
            tabela.inserir(lexema, "programa", MOD_PROGRAMA);
            tabela.abrirEscopo(lexema);
            break;
        }
        case 21: // Fim programa
            finalizarAnalise();
            tabela.fecharEscopo();
            break;

        case 22: { // Procedimento
            std::string erro = tabela.inserir(lexema, "void", MOD_PROCEDIMENTO);
            if (!erro.empty()) addErro(erro);
            tabela.abrirEscopo(lexema);
            paramPos = 0;
            break;
        }
        case 23: // Fechar escopo subrotina
            tabela.fecharEscopo();
            break;

        case 24: { // Funcao
            std::string erro = tabela.inserir(lexema, tipoAtual, MOD_FUNCAO);
            if (!erro.empty()) addErro(erro);
            tabela.abrirEscopo(lexema);
            paramPos = 0;
            break;
        }
        case 25: { // Parametro escalar
            paramPos++;
            std::string erro = tabela.inserir(lexema, tipoAtual, MOD_PARAMETRO, 0, paramPos);
            if (!erro.empty()) addErro(erro);
            break;
        }
        case 26: { // Parametro vetor
            paramPos++;
            std::string erro = tabela.inserir(lexema, tipoAtual, MOD_PARAMETRO_VETOR, 0, paramPos);
            if (!erro.empty()) addErro(erro);
            break;
        }
        default:
            break;
    }
}
