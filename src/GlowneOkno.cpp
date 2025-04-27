// src/GlowneOkno.cpp
#include "include/GlowneOkno.h"
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QGridLayout>
#include <QSplitter>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <numeric>
#include <future>
#include <iostream>

GlowneOkno::GlowneOkno(QWidget* parent) : QMainWindow(parent) {
    // Tworzenie obiektów klienta API i bazy danych
    klientAPI = std::make_unique<KlientAPI>();
    bazaDanych = std::make_unique<BazaDanych>();
    
    // Inicjalizacja GUI
    setWindowTitle("Monitor Jakości Powietrza");
    resize(1024, 768);
    
    // Wyśrodkowanie okna na ekranie
    QRect geometriaEkranu = QGuiApplication::primaryScreen()->geometry();
    int x = (geometriaEkranu.width() - width()) / 2;
    int y = (geometriaEkranu.height() - height()) / 2;
    move(x, y);
    
    // Tworzenie układu
    utworzUklad();
    
    // Podłączanie sygnałów i slotów
    podlaczSygnaly();
    
    // Wczytanie danych stacji
    wczytajDaneStacji();
    
    // Ustawienie zakresu dat (ostatnie 7 dni)
    QDate dzisiaj = QDate::currentDate();
    dataOd->setDate(dzisiaj.addDays(-7));
    dataDo->setDate(dzisiaj);
    
    // Status początkowy
    aktualizujStatus("Gotowy. Wybierz stację pomiarową.");
}

GlowneOkno::~GlowneOkno() {
    // Czyszczenie zasobów
}

void GlowneOkno::utworzUklad() {
    // Główny widget i layout
    QWidget* centralnyWidget = new QWidget(this);
    QVBoxLayout* glownyUklad = new QVBoxLayout(centralnyWidget);
    
    // Górny panel z filtrami
    QGroupBox* grupaFiltrow = new QGroupBox("Filtry", centralnyWidget);
    QHBoxLayout* ukladFiltrow = new QHBoxLayout(grupaFiltrow);
    
    QLabel* etykietaMiasto = new QLabel("Miasto:", grupaFiltrow);
    // Fragment metody utworzUklad(), sprawdzamy konfigurację filtrMiasta
    filtrMiasta = new QComboBox(grupaFiltrow);
    filtrMiasta->setEditable(true);
    filtrMiasta->setInsertPolicy(QComboBox::NoInsert);
    // Ustawmy minimalne wymiary, by był widoczny
    filtrMiasta->setMinimumWidth(150);
    
    ukladFiltrow->addWidget(etykietaMiasto);
    ukladFiltrow->addWidget(filtrMiasta);
    ukladFiltrow->addStretch();
    
    // Panel danych
    QSplitter* rozdzielacz = new QSplitter(Qt::Horizontal, centralnyWidget);
    
    // Panel lewy - stacje i czujniki
    QWidget* panelLewy = new QWidget(rozdzielacz);
    QVBoxLayout* ukladLewy = new QVBoxLayout(panelLewy);
    
    QLabel* etykietaStacje = new QLabel("Stacje pomiarowe:", panelLewy);
    listaStacji = new QListWidget(panelLewy);
    
    QLabel* etykietaCzujniki = new QLabel("Czujniki:", panelLewy);
    listaCzujnikow = new QListWidget(panelLewy);
    
    przyciskOdswiez = new QPushButton("Odśwież dane", panelLewy);
    
    ukladLewy->addWidget(etykietaStacje);
    ukladLewy->addWidget(listaStacji);
    ukladLewy->addWidget(etykietaCzujniki);
    ukladLewy->addWidget(listaCzujnikow);
    ukladLewy->addWidget(przyciskOdswiez);
    
    // Panel prawy - wykres i analiza
    QWidget* panelPrawy = new QWidget(rozdzielacz);
    QVBoxLayout* ukladPrawy = new QVBoxLayout(panelPrawy);
    
    // Panel dat
    QGroupBox* grupaDat = new QGroupBox("Zakres dat", panelPrawy);
    QHBoxLayout* ukladDat = new QHBoxLayout(grupaDat);
    
    QLabel* etykietaOd = new QLabel("Od:", grupaDat);
    dataOd = new QDateEdit(grupaDat);
    dataOd->setCalendarPopup(true);
    
    QLabel* etykietaDo = new QLabel("Do:", grupaDat);
    dataDo = new QDateEdit(grupaDat);
    dataDo->setCalendarPopup(true);
    
    przyciskWykres = new QPushButton("Pokaż wykres", grupaDat);
    
    ukladDat->addWidget(etykietaOd);
    ukladDat->addWidget(dataOd);
    ukladDat->addWidget(etykietaDo);
    ukladDat->addWidget(dataDo);
    ukladDat->addWidget(przyciskWykres);
    
    // Wykres
    QChart* wykres = new QChart();
    wykres->setTitle("Pomiary jakości powietrza");
    wykres->legend()->setVisible(true);
    wykres->legend()->setAlignment(Qt::AlignBottom);
    
    widokWykresu = new QChartView(wykres, panelPrawy);
    widokWykresu->setRenderHint(QPainter::Antialiasing);
    
    // Panel analizy
    QGroupBox* grupaAnalizy = new QGroupBox("Analiza danych", panelPrawy);
    QVBoxLayout* ukladAnalizy = new QVBoxLayout(grupaAnalizy);
    
    etykietaAnaliza = new QLabel("Wybierz stację i czujnik, aby zobaczyć analizę.", grupaAnalizy);
    etykietaAnaliza->setWordWrap(true);
    
    ukladAnalizy->addWidget(etykietaAnaliza);
    
    // Dodaj wszystko do prawego panelu
    ukladPrawy->addWidget(grupaDat);
    ukladPrawy->addWidget(widokWykresu);
    ukladPrawy->addWidget(grupaAnalizy);
    
    // Dostosowanie rozdzielacza
    rozdzielacz->addWidget(panelLewy);
    rozdzielacz->addWidget(panelPrawy);
    rozdzielacz->setSizes(QList<int>() << 300 << 700);
    
    // Status
    etykietaStatus = new QLabel("Gotowy", centralnyWidget);
    
    // Dodaj wszystko do głównego układu
    glownyUklad->addWidget(grupaFiltrow);
    glownyUklad->addWidget(rozdzielacz);
    glownyUklad->addWidget(etykietaStatus);
    
    setCentralWidget(centralnyWidget);
}

