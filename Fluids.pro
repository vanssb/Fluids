#-------------------------------------------------
#
# Project created by QtCreator 2017-02-23T21:56:10
#
#-------------------------------------------------

QT       += core gui widgets


TARGET = Fluids
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    scene.cpp \
    particle.cpp \
    engine.cpp \
    camera.cpp \
    loader.cpp

HEADERS  += mainwindow.h \
    scene.h \
    particle.h \
    engine.h \
    types.h \
    camera.h \
    loader.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp
