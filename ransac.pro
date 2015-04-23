#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T11:45:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ransac
CONFIG   += console debug
CONFIG   -= app_bundle

TEMPLATE = app

OBJECTS_DIR = objects
DESTDIR = bin



INCLUDEPATH += src

QMAKE_CXXFLAGS += -std=c++11  -flto  -g3
QMAKE_LIBS += -lncurses -lopencv_core -lopencv_highgui -lfftw3
QMAKE_LFLAGS += -g3  -flto















SOURCES += \
        src/dense/apply_kernel.cpp \
        src/dense/kernel.cpp \
        src/dense/run_kernel.cpp \
        src/display.cpp \
        src/in/camera.cpp \
        src/in/point3d.cpp \
        src/in/scene.cpp \
        src/out/featurehistory.cpp \
        src/out/image.cpp \
        src/out/point2d.cpp \
        src/out/reconstructor.cpp \
        src/run.cpp \
        src/subimg/context.cpp \
        src/subimg/img_utils.cpp \
        src/subimg/roi.cpp \
        src/subimg/segment.cpp \
        src/subimg/subcontext.cpp \
        src/subimg/subimg.cxx \
        src/tracking/trackedobject.cpp \
        src/util/combination.cpp \
        src/util/crop.cpp \
        src/util/logger.cpp \
        src/util/matrix.cpp \
    src/tracking/trackingcontext.cpp

HEADERS += \
        src/dense/dense_image.h \
        src/dense/image_channel.h \
        src/dense/kernel.h \
        src/display.h \
        src/in/camera.h \
        src/in/point3d.h \
        src/in/scene.h \
        src/out/featurehistory.h \
        src/out/image.h \
        src/out/point2d.h \
        src/out/reconstructor.h \
        src/subimg/context.h \
        src/subimg/roi.h \
        src/subimg/subcontext.h \
        src/tracking/trackedobject.h \
        src/util/combination.h \
        src/util/logger.h \
        src/util/matrix.h \
    src/tracking/trackingcontext.h

