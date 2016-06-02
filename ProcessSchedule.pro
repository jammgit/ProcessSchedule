#-------------------------------------------------
#
# Project created by QtCreator 2016-05-31T14:52:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProcessSchedule
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    schedulealgorithm.cpp

HEADERS  += mainwindow.h \
    schedulealgorithm.h \
    datastruct.h

FORMS    += mainwindow.ui
CONFIG += c++11
