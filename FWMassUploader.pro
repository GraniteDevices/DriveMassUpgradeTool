#-------------------------------------------------
#
# Project created by QtCreator 2017-04-06T00:41:23
#
#-------------------------------------------------

QT       += core gui

include(SimpleMotionV2/SimpleMotionV2.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeploymentTool
TEMPLATE = app

win32 {
    LIBS+=-lws2_32
}

SOURCES += main.cpp\
        mw.cpp

HEADERS  += mw.h

FORMS    += mw.ui

RC_FILE = icon/graniteicon.rc
