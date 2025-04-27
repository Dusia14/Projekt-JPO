# TestStacjaPomiarowa/TestStacjaPomiarowa.pro

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Ścieżki do nagłówków projektu głównego
INCLUDEPATH += ../../include

SOURCES += \
    tst_teststacjapomiarowa.cpp \
    ../../src/StacjaPomiarowa.cpp

HEADERS += \
    ../../include/StacjaPomiarowa.h