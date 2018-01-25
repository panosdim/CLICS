#-------------------------------------------------
#
# Project created by QtCreator 2018-01-18T14:40:43
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CLICS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dbhandler.cpp \
    clicsitem.cpp \
    greekholidays.cpp

HEADERS += \
        mainwindow.h \
    dbhandler.h \
    clicsitem.h \
    greekholidays.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    clics.db3 \
    clics.ico \
    README.md \
    LICENSE

RC_ICONS = clics.ico

win32 {
    PWD_WIN = $${PWD}
    PWD_WIN ~= s,/,\\,g
    OUT_PWD_WIN = $${OUT_PWD}
    OUT_PWD_WIN ~= s,/,\\,g

    CONFIG(debug, debug|release) {
        OUTDIR = debug
    } else {
        OUTDIR = release
    }

    for(file, DISTFILES) {
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($${PWD_WIN}\\$${file}) $$shell_quote($${OUT_PWD_WIN}\\$${OUTDIR}) &
        QMAKE_CLEAN += $${OUTDIR}\\$${file}
    }
}
