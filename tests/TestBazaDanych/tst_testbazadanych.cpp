#include <QtTest>
#include <QDir>
#include <filesystem>
#include "BazaDanych.h"
#include "StacjaPomiarowa.h"
#include "CzujnikPomiarowy.h"
#include "Pomiar.h"

class TestBazaDanych : public QObject
{
    Q_OBJECT

private:
    const std::string TEST_DIR = "test_dane/";
    BazaDanych* bazaDanych;
    
    // Pomocnicze metody do tworzenia przykładowych danych
    std::vector<StacjaPomiarowa> utworzPrzykladoweStacje();
    std::vector<CzujnikPomiarowy> utworzPrzykladoweCzujniki(int idStacji);
    std::vector<Pomiar> utworzPrzykladowePomiary();

public:
    TestBazaDanych();
    ~TestBazaDanych();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_zapiszWczytajStacje();
    void test_zapiszWczytajCzujniki();
    void test_zapiszWczytajPomiary();
    void test_czyDaneIstnieja();
};

TestBazaDanych::TestBazaDanych()
{
}

TestBazaDanych::~TestBazaDanych()
{
}

void TestBazaDanych::initTestCase()
{
    qDebug() << "Inicjalizacja testów dla klasy BazaDanych";
    
    // Utworzenie katalogu testowego
    QDir dir;
    dir.mkpath(QString::fromStdString(TEST_DIR));
    
    // Utworzenie instancji BazaDanych z testowym katalogiem
    bazaDanych = new BazaDanych(TEST_DIR);
}

void TestBazaDanych::cleanupTestCase()
{
    qDebug() << "Sprzątanie po testach dla klasy BazaDanych";
    
    // Usunięcie instancji BazaDanych
    delete bazaDanych;
    
    // Usunięcie katalogu testowego i jego zawartości
    QDir dir(QString::fromStdString(TEST_DIR));
    dir.removeRecursively();
}

std::vector<StacjaPomiarowa> TestBazaDanych::utworzPrzykladoweStacje()
{
    std::vector<StacjaPomiarowa> stacje;
    
    StacjaPomiarowa s1(1, "Stacja Testowa 1", 52.2297, 21.0122);
    s1.ustawMiasto("Warszawa");
    s1.ustawAdres("ul. Testowa 1");
    s1.ustawWojewodztwo("mazowieckie");
    
    StacjaPomiarowa s2(2, "Stacja Testowa 2", 50.0647, 19.9450);
    s2.ustawMiasto("Kraków");
    s2.ustawAdres("ul. Testowa 2");
    s2.ustawWojewodztwo("małopolskie");
    
    stacje.push_back(s1);
    stacje.push_back(s2);
    
    return stacje;
}

std::vector<CzujnikPomiarowy> TestBazaDanych::utworzPrzykladoweCzujniki(int idStacji)
{
    std::vector<CzujnikPomiarowy> czujniki;
    
    CzujnikPomiarowy c1(101, idStacji, "Pył zawieszony PM10", "PM10", "PM10", 1);
    CzujnikPomiarowy c2(102, idStacji, "Pył zawieszony PM2.5", "PM2.5", "PM2.5", 2);
    
    czujniki.push_back(c1);
    czujniki.push_back(c2);
    
    return czujniki;
}

std::vector<Pomiar> TestBazaDanych::utworzPrzykladowePomiary()
{
    std::vector<Pomiar> pomiary;
    
    pomiary.emplace_back("2023-04-01T12:00:00", std::optional<double>(25.5));
    pomiary.emplace_back("2023-04-01T13:00:00", std::optional<double>(30.2));
    pomiary.emplace_back("2023-04-01T14:00:00", std::optional<double>());  // Wartość null
    
    return pomiary;
}

