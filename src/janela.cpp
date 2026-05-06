#include "janela.h"

#include "Lexico.h"
#include "Sintatico.h"
#include "Semantico.h"
#include "Token.h"
#include "LexicalError.h"
#include "SyntacticError.h"
#include "SemanticError.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QDateTime>
#include <iostream>

Janela::Janela(QWidget *parent) :
    QMainWindow(parent),
    arquivoAtual(""),
    modificado(false)
{
    setWindowTitle("IDE Compilador - Sem título");
    resize(900, 700);

    criarInterface();
    criarMenus();
    configurarEditor();

    atualizarTitulo();
}

Janela::~Janela()
{
}

void Janela::criarInterface()
{
    // Widget central
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(6, 6, 6, 6);
    layoutPrincipal->setSpacing(4);

    // Splitter vertical: editor em cima, saida embaixo
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // === EDITOR DE CODIGO (fonte tamanho 14) ===
    editor = new QPlainTextEdit(this);
    QFont fonteEditor("Consolas", 14);
    fonteEditor.setStyleHint(QFont::Monospace);
    editor->setFont(fonteEditor);
    editor->setTabStopDistance(40);
    editor->setPlaceholderText("Digite seu programa aqui...");
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    // === AREA DE SAIDA/MENSAGENS (fonte tamanho 14) ===
    saidaMensagens = new QTextEdit(this);
    QFont fonteSaida("Consolas", 14);
    fonteSaida.setStyleHint(QFont::Monospace);
    saidaMensagens->setFont(fonteSaida);
    saidaMensagens->setReadOnly(true);
    saidaMensagens->setPlaceholderText("Mensagens do compilador aparecerão aqui...");
    saidaMensagens->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }"
    );

    splitter->addWidget(editor);
    splitter->addWidget(saidaMensagens);
    splitter->setStretchFactor(0, 3);  // Editor ocupa mais espaco
    splitter->setStretchFactor(1, 1);

    layoutPrincipal->addWidget(splitter);

    // === BARRA INFERIOR: botao compilar + status ===
    QHBoxLayout *layoutInferior = new QHBoxLayout();

    labelStatus = new QLabel("Linha: 1  |  Coluna: 1", this);

    botaoCompilar = new QPushButton("Compilar", this);
    botaoCompilar->setMinimumHeight(36);
    botaoCompilar->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-size: 14px; "
        "font-weight: bold; padding: 6px 24px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }"
    );

    layoutInferior->addWidget(labelStatus);
    layoutInferior->addStretch();
    layoutInferior->addWidget(botaoCompilar);

    layoutPrincipal->addLayout(layoutInferior);

    // Conectar sinais
    connect(botaoCompilar, &QPushButton::clicked, this, &Janela::compilar);
    connect(editor, &QPlainTextEdit::textChanged, this, &Janela::atualizarTitulo);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Janela::atualizarLinhaColuna);
}

void Janela::criarMenus()
{
    QMenuBar *barra = menuBar();

    // Menu Arquivo
    QMenu *menuArquivo = barra->addMenu("&Arquivo");

    QAction *acaoNovo = new QAction("&Novo", this);
    acaoNovo->setShortcut(QKeySequence::New);
    connect(acaoNovo, &QAction::triggered, this, &Janela::novoArquivo);
    menuArquivo->addAction(acaoNovo);

    QAction *acaoAbrir = new QAction("&Abrir...", this);
    acaoAbrir->setShortcut(QKeySequence::Open);
    connect(acaoAbrir, &QAction::triggered, this, &Janela::abrirArquivo);
    menuArquivo->addAction(acaoAbrir);

    QAction *acaoSalvar = new QAction("&Salvar", this);
    acaoSalvar->setShortcut(QKeySequence::Save);
    connect(acaoSalvar, &QAction::triggered, this, &Janela::salvarArquivo);
    menuArquivo->addAction(acaoSalvar);

    QAction *acaoSalvarComo = new QAction("Salvar &Como...", this);
    acaoSalvarComo->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(acaoSalvarComo, &QAction::triggered, this, &Janela::salvarComo);
    menuArquivo->addAction(acaoSalvarComo);

    menuArquivo->addSeparator();

    QAction *acaoSair = new QAction("Sai&r", this);
    acaoSair->setShortcut(QKeySequence::Quit);
    connect(acaoSair, &QAction::triggered, this, &QWidget::close);
    menuArquivo->addAction(acaoSair);

    // Menu Compilar
    QMenu *menuCompilar = barra->addMenu("&Compilar");

    QAction *acaoCompilar = new QAction("&Compilar", this);
    acaoCompilar->setShortcut(QKeySequence("F9"));
    connect(acaoCompilar, &QAction::triggered, this, &Janela::compilar);
    menuCompilar->addAction(acaoCompilar);
}

void Janela::configurarEditor()
{
    // Editor vazio - sem programa de exemplo
    editor->setPlainText("");
}

