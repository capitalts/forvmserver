#-------------------------------------------------
#
# Project created by QtCreator 2015-10-20T23:39:28
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ForvmServer
TEMPLATE = app


SOURCES += main.cpp \
    articlereader.cpp \
    updater.cpp \
    server.cpp

HEADERS  += \
    articlereader.h \
    updater.h \
    server.h

FORMS    +=
