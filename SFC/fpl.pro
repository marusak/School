QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fpl
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
           window.cpp \
           canvas.cpp \
           lorenz.cpp \
           camera.cpp

HEADERS  += window.h \
            canvas.h \
            lorenz.h \
            camera.h

FORMS += window.ui

LIBS += -lglut -lGLU
