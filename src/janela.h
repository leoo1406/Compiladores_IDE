#ifndef JANELA_H
#define JANELA_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QFont>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QHeaderView>

class Semantico;

class Janela : public QMainWindow
{
    Q_OBJECT

public:
    explicit Janela(QWidget *parent = nullptr);
    ~Janela();

private slots:
    void compilar();
    void novoArquivo();
    void abrirArquivo();
    void salvarArquivo();
    void salvarComo();
    void atualizarTitulo();
    void atualizarLinhaColuna();

private:
    QPlainTextEdit *editor;
    QTextEdit *saidaMensagens;
    QTableWidget *tabelaSimbolosWidget;
    QTabWidget *tabWidget;
    QPushButton *botaoCompilar;
    QLabel *labelStatus;
    QString arquivoAtual;
    bool modificado;

    void criarMenus();
    void criarInterface();
    void configurarEditor();
    void atualizarTabelaSimbolos(const Semantico &semantico);
};

#endif
