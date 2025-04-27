# MonitorPowietrza.pro
QT += core gui charts widgets network concurrent
QMAKE_LIBDIR += "C:\Qt\6.9.0\mingw_64\lib"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = MonitorPowietrza
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/GlowneOkno.cpp \
    src/StacjaPomiarowa.cpp \
    src/CzujnikPomiarowy.cpp \
    src/Pomiar.cpp \
    src/KlientAPI.cpp \
    src/IndeksJakosci.cpp \
    src/BazaDanych.cpp

HEADERS += \
    include/GlowneOkno.h \
    include/StacjaPomiarowa.h \
    include/CzujnikPomiarowy.h \
    include/Pomiar.h \
    include/KlientAPI.h \
    include/IndeksJakosci.h \
    include/BazaDanych.h

INCLUDEPATH += include/



# Dla Windows podajemy ścieżkę do biblioteki cURL
win32 {
    # Ścieżka do plików nagłówkowych cURL
    INCLUDEPATH += C:\curl-8.13.0_2-win64-mingw\include

    LIBS += -L$$quote(C:/curl-8.13.0_2-win64-mingw/lib) -lcurl

}

# Utworzenie katalogu na dane
DESTDIR = $$PWD/build
mkpath($$DESTDIR/dane)


