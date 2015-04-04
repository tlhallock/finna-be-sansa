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


INCLUDEPATH += src

QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
        src/camera.cpp \
        src/image.cpp \
        src/matrix.cpp \
        src/point2d.cpp \
        src/point3d.cpp \
        src/run.cpp \
        src/scene.cpp \
    src/display.cpp

HEADERS += \
        src/camera.h \
        src/image.h \
        src/matrix.h \
        src/point2d.h \
        src/point3d.h \
        src/scene.h \
    src/display.h



