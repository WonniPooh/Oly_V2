QT       += core gui
QT       += network

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = oly_server
CONFIG += console

TEMPLATE = app

SOURCES += main.cpp \
    olyserver.cpp \
    clientconnection.cpp \
    clientnames.cpp \
    olyserverwidget.cpp \
    clientnameswidget.cpp

HEADERS += \
    olyserver.h \
    clientconnection.h \
    clientnames.h \
    olyserverwidget.h \
    clientnameswidget.h
