#include "janela.h"
#include "Lexico.h"
#include "Sintatico.h"
#include "Semantico.h"
#include "Token.h"
#include "LexicalError.h"
#include "SyntacticError.h"
#include "SemanticError.h"

#include <QTextCursor>

Janela::Janela(QWidget *parent) :
    QMainWindow(parent), arquivoAtual(""), modificado(false)
{
    setWindowTitle("IDE Compilador - Sem titulo");
    resize(1100, 750);
    criarInterface();
    criarMenus();
    configurarEditor();
    atualizarTitulo();
}

Janela::~Janela() {}

void Janela::criarInterface()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(6, 6, 6, 6);
    layoutPrincipal->setSpacing(4);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // Editor
    editor = new QPlainTextEdit(this);
    QFont fonteEditor("Consolas", 14);
    fonteEditor.setStyleHint(QFont::Monospace);
    editor->setFont(fonteEditor);
    editor->setTabStopDistance(40);
    editor->setPlaceholderText("Digite seu programa aqui...");
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    // Abas: Mensagens + Tabela de Simbolos
    tabWidget = new QTabWidget(this);

    saidaMensagens = new QTextEdit(this);
    QFont fonteSaida("Consolas", 14);
    fonteSaida.setStyleHint(QFont::Monospace);
    saidaMensagens->setFont(fonteSaida);
    saidaMensagens->setReadOnly(true);
    saidaMensagens->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");
    tabWidget->addTab(saidaMensagens, "Mensagens");

    tabelaSimbolosWidget = new QTableWidget(this);
    tabelaSimbolosWidget->setColumnCount(10);
    tabelaSimbolosWidget->setHorizontalHeaderLabels({
        "ID", "Nome", "Tipo", "Modalidade", "Escopo",
        "Inic.", "Usada", "Param.", "Pos.", "Tam.Vet"
    });
    tabelaSimbolosWidget->horizontalHeader()->setStretchLastSection(false);
    tabelaSimbolosWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tabelaSimbolosWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabelaSimbolosWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabelaSimbolosWidget->setAlternatingRowColors(false);
    tabelaSimbolosWidget->setFont(QFont("Consolas", 12));
    tabelaSimbolosWidget->setStyleSheet(
        "QTableWidget { background-color: white; color: black; gridline-color: #ccc; }"
        "QHeaderView::section { background-color: #e0e0e0; color: black; font-weight: bold; "
        "border: 1px solid #ccc; padding: 4px; }"
    );

    // Container com tabela + legenda
    QWidget *tabelaContainer = new QWidget(this);
    QVBoxLayout *tabelaLayout = new QVBoxLayout(tabelaContainer);
    tabelaLayout->setContentsMargins(0, 0, 0, 0);
    tabelaLayout->setSpacing(2);
    tabelaLayout->addWidget(tabelaSimbolosWidget);

    // Legenda
    QLabel *legenda = new QLabel(
        "<span style='color: #cc0000;'>&#9632;</span> <b>Vermelho</b> = Nao usada &nbsp;&nbsp;&nbsp;"
        "<span style='color: #cc8800;'>&#9632;</span> <b>Laranja</b> = Nao inicializada &nbsp;&nbsp;&nbsp;"
        "<span style='color: black;'>&#9632;</span> <b>Preto</b> = OK",
        this
    );
    legenda->setStyleSheet("QLabel { background-color: #f0f0f0; color: black; padding: 4px; border-top: 1px solid #ccc; }");
    tabelaLayout->addWidget(legenda);

    tabWidget->addTab(tabelaContainer, "Tabela de Simbolos");

    splitter->addWidget(editor);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    layoutPrincipal->addWidget(splitter);

    // Barra inferior
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

    connect(botaoCompilar, &QPushButton::clicked, this, &Janela::compilar);
    connect(editor, &QPlainTextEdit::textChanged, this, &Janela::atualizarTitulo);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Janela::atualizarLinhaColuna);
}

