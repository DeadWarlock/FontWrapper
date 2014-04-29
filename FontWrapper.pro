#-------------------------------------------------
#
# Project created by QtCreator 2014-04-07T00:13:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtFontWrapper
TEMPLATE = app

QMAKE_CXXFLAGS += -std=gnu++11
#QMAKE_CXXFLAGS += -std=c++11

CONFIG += precompile_header

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ftengine.cpp

PRECOMPILED_HEADER += precompile.h

HEADERS += \
    mainwindow.h \
    ftengine.h \
    precompile.h

FORMS += mainwindow.ui

#RESOURCES += \
#    res_file.qrc

include(freetype.pri)
