// src/main.cpp
#include <QApplication>
#include "GlowneOkno.h"
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Ustawienie polskich znaków
    QLocale::setDefault(QLocale(QLocale::Polish, QLocale::Poland));

    // Tytuł i ikona aplikacji
    app.setApplicationName("Monitor Jakości Powietrza");
    app.setApplicationDisplayName("Monitor Jakości Powietrza");

    // Utworzenie i wyświetlenie głównego okna
    GlowneOkno okno;
    okno.show();

    return app.exec();
}
