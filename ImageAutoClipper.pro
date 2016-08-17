#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T12:53:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageAutoClipper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    io.cpp \
    text.cpp \
    graphicssceneex.cpp \
    graphicsviewex.cpp

HEADERS  += mainwindow.h \
    io.h \
    text.h \
    graphicssceneex.h \
    graphicsviewex.h \
    extcolordefs.h

FORMS    += mainwindow.ui