// ============================================================
// COMPILAR - Analise Sintatica usando os arquivos do GALS
// ============================================================
void Janela::compilar()
{
    saidaMensagens->clear();

    QString codigoFonte = editor->toPlainText();

    if (codigoFonte.trimmed().isEmpty()) {
        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("Erro: Nenhum código fonte para compilar.");
        return;
    }

    // Converter QString para std::string
    std::string entrada = codigoFonte.toStdString();

    // Criar os analisadores
    Lexico lexico(entrada.c_str());
    Sintatico sintatico;
    Semantico semantico;

    saidaMensagens->setTextColor(QColor("#569cd6"));
    saidaMensagens->append("Compilando...\n");
    std::cout << "Compilando..." << std::endl;

    try
    {
        // Executar a análise sintática (que internamente chama a léxica)
        sintatico.parse(&lexico, &semantico);

        // Se chegou aqui, compilou com sucesso!
        saidaMensagens->setTextColor(QColor("#4ec9b0"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  Programa compilado com sucesso!");
        saidaMensagens->append("========================================");

        std::cout << "\n========================================" << std::endl;
        std::cout << "  Programa compilado com sucesso!" << std::endl;
        std::cout << "========================================" << std::endl;

        statusBar()->showMessage("Compilação bem-sucedida!", 5000);
    }
    catch (LexicalError &e)
    {
        // Erro léxico
        int pos = e.getPosition();
        // Calcular linha e coluna a partir da posição
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO LÉXICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(
            QString("Linha %1, Coluna %2: %3")
                .arg(linha)
                .arg(coluna)
                .arg(QString::fromStdString(e.getMessage()))
        );

        std::cout << "\n========================================" << std::endl;
        std::cout << "ERRO LÉXICO" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Linha " << linha << ", Coluna " << coluna << ": " << e.getMessage() << std::endl;

        // Posicionar cursor no erro
        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(pos);
        editor->setTextCursor(cursor);
        editor->setFocus();

        statusBar()->showMessage(QString("Erro léxico na linha %1").arg(linha), 5000);
    }
    catch (SyntacticError &e)
    {
        // Erro sintático
        int pos = e.getPosition();
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO SINTÁTICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(
            QString("Linha %1, Coluna %2: %3")
                .arg(linha)
                .arg(coluna)
                .arg(QString::fromStdString(e.getMessage()))
        );

        std::cout << "\n========================================" << std::endl;
        std::cout << "ERRO SINTÁTICO" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Linha " << linha << ", Coluna " << coluna << ": " << e.getMessage() << std::endl;

        // Posicionar cursor no erro
        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(qMin(pos, (int)codigoFonte.size() - 1));
        editor->setTextCursor(cursor);
        editor->setFocus();

        statusBar()->showMessage(QString("Erro sintático na linha %1").arg(linha), 5000);
    }
    catch (SemanticError &e)
    {
        // Erro semântico
        int pos = e.getPosition();
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO SEMÂNTICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(
            QString("Linha %1, Coluna %2: %3")
                .arg(linha)
                .arg(coluna)
                .arg(QString::fromStdString(e.getMessage()))
        );

        std::cout << "\n========================================" << std::endl;
        std::cout << "ERRO SEMÂNTICO" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Linha " << linha << ", Coluna " << coluna << ": " << e.getMessage() << std::endl;

        statusBar()->showMessage(QString("Erro semântico na linha %1").arg(linha), 5000);
    }
}

// ============================================================
// FUNCOES DO MENU ARQUIVO
// ============================================================
void Janela::novoArquivo()
{
    editor->clear();
    saidaMensagens->clear();
    arquivoAtual = "";
    modificado = false;
    atualizarTitulo();
}

void Janela::abrirArquivo()
{
    QString nomeArquivo = QFileDialog::getOpenFileName(
        this, "Abrir Arquivo", "",
        "Arquivos de Código (*.txt *.src *.prog);;Todos os Arquivos (*)"
    );

    if (!nomeArquivo.isEmpty()) {
        QFile arquivo(nomeArquivo);
        if (arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            editor->setPlainText(QString::fromUtf8(arquivo.readAll()));
            arquivo.close();
            arquivoAtual = nomeArquivo;
            modificado = false;
            saidaMensagens->clear();
            atualizarTitulo();
        }
    }
}

void Janela::salvarArquivo()
{
    if (arquivoAtual.isEmpty()) {
        salvarComo();
        return;
    }

    QFile arquivo(arquivoAtual);
    if (arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        arquivo.write(editor->toPlainText().toUtf8());
        arquivo.close();
        modificado = false;
        atualizarTitulo();
        statusBar()->showMessage("Arquivo salvo!", 3000);
    }
}

void Janela::salvarComo()
{
    QString nomeArquivo = QFileDialog::getSaveFileName(
        this, "Salvar Como", "",
        "Arquivos de Código (*.txt *.src *.prog);;Todos os Arquivos (*)"
    );

    if (!nomeArquivo.isEmpty()) {
        arquivoAtual = nomeArquivo;
        salvarArquivo();
    }
}

void Janela::atualizarTitulo()
{
    QString titulo = "IDE Compilador - ";
    if (arquivoAtual.isEmpty())
        titulo += "Sem título";
    else
        titulo += arquivoAtual;

    setWindowTitle(titulo);
    modificado = true;
}

void Janela::atualizarLinhaColuna()
{
    QTextCursor cursor = editor->textCursor();
    int linha = cursor.blockNumber() + 1;
    int coluna = cursor.columnNumber() + 1;
    labelStatus->setText(QString("Linha: %1  |  Coluna: %2").arg(linha).arg(coluna));
}
