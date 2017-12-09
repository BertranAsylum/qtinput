QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = drag-controller
TEMPLATE = lib

SOURCES += drag-controller.cpp \
           drag-event-notifier.cpp \
    circular-overlay.cpp \
    linear-overlay.cpp \
    overlay.cpp

HEADERS  += drag-controller.h \
            drag-event-notifier.h \
    circular-overlay.h \
    linear-overlay.h \
    overlay.h

target.path += /usr/local/lib
include.path += /usr/local/include/qt-input-tools
include.files += drag-controller.h
INSTALLS += target include