void GlowneOkno::podlaczSygnaly() {
    connect(przyciskOdswiez, &QPushButton::clicked, this, &GlowneOkno::odswiezListeStacji);

    // Usuńmy stare połączenie
    disconnect(filtrMiasta, &QComboBox::currentTextChanged, this, &GlowneOkno::filtrujStacjePoMiescie);

    // Dodajmy dwa połączenia dla pewności, że któreś zadziała
    connect(filtrMiasta, &QComboBox::currentTextChanged, this, &GlowneOkno::filtrujStacjePoMiescie);
    connect(filtrMiasta, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                filtrujStacjePoMiescie(filtrMiasta->itemText(index));
            });

    connect(listaStacji, &QListWidget::currentRowChanged, this, &GlowneOkno::wybranoStacje);
    connect(listaCzujnikow, &QListWidget::currentRowChanged, this, &GlowneOkno::wybranoCzujnik);
    connect(przyciskWykres, &QPushButton::clicked, this, &GlowneOkno::pokazWykres);
}

// src/GlowneOkno.cpp
void GlowneOkno::wczytajDaneStacji() {
    // Najpierw spróbuj załadować dane lokalne
    bool daneZaladowane = false;

    // Debugowanie
    std::cerr << "Wczytywanie danych stacji..." << std::endl;

    if (bazaDanych->czyDaneIstnieja("stacje")) {
        auto lokalneStacje = bazaDanych->wczytajStacje();
        if (lokalneStacje.has_value() && !lokalneStacje.value().empty()) {
            stacje = lokalneStacje.value();
            daneZaladowane = true;
            std::cerr << "Załadowano " << stacje.size() << " stacji z lokalnej bazy danych." << std::endl;
            aktualizujStatus("Załadowano stacje z lokalnej bazy danych.");
        } else {
            std::cerr << "Plik stacji istnieje, ale nie zawiera poprawnych danych." << std::endl;
        }
    } else {
        std::cerr << "Nie znaleziono lokalnego pliku stacji." << std::endl;
    }

    // Jeśli nie ma danych lokalnych lub są niepełne, pobierz z API
    if (!daneZaladowane) {
        std::cerr << "Próba pobrania stacji z API..." << std::endl;

        // Dla uproszczenia testów, spróbujmy pobrać dane synchronicznie
        try {
            aktualizujStatus("Pobieranie listy stacji z API...");
            auto noweStacje = klientAPI->pobierzStacje();

            if (!noweStacje.empty()) {
                stacje = noweStacje;
                bazaDanych->zapiszStacje(stacje);
                std::cerr << "Pobrano " << stacje.size() << " stacji z API." << std::endl;
                aktualizujStatus("Pobrano " + QString::number(stacje.size()) + " stacji pomiarowych.");
                daneZaladowane = true;
            } else {
                std::cerr << "Pobrano pustą listę stacji z API." << std::endl;
                aktualizujStatus("Pobrano pustą listę stacji. Sprawdź połączenie z internetem.");
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas pobierania stacji z API: " << e.what() << std::endl;
            pokazBlad("Błąd API", QString("Nie udało się pobrać stacji: ") + e.what());
            aktualizujStatus("Błąd podczas pobierania stacji.");
        }
    }

    // Wypełnij listę stacji tylko jeśli dane zostały załadowane
    if (daneZaladowane && !stacje.empty()) {
        wypelnijListeStacji();
    } else {
        std::cerr << "Brak danych stacji do wyświetlenia!" << std::endl;
        aktualizujStatus("Nie znaleziono żadnych stacji. Sprawdź połączenie z internetem.");
    }
}

void GlowneOkno::wypelnijListeStacji() {
    listaStacji->clear();
    filtrMiasta->clear();

    // Zbiór unikalnych miast
    std::set<std::string> miasta;

    // Dodajmy sprawdzenie, czy mamy jakiekolwiek stacje
    if (stacje.empty()) {
        std::cerr << "Brak stacji do wyświetlenia!" << std::endl;
        filtrMiasta->addItem("Wszystkie miasta");
        return;
    }

    for (const auto& stacja : stacje) {
        QString nazwa = QString::fromStdString(stacja.pobierzNazwe());
        QString miasto = QString::fromStdString(stacja.pobierzMiasto());

        // Dodajmy sprawdzenie niepustości miasta
        if (!stacja.pobierzMiasto().empty()) {
            miasta.insert(stacja.pobierzMiasto());
        }

        // Dodaj do listy stacji
        QString tekst = nazwa;
        if (!miasto.isEmpty()) {
            tekst += " (" + miasto + ")";
        }

        QListWidgetItem* element = new QListWidgetItem(tekst, listaStacji);
        element->setData(Qt::UserRole, stacja.pobierzId());
    }

    // Dodaj miasta do filtra
    filtrMiasta->clear(); // wyczyść komponent przed dodaniem nowych elementów
    filtrMiasta->addItem("Wszystkie miasta");

    std::cerr << "Dodaję " << miasta.size() << " miast do filtra" << std::endl;

    for (const auto& miasto : miasta) {
        if (!miasto.empty()) {
            QString qMiasto = QString::fromStdString(miasto);
            std::cerr << "Dodaję miasto: " << miasto << std::endl;
            filtrMiasta->addItem(qMiasto);
        }
    }
}

void GlowneOkno::filtrujStacjePoMiescie(const QString& miasto) {
    // Dodajmy debugowanie
    std::cerr << "Filtrowanie po mieście: " << miasto.toStdString() << std::endl;

    if (miasto.isEmpty() || miasto == "Wszystkie miasta") {
        // Pokaż wszystkie stacje
        for (int i = 0; i < listaStacji->count(); ++i) {
            listaStacji->item(i)->setHidden(false);
        }
    } else {
        // Filtruj stacje po mieście
        for (int i = 0; i < listaStacji->count(); ++i) {
            QListWidgetItem* element = listaStacji->item(i);
            int idStacji = element->data(Qt::UserRole).toInt();

            bool ukryj = true;
            for (const auto& stacja : stacje) {
                if (stacja.pobierzId() == idStacji &&
                    QString::fromStdString(stacja.pobierzMiasto()) == miasto) {
                    ukryj = false;
                    break;
                }
            }

            element->setHidden(ukryj);
        }
    }
}

void GlowneOkno::wybranoStacje(int wiersz) {
    if (wiersz >= 0) {
        int idStacji = listaStacji->item(wiersz)->data(Qt::UserRole).toInt();
        idAktualnejStacji = idStacji;
        
        // Pobierz czujniki dla wybranej stacji
        zaladujCzujnikiDlaStacji(idStacji);
    }
}

void GlowneOkno::zaladujCzujnikiDlaStacji(int idStacji) {
    listaCzujnikow->clear();
    
    // Najpierw spróbuj załadować dane lokalne
    bool daneZaladowane = false;
    
    if (bazaDanych->czyDaneIstnieja("czujniki", idStacji)) {
        auto lokalneCzujniki = bazaDanych->wczytajCzujniki(idStacji);
        if (lokalneCzujniki.has_value()) {
            aktualneCzujniki = lokalneCzujniki.value();
            daneZaladowane = true;
            aktualizujStatus("Załadowano czujniki z lokalnej bazy danych.");
            
            // Wypełnij listę czujników
            for (const auto& czujnik : aktualneCzujniki) {
                QString nazwa = QString::fromStdString(czujnik.pobierzNazweParametru());
                QString symbol = QString::fromStdString(czujnik.pobierzSymbolParametru());
                
                QString tekst = nazwa;
                if (!symbol.isEmpty()) {
                    tekst += " (" + symbol + ")";
                }
                
                QListWidgetItem* element = new QListWidgetItem(tekst, listaCzujnikow);
                element->setData(Qt::UserRole, czujnik.pobierzId());
            }
        }
    }
    
    // Jeśli nie ma danych lokalnych, pobierz z API
    if (!daneZaladowane) {
        try {
            // Pobieranie w osobnym wątku
            std::thread([this, idStacji]() {
                try {
                    std::lock_guard<std::mutex> blokada(mutexDanych);
                    aktualizujStatus("Pobieranie czujników z API...");
                    
                    auto noweCzujniki = klientAPI->pobierzCzujniki(idStacji);
                    
                    QMetaObject::invokeMethod(this, [this, noweCzujniki, idStacji]() {
                        aktualneCzujniki = noweCzujniki;
                        
                        // Wypełnij listę czujników
                        for (const auto& czujnik : aktualneCzujniki) {
                            QString nazwa = QString::fromStdString(czujnik.pobierzNazweParametru());
                            QString symbol = QString::fromStdString(czujnik.pobierzSymbolParametru());
                            
                            QString tekst = nazwa;
                            if (!symbol.isEmpty()) {
                                tekst += " (" + symbol + ")";
                            }
                            
                            QListWidgetItem* element = new QListWidgetItem(tekst, listaCzujnikow);
                            element->setData(Qt::UserRole, czujnik.pobierzId());
                        }
                        
                        // Zapisz do lokalnej bazy
                        bazaDanych->zapiszCzujniki(idStacji, aktualneCzujniki);
                        
                        aktualizujStatus("Pobrano " + QString::number(aktualneCzujniki.size()) + " czujników pomiarowych.");
                    }, Qt::QueuedConnection);
                } catch (const std::exception& e) {
                    QMetaObject::invokeMethod(this, [this, e]() {
                        pokazBlad("Błąd API", QString("Nie udało się pobrać czujników: ") + e.what());
                        aktualizujStatus("Błąd podczas pobierania czujników.");
                    }, Qt::QueuedConnection);
                }
            }).detach();
        } catch (const std::exception& e) {
            pokazBlad("Błąd wątku", QString("Nie można utworzyć wątku: ") + e.what());
        }
    }
}

void GlowneOkno::wybranoCzujnik(int wiersz) {
    if (wiersz >= 0) {
        int idCzujnika = listaCzujnikow->item(wiersz)->data(Qt::UserRole).toInt();
        idAktualnegoCzujnika = idCzujnika;
        
        // Pobierz pomiary dla wybranego czujnika
        zaladujPomiaryDlaCzujnika(idCzujnika);
    }
}

void GlowneOkno::zaladujPomiaryDlaCzujnika(int idCzujnika) {
    // Najpierw spróbuj załadować dane lokalne
    bool daneZaladowane = false;
    
    if (bazaDanych->czyDaneIstnieja("pomiary", idCzujnika)) {
        auto lokalnePomiary = bazaDanych->wczytajPomiary(idCzujnika);
        if (lokalnePomiary.has_value()) {
            aktualnePomiary = lokalnePomiary.value();
            daneZaladowane = true;
            aktualizujStatus("Załadowano pomiary z lokalnej bazy danych.");
            
            // Pokaż wykres
            odswiezWykres();
            
            // Pokaż analizę
            analizujDane();
        }
    }
    
    // Jeśli nie ma danych lokalnych, pobierz z API
    if (!daneZaladowane) {
        try {
            // Pobieranie w osobnym wątku
            std::thread([this, idCzujnika]() {
                try {
                    std::lock_guard<std::mutex> blokada(mutexDanych);
                    aktualizujStatus("Pobieranie pomiarów z API...");
                    
                    auto nowePomiary = klientAPI->pobierzPomiary(idCzujnika);
                    
                    QMetaObject::invokeMethod(this, [this, nowePomiary, idCzujnika]() {
                        aktualnePomiary = nowePomiary;
                        
                        // Zapisz do lokalnej bazy
                        bazaDanych->zapiszPomiary(idCzujnika, aktualnePomiary);
                        
                        aktualizujStatus("Pobrano " + QString::number(aktualnePomiary.size()) + " pomiarów.");
                        
                        // Pokaż wykres
                        odswiezWykres();
                        
                        // Pokaż analizę
                        analizujDane();
                    }, Qt::QueuedConnection);
                } catch (const std::exception& e) {
                    QMetaObject::invokeMethod(this, [this, e]() {
                        pokazBlad("Błąd API", QString("Nie udało się pobrać pomiarów: ") + e.what());
                        aktualizujStatus("Błąd podczas pobierania pomiarów.");
                    }, Qt::QueuedConnection);
                }
            }).detach();
        } catch (const std::exception& e) {
            pokazBlad("Błąd wątku", QString("Nie można utworzyć wątku: ") + e.what());
        }
    }
}

void GlowneOkno::pokazWykres() {
    if (idAktualnegoCzujnika < 0) {
        pokazBlad("Błąd", "Wybierz najpierw czujnik pomiarowy!");
        return;
    }

    odswiezWykres();
    analizujDane();
}
void GlowneOkno::odswiezWykres() {
    // Usuwanie starego wykresu i tworzenie nowego
    QChart* staryWykres = widokWykresu->chart();
    QChart* nowyWykres = new QChart();
    nowyWykres->setTitle("Pomiary jakości powietrza");
    nowyWykres->legend()->setVisible(true);
    nowyWykres->legend()->setAlignment(Qt::AlignBottom);

    // Filtrowanie danych według zakresu dat
    QDateTime dataOdCzas = QDateTime(dataOd->date(), QTime(0, 0));
    QDateTime dataDoCzas = QDateTime(dataDo->date(), QTime(23, 59, 59));

    QLineSeries* seria = new QLineSeries();
    QString nazwaSerii = "Pomiary";

    // Znajdź nazwę parametru
    for (const auto& czujnik : aktualneCzujniki) {
        if (czujnik.pobierzId() == idAktualnegoCzujnika) {
            nazwaSerii = QString::fromStdString(czujnik.pobierzNazweParametru());
            if (!czujnik.pobierzSymbolParametru().empty()) {
                nazwaSerii += " (" + QString::fromStdString(czujnik.pobierzSymbolParametru()) + ")";
            }
            break;
        }
    }

    seria->setName(nazwaSerii);

    // Dodaj dane do serii
    for (const auto& pomiar : aktualnePomiary) {
        QDateTime dataPomiaru = QDateTime::fromString(QString::fromStdString(pomiar.pobierzDate()), Qt::ISODate);

        if (dataPomiaru >= dataOdCzas && dataPomiaru <= dataDoCzas && pomiar.pobierzWartosc().has_value()) {
            seria->append(dataPomiaru.toMSecsSinceEpoch(), pomiar.pobierzWartosc().value());
        }
    }

    nowyWykres->addSeries(seria);

    // Ustawienie osi X
    QDateTimeAxis* osX = new QDateTimeAxis();
    osX->setFormat("dd.MM.yyyy hh:mm");
    osX->setTitleText("Data i czas");
    nowyWykres->addAxis(osX, Qt::AlignBottom);
    seria->attachAxis(osX);

    // Ustawienie osi Y
    QValueAxis* osY = new QValueAxis();
    osY->setTitleText("Wartość");
    nowyWykres->addAxis(osY, Qt::AlignLeft);
    seria->attachAxis(osY);

    // Zastąpienie starego wykresu nowym
    widokWykresu->setChart(nowyWykres);
    if (staryWykres) {
        delete staryWykres;
    }

    // Aktualizacja widoku
    widokWykresu->update();
}

void GlowneOkno::analizujDane() {
    // Filtrowanie danych według zakresu dat
    QDateTime dataOdCzas = QDateTime(dataOd->date(), QTime(0, 0));
    QDateTime dataDoCzas = QDateTime(dataDo->date(), QTime(23, 59, 59));
    
    // Zapisz przefiltrowane dane do analizy
    std::vector<double> wartosci;
    std::vector<std::pair<QDateTime, double>> paryDaty;
    
    for (const auto& pomiar : aktualnePomiary) {
        QDateTime dataPomiaru = QDateTime::fromString(QString::fromStdString(pomiar.pobierzDate()), Qt::ISODate);
        
        if (dataPomiaru >= dataOdCzas && dataPomiaru <= dataDoCzas && pomiar.pobierzWartosc().has_value()) {
            wartosci.push_back(pomiar.pobierzWartosc().value());
            paryDaty.emplace_back(dataPomiaru, pomiar.pobierzWartosc().value());
        }
    }
    
    // Sprawdź czy mamy wystarczające dane
    if (wartosci.empty()) {
        etykietaAnaliza->setText("Brak danych do analizy w wybranym zakresie dat.");
        return;
    }
    
    // Analiza danych
    double min = *std::min_element(wartosci.begin(), wartosci.end());
    double max = *std::max_element(wartosci.begin(), wartosci.end());
    double suma = std::accumulate(wartosci.begin(), wartosci.end(), 0.0);
    double srednia = suma / wartosci.size();
    
    // Znajdź daty min i max
    std::string dataMin, dataMax;
    for (const auto& para : paryDaty) {
        if (para.second == min) {
            dataMin = para.first.toString("dd.MM.yyyy hh:mm").toStdString();
        }
        if (para.second == max) {
            dataMax = para.first.toString("dd.MM.yyyy hh:mm").toStdString();
        }
    }
    
    // Oblicz trend (średni przyrost/spadek)
    double trend = 0.0;
    if (paryDaty.size() > 1) {
        double suma_zmian = 0.0;
        for (size_t i = 1; i < paryDaty.size(); i++) {
            suma_zmian += (paryDaty[i].second - paryDaty[i-1].second);
        }
        trend = suma_zmian / (paryDaty.size() - 1);
    }
    
    // Stwórz tekst analizy
    QString tekstAnalizy = QString(
        "Liczba pomiarów: %1\n"
        "Wartość minimalna: %2 (data: %3)\n"
        "Wartość maksymalna: %4 (data: %5)\n"
        "Wartość średnia: %6\n"
        "Trend: %7"
    ).arg(wartosci.size())
     .arg(min, 0, 'f', 2)
     .arg(QString::fromStdString(dataMin))
     .arg(max, 0, 'f', 2)
     .arg(QString::fromStdString(dataMax))
     .arg(srednia, 0, 'f', 2)
     .arg(trend > 0 ? "rosnący" : (trend < 0 ? "malejący" : "stały"));
    
    etykietaAnaliza->setText(tekstAnalizy);
}

void GlowneOkno::odswiezListeStacji() {
    // Pobierz świeże dane z API
    aktualizujStatus("Odświeżanie danych stacji...");
    
    try {
        std::thread([this]() {
            try {
                std::lock_guard<std::mutex> blokada(mutexDanych);
                auto noweStacje = klientAPI->pobierzStacje();
                
                QMetaObject::invokeMethod(this, [this, noweStacje]() {
                    stacje = noweStacje;
                    bazaDanych->zapiszStacje(stacje);
                    wypelnijListeStacji();
                    aktualizujStatus("Odświeżono listę stacji pomiarowych.");
                }, Qt::QueuedConnection);
            } catch (const std::exception& e) {
                QMetaObject::invokeMethod(this, [this, e]() {
                    pokazBlad("Błąd API", QString("Nie udało się odświeżyć stacji: ") + e.what());
                    aktualizujStatus("Błąd podczas odświeżania stacji.");
                }, Qt::QueuedConnection);
            }
        }).detach();
    } catch (const std::exception& e) {
        pokazBlad("Błąd wątku", QString("Nie można utworzyć wątku: ") + e.what());
    }
}

void GlowneOkno::pokazBlad(const QString& tytul, const QString& komunikat) {
    QMessageBox::critical(this, tytul, komunikat);
}

void GlowneOkno::pokazInfo(const QString& tytul, const QString& komunikat) {
    QMessageBox::information(this, tytul, komunikat);
}

void GlowneOkno::aktualizujStatus(const QString& komunikat) {
    etykietaStatus->setText(komunikat);
}
