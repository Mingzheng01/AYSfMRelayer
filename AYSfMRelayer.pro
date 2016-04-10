#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T22:54:43
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = AYSfMRelayer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    sfmserverrelayer.cpp \
    relayerserver.cpp

HEADERS += \
    sfmserverrelayer.h \
    relayerserver.h
