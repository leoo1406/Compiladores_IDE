QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IDE_Compilador
TEMPLATE = app

CONFIG += c++17

# Organizar arquivos gerados em pastas separadas
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
DESTDIR = build/bin

SOURCES += \
    src/main.cpp \
    src/janela.cpp \
    src/Lexico.cpp \
    src/Sintatico.cpp \
    src/Semantico.cpp \
    src/Constants.cpp

HEADERS += \
    src/janela.h \
    src/Lexico.h \
    src/Sintatico.h \
    src/Semantico.h \
    src/Token.h \
    src/Constants.h \
    src/AnalysisError.h \
    src/LexicalError.h \
    src/SyntacticError.h \
    src/SemanticError.h

INCLUDEPATH += src
