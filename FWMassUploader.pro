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

SOURCES += main.cpp\
        mw.cpp

HEADERS  += mw.h

FORMS    += mw.ui

RC_FILE = icon/graniteicon.rc


macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
macx:ICON=icon/granity.icns
#note to build .dmg on macos, run in the shadow build dir: /Users/tero/softat/5.5/clang_64/bin/macdeployqt DeploymentTool.app -dmg
