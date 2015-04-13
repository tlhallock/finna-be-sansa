#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T11:45:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = another_test
CONFIG   += console debug
CONFIG   -= app_bundle

TEMPLATE = app

OBJECTS_DIR = ../objects
DESTDIR = ../bin

INCLUDEPATH += src

QMAKE_CXXFLAGS += -std=c++11 -g3
QMAKE_LIBS += -lfftw3
QMAKE_LFLAGS += -g3

SOURCES += another_test.cpp

