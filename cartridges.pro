#-------------------------------------------------
#
# Project created by QtCreator 2014-06-04T00:44:22
#
#-------------------------------------------------

QT       += core gui sql uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cartridges
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui \
    prefDatabaseDialog.ui \
    tableEditor.ui \
    modelDialog.ui \
    logDialog.ui

RESOURCES += \
    res.qrc