void TestBazaDanych::test_zapiszWczytajStacje()
{
    // Tworzenie przykładowych stacji
    auto stacje = utworzPrzykladoweStacje();
    
    // Zapisanie stacji
    bool wynikZapisu = bazaDanych->zapiszStacje(stacje);
    QVERIFY(wynikZapisu);
    
    // Wczytanie stacji
    auto wczytaneStacje = bazaDanych->wczytajStacje();
    QVERIFY(wczytaneStacje.has_value());
    QCOMPARE(wczytaneStacje.value().size(), stacje.size());
    
    // Sprawdzenie czy wczytane dane są zgodne z zapisanymi
    for (size_t i = 0; i < stacje.size(); i++) {
        QCOMPARE(wczytaneStacje.value()[i].pobierzId(), stacje[i].pobierzId());
        QCOMPARE(wczytaneStacje.value()[i].pobierzNazwe(), stacje[i].pobierzNazwe());
        QCOMPARE(wczytaneStacje.value()[i].pobierzMiasto(), stacje[i].pobierzMiasto());
        QCOMPARE(wczytaneStacje.value()[i].pobierzWojewodztwo(), stacje[i].pobierzWojewodztwo());
    }
}

void TestBazaDanych::test_zapiszWczytajCzujniki()
{
    int idStacji = 1;
    auto czujniki = utworzPrzykladoweCzujniki(idStacji);
    
    // Zapisanie czujników
    bool wynikZapisu = bazaDanych->zapiszCzujniki(idStacji, czujniki);
    QVERIFY(wynikZapisu);
    
    // Wczytanie czujników
    auto wczytaneCzujniki = bazaDanych->wczytajCzujniki(idStacji);
    QVERIFY(wczytaneCzujniki.has_value());
    QCOMPARE(wczytaneCzujniki.value().size(), czujniki.size());
    
    // Sprawdzenie czy wczytane dane są zgodne z zapisanymi
    for (size_t i = 0; i < czujniki.size(); i++) {
        QCOMPARE(wczytaneCzujniki.value()[i].pobierzId(), czujniki[i].pobierzId());
        QCOMPARE(wczytaneCzujniki.value()[i].pobierzIdStacji(), czujniki[i].pobierzIdStacji());
        QCOMPARE(wczytaneCzujniki.value()[i].pobierzNazweParametru(), czujniki[i].pobierzNazweParametru());
        QCOMPARE(wczytaneCzujniki.value()[i].pobierzSymbolParametru(), czujniki[i].pobierzSymbolParametru());
    }
}

void TestBazaDanych::test_zapiszWczytajPomiary()
{
    int idCzujnika = 101;
    auto pomiary = utworzPrzykladowePomiary();
    
    // Zapisanie pomiarów
    bool wynikZapisu = bazaDanych->zapiszPomiary(idCzujnika, pomiary);
    QVERIFY(wynikZapisu);
    
    // Wczytanie pomiarów
    auto wczytanePomiary = bazaDanych->wczytajPomiary(idCzujnika);
    QVERIFY(wczytanePomiary.has_value());
    QCOMPARE(wczytanePomiary.value().size(), pomiary.size());
    
    // Sprawdzenie czy wczytane dane są zgodne z zapisanymi
    for (size_t i = 0; i < pomiary.size(); i++) {
        QCOMPARE(wczytanePomiary.value()[i].pobierzDate(), pomiary[i].pobierzDate());
        
        // Sprawdzamy czy oba optional<double> są zgodne (puste lub mają takie same wartości)
        if (pomiary[i].pobierzWartosc().has_value()) {
            QVERIFY(wczytanePomiary.value()[i].pobierzWartosc().has_value());
            QCOMPARE(wczytanePomiary.value()[i].pobierzWartosc().value(), pomiary[i].pobierzWartosc().value());
        } else {
            QVERIFY(!wczytanePomiary.value()[i].pobierzWartosc().has_value());
        }
    }
}

void TestBazaDanych::test_czyDaneIstnieja()
{
    // Zakładamy, że mamy już zapisane dane z poprzednich testów
    QVERIFY(bazaDanych->czyDaneIstnieja("stacje"));
    QVERIFY(bazaDanych->czyDaneIstnieja("czujniki", 1));
    QVERIFY(bazaDanych->czyDaneIstnieja("pomiary", 101));
    
    // Sprawdzamy nieistniejące dane
    QVERIFY(!bazaDanych->czyDaneIstnieja("czujniki", 999));
    QVERIFY(!bazaDanych->czyDaneIstnieja("pomiary", 999));
    QVERIFY(!bazaDanych->czyDaneIstnieja("nieistniejacy_typ"));
}

QTEST_APPLESS_MAIN(TestBazaDanych)

#include "tst_testbazadanych.moc"