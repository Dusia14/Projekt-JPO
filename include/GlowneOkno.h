// include/GlowneOkno.h
#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <memory>
#include <thread>
#include <mutex>
#include "KlientAPI.h"
#include "BazaDanych.h"

class GlowneOkno : public QMainWindow {
    Q_OBJECT

public:
    GlowneOkno(QWidget* parent = nullptr);
    ~GlowneOkno();

private slots:
    void odswiezListeStacji();
    void filtrujStacjePoMiescie(const QString& miasto);
    void wybranoStacje(int wiersz);
    void wybranoCzujnik(int wiersz);
    void pokazWykres();
    void przeanalizujDane();

private:
    void wypelnijListeStacji();

    // Komponenty GUI
    QListWidget* listaStacji;
    QListWidget* listaCzujnikow;
    QComboBox* filtrMiasta;
    QDateEdit* dataOd;
    QDateEdit* dataDo;
    QPushButton* przyciskOdswiez;
    QPushButton* przyciskWykres;
    QChartView* widokWykresu;
    QLabel* etykietaStatus;
    QLabel* etykietaAnaliza;

    // Dane aplikacji
    std::unique_ptr<KlientAPI> klientAPI;
    std::unique_ptr<BazaDanych> bazaDanych;
    std::vector<StacjaPomiarowa> stacje;
    std::vector<CzujnikPomiarowy> aktualneCzujniki;
    std::vector<Pomiar> aktualnePomiary;
    int idAktualnejStacji = -1;
    int idAktualnegoCzujnika = -1;

    // Mutex dla operacji wielowątkowych
    std::mutex mutexDanych;

    // Metody GUI
    void utworzUklad();
    void podlaczSygnaly();
    void wczytajDaneStacji();
    void zaladujCzujnikiDlaStacji(int idStacji);
    void zaladujPomiaryDlaCzujnika(int idCzujnika);
    void odswiezWykres();
    void analizujDane();

    // Metody pomocnicze
    void pokazBlad(const QString& tytul, const QString& komunikat);
    void pokazInfo(const QString& tytul, const QString& komunikat);
    void aktualizujStatus(const QString& komunikat);
};