void Janela::criarMenus()
{
    QMenuBar *barra = menuBar();
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

    QMenu *menuCompilar = barra->addMenu("&Compilar");
    QAction *acaoCompilar = new QAction("&Compilar", this);
    acaoCompilar->setShortcut(QKeySequence("F9"));
    connect(acaoCompilar, &QAction::triggered, this, &Janela::compilar);
    menuCompilar->addAction(acaoCompilar);
}

void Janela::configurarEditor() { editor->setPlainText(""); }

void Janela::atualizarTabelaSimbolos(const Semantico &semantico)
{
    const auto &tab = semantico.getTabelaSimbolos().getTabela();
    tabelaSimbolosWidget->setRowCount(tab.size());

    for (int i = 0; i < (int)tab.size(); i++) {
        const auto &s = tab[i];
        tabelaSimbolosWidget->setItem(i, 0, new QTableWidgetItem(QString::number(s.id)));
        tabelaSimbolosWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s.nome)));
        tabelaSimbolosWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.tipo)));
        tabelaSimbolosWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(s.getModalidadeStr())));
        tabelaSimbolosWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(s.escopo)));
        tabelaSimbolosWidget->setItem(i, 5, new QTableWidgetItem(s.inicializada ? "T" : "F"));
        tabelaSimbolosWidget->setItem(i, 6, new QTableWidgetItem(s.usada ? "T" : "F"));
        tabelaSimbolosWidget->setItem(i, 7, new QTableWidgetItem(s.parametro ? "T" : "F"));
        tabelaSimbolosWidget->setItem(i, 8, new QTableWidgetItem(QString::number(s.posicao)));
        tabelaSimbolosWidget->setItem(i, 9, new QTableWidgetItem(QString::number(s.tamanhoVetor)));

        // Definir cor do texto: vermelho=nao usada, laranja=nao inicializada, preto=ok
        QColor corTexto = Qt::black;
        if (s.modalidade != MOD_FUNCAO && s.modalidade != MOD_PROCEDIMENTO && s.modalidade != MOD_PROGRAMA) {
            if (!s.usada) corTexto = QColor(204, 0, 0);              // vermelho
            else if (!s.inicializada) corTexto = QColor(204, 136, 0); // laranja
        }

        for (int c = 0; c < 10; c++) {
            if (tabelaSimbolosWidget->item(i, c)) {
                tabelaSimbolosWidget->item(i, c)->setBackground(Qt::white);
                tabelaSimbolosWidget->item(i, c)->setForeground(corTexto);
            }
        }
    }
    tabelaSimbolosWidget->resizeColumnsToContents();
}

