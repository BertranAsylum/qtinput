QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = drag-controller
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic

DESTDIR = ../lib
OBJECTS_DIR = ../build/objects
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
RCC_DIR = ../build/rcc

SOURCES += drag-controller.cpp \
    circular-overlay.cpp \
    linear-overlay.cpp \
    overlay.cpp \
    mouse-event-notifier.cpp

HEADERS  += drag-controller.h \
    circular-overlay.h \
    linear-overlay.h \
    overlay.h \
    drag-controller-private.h \
    mouse-event-notifier.h

target.path += /usr/local/lib
include.path += /usr/local/include/qtinput
include.files += drag-controller.h
INSTALLS += target include
