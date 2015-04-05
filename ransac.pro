#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T11:45:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ransac
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

OBJECTS_DIR = objects
DESTDIR = bin



INCLUDEPATH += src

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LIBS += -lncurses
QMAKE_CXXFLAGS += -g3
QMAKE_LFLAGS += -g3


SOURCES += \
        src/camera.cpp \
        src/display.cpp \
        src/image.cpp \
        src/logger.cpp \
        src/matrix.cpp \
        src/point2d.cpp \
        src/point3d.cpp \
        src/run.cpp \
        src/scene.cpp \

HEADERS += \
        src/camera.h \
        src/display.h \
        src/image.h \
        src/logger.h \
        src/matrix.h \
        src/point2d.h \
        src/point3d.h \
        src/scene.h \





