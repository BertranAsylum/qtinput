QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = drag-controller
TEMPLATE = lib

SOURCES += drag-controller.cpp \
           drag-event-notifier.cpp

HEADERS  += drag-controller.h \
            drag-event-notifier.h

target.path += /usr/local/lib
include.path += /usr/local/include/qt-input-tools
include.files += drag-controller.h
INSTALLS += target include