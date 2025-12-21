QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = NJUPTLibrary
TEMPLATE = app

SOURCES += \
    LoginDialog.cpp \
    main.cpp \
    MainWindow.cpp \
    LibraryManager.cpp \
    Book.cpp


HEADERS += \
    LoginDialog.h \
    MainWindow.h \
    LibraryManager.h \
    Book.h
