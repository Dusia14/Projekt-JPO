# TestBazaDanych/TestBazaDanych.pro

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Ścieżki do nagłówków projektu głównego
INCLUDEPATH += ../../include

# Dodanie biblioteki nlohmann/json
DEFINES += JSON_NOEXCEPTION

SOURCES += \
    tst_testbazadanych.cpp \
    ../../src/BazaDanych.cpp \
    ../../src/StacjaPomiarowa.cpp \
    ../../src/CzujnikPomiarowy.cpp \
    ../../src/Pomiar.cpp

HEADERS += \
    ../../include/BazaDanych.h \
    ../../include/StacjaPomiarowa.h \
    ../../include/CzujnikPomiarowy.h \
    ../../include/Pomiar.h

# Dodanie zależności nlohmann/json
unix:!macx: LIBS += -lnlohmann_json

# Dla systemów Windows, trzeba dostosować ścieżkę do biblioteki
win32: {
    # Przykładowa ścieżka dla vcpkg
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    DEPENDPATH += C:/vcpkg/installed/x64-windows/include
}