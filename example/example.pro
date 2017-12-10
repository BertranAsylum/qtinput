QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example
TEMPLATE = app

DESTDIR = ../exe
OBJECTS_DIR = ../build/objects
UI_DIR = ../build/ui
RCC_DIR = ../build/rcc

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

LIBS += -L/usr/local/lib -ldrag-controller
