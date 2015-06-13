#-------------------------------------------------
#
# Project created by QtCreator 2015-06-11T00:24:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = http-rest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    httprequestworker.cpp

HEADERS  += mainwindow.h \
    httprequestworker.h

FORMS    += mainwindow.ui