void Janela::compilar()
{
    saidaMensagens->clear();
    tabelaSimbolosWidget->setRowCount(0);

    QString codigoFonte = editor->toPlainText();
    if (codigoFonte.trimmed().isEmpty()) {
        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("Erro: Nenhum codigo fonte para compilar.");
        return;
    }

    std::string entrada = codigoFonte.toStdString();
    Lexico lexico(entrada.c_str());
    Sintatico sintatico;
    Semantico semantico;

    saidaMensagens->setTextColor(QColor("#569cd6"));
    saidaMensagens->append("Compilando...\n");

    try
    {
        sintatico.parse(&lexico, &semantico);
        atualizarTabelaSimbolos(semantico);

        const auto &errosSem = semantico.getErros();
        if (!errosSem.empty()) {
            saidaMensagens->setTextColor(QColor("#ff6b6b"));
            for (auto &e : errosSem)
                saidaMensagens->append(QString::fromStdString(e));
        }

        const auto &avisosSem = semantico.getAvisos();
        if (!avisosSem.empty()) {
            saidaMensagens->setTextColor(QColor("#dcdcaa"));
            for (auto &a : avisosSem)
                saidaMensagens->append(QString::fromStdString(a));
        }

        if (errosSem.empty()) {
            saidaMensagens->setTextColor(QColor("#4ec9b0"));
            saidaMensagens->append("\n========================================");
            saidaMensagens->append("  Programa compilado com sucesso!");
            saidaMensagens->append("========================================");
            statusBar()->showMessage("Compilacao bem-sucedida!", 5000);
        } else {
            statusBar()->showMessage(
                QString("Compilacao com %1 erro(s) semantico(s)").arg(errosSem.size()), 5000);
        }

        tabWidget->setCurrentIndex(0);
    }
    catch (LexicalError &e)
    {
        int pos = e.getPosition();
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO LEXICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(QString("Linha %1, Coluna %2: %3").arg(linha).arg(coluna)
            .arg(QString::fromStdString(e.getMessage())));

        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(pos);
        editor->setTextCursor(cursor);
        editor->setFocus();
    }
    catch (SyntacticError &e)
    {
        int pos = e.getPosition();
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO SINTATICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(QString("Linha %1, Coluna %2: %3").arg(linha).arg(coluna)
            .arg(QString::fromStdString(e.getMessage())));

        atualizarTabelaSimbolos(semantico);

        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(qMin(pos, (int)codigoFonte.size() - 1));
        editor->setTextCursor(cursor);
        editor->setFocus();
    }
    catch (SemanticError &e)
    {
        int pos = e.getPosition();
        QString texto = codigoFonte.left(pos);
        int linha = texto.count('\n') + 1;
        int coluna = pos - texto.lastIndexOf('\n');

        saidaMensagens->setTextColor(QColor("#ff6b6b"));
        saidaMensagens->append("========================================");
        saidaMensagens->append("  ERRO SEMANTICO");
        saidaMensagens->append("========================================");
        saidaMensagens->append(QString("Linha %1, Coluna %2: %3").arg(linha).arg(coluna)
            .arg(QString::fromStdString(e.getMessage())));

        atualizarTabelaSimbolos(semantico);
    }
}

void Janela::novoArquivo() {
    editor->clear(); saidaMensagens->clear();
    tabelaSimbolosWidget->setRowCount(0);
    arquivoAtual = ""; modificado = false; atualizarTitulo();
}

void Janela::abrirArquivo() {
    QString nomeArquivo = QFileDialog::getOpenFileName(this, "Abrir Arquivo", "",
        "Arquivos de Codigo (*.txt *.src *.prog);;Todos os Arquivos (*)");
    if (!nomeArquivo.isEmpty()) {
        QFile arquivo(nomeArquivo);
        if (arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            editor->setPlainText(QString::fromUtf8(arquivo.readAll()));
            arquivo.close();
            arquivoAtual = nomeArquivo; modificado = false;
            saidaMensagens->clear(); atualizarTitulo();
        }
    }
}

void Janela::salvarArquivo() {
    if (arquivoAtual.isEmpty()) { salvarComo(); return; }
    QFile arquivo(arquivoAtual);
    if (arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        arquivo.write(editor->toPlainText().toUtf8());
        arquivo.close(); modificado = false; atualizarTitulo();
        statusBar()->showMessage("Arquivo salvo!", 3000);
    }
}

void Janela::salvarComo() {
    QString nomeArquivo = QFileDialog::getSaveFileName(this, "Salvar Como", "",
        "Arquivos de Codigo (*.txt *.src *.prog);;Todos os Arquivos (*)");
    if (!nomeArquivo.isEmpty()) { arquivoAtual = nomeArquivo; salvarArquivo(); }
}

void Janela::atualizarTitulo() {
    QString titulo = "IDE Compilador - ";
    titulo += arquivoAtual.isEmpty() ? "Sem titulo" : arquivoAtual;
    setWindowTitle(titulo); modificado = true;
}

void Janela::atualizarLinhaColuna() {
    QTextCursor cursor = editor->textCursor();
    labelStatus->setText(QString("Linha: %1  |  Coluna: %2")
        .arg(cursor.blockNumber() + 1).arg(cursor.columnNumber() + 1));
}
