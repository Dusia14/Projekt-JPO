#include <QtTest>
#include "StacjaPomiarowa.h"
#include <cmath>

class TestStacjaPomiarowa : public QObject
{
    Q_OBJECT

public:
    TestStacjaPomiarowa();
    ~TestStacjaPomiarowa();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_konstruktor();
    void test_ustawGettery();
    void test_kopiowanie();
    void test_obliczOdleglosc();
};

TestStacjaPomiarowa::TestStacjaPomiarowa()
{
}

TestStacjaPomiarowa::~TestStacjaPomiarowa()
{
}

void TestStacjaPomiarowa::initTestCase()
{
    qDebug() << "Rozpoczęcie testów dla klasy StacjaPomiarowa";
}

void TestStacjaPomiarowa::cleanupTestCase()
{
    qDebug() << "Zakończenie testów dla klasy StacjaPomiarowa";
}

void TestStacjaPomiarowa::test_konstruktor()
{
    // Test konstruktora domyślnego
    StacjaPomiarowa stacja1;
    QCOMPARE(stacja1.pobierzId(), 0);
    QCOMPARE(stacja1.pobierzNazwe(), std::string(""));
    QCOMPARE(stacja1.pobierzSzerokosc(), 0.0);
    QCOMPARE(stacja1.pobierzDlugosc(), 0.0);
    
    // Test konstruktora parametrycznego
    StacjaPomiarowa stacja2(123, "Testowa Stacja", 52.2297, 21.0122);
    QCOMPARE(stacja2.pobierzId(), 123);
    QCOMPARE(stacja2.pobierzNazwe(), std::string("Testowa Stacja"));
    QCOMPARE(stacja2.pobierzSzerokosc(), 52.2297);
    QCOMPARE(stacja2.pobierzDlugosc(), 21.0122);
    QCOMPARE(stacja2.pobierzMiasto(), std::string(""));
    QCOMPARE(stacja2.pobierzAdres(), std::string(""));
    QCOMPARE(stacja2.pobierzWojewodztwo(), std::string(""));
}

void TestStacjaPomiarowa::test_ustawGettery()
{
    StacjaPomiarowa stacja;
    
    // Ustawienie i odczyt ID
    stacja.ustawId(456);
    QCOMPARE(stacja.pobierzId(), 456);
    
    // Ustawienie i odczyt nazwy
    stacja.ustawNazwe("Nowa Nazwa");
    QCOMPARE(stacja.pobierzNazwe(), std::string("Nowa Nazwa"));
    
    // Ustawienie i odczyt współrzędnych
    stacja.ustawSzerokosc(54.3889);
    stacja.ustawDlugosc(18.6031);
    QCOMPARE(stacja.pobierzSzerokosc(), 54.3889);
    QCOMPARE(stacja.pobierzDlugosc(), 18.6031);
    
    // Ustawienie i odczyt miasta
    stacja.ustawMiasto("Gdańsk");
    QCOMPARE(stacja.pobierzMiasto(), std::string("Gdańsk"));
    
    // Ustawienie i odczyt adresu
    stacja.ustawAdres("ul. Przykładowa 123");
    QCOMPARE(stacja.pobierzAdres(), std::string("ul. Przykładowa 123"));
    
    // Ustawienie i odczyt województwa
    stacja.ustawWojewodztwo("pomorskie");
    QCOMPARE(stacja.pobierzWojewodztwo(), std::string("pomorskie"));
}

void TestStacjaPomiarowa::test_kopiowanie()
{
    // Tworzenie i inicjalizacja stacji źródłowej
    StacjaPomiarowa stacjaZrodlowa(789, "Stacja Źródłowa", 50.0647, 19.9450);
    stacjaZrodlowa.ustawMiasto("Kraków");
    stacjaZrodlowa.ustawAdres("ul. Testowa 456");
    stacjaZrodlowa.ustawWojewodztwo("małopolskie");
    
    // Kopiowanie przez konstruktor kopiujący (domyślny)
    StacjaPomiarowa stacjaKopia1 = stacjaZrodlowa;
    
    // Sprawdzanie czy kopiowanie jest poprawne
    QCOMPARE(stacjaKopia1.pobierzId(), stacjaZrodlowa.pobierzId());
    QCOMPARE(stacjaKopia1.pobierzNazwe(), stacjaZrodlowa.pobierzNazwe());
    QCOMPARE(stacjaKopia1.pobierzSzerokosc(), stacjaZrodlowa.pobierzSzerokosc());
    QCOMPARE(stacjaKopia1.pobierzDlugosc(), stacjaZrodlowa.pobierzDlugosc());
    QCOMPARE(stacjaKopia1.pobierzMiasto(), stacjaZrodlowa.pobierzMiasto());
    QCOMPARE(stacjaKopia1.pobierzAdres(), stacjaZrodlowa.pobierzAdres());
    QCOMPARE(stacjaKopia1.pobierzWojewodztwo(), stacjaZrodlowa.pobierzWojewodztwo());
    
    // Kopiowanie przez operator przypisania (domyślny)
    StacjaPomiarowa stacjaKopia2;
    stacjaKopia2 = stacjaZrodlowa;
    
    // Sprawdzanie czy kopiowanie jest poprawne
    QCOMPARE(stacjaKopia2.pobierzId(), stacjaZrodlowa.pobierzId());
    QCOMPARE(stacjaKopia2.pobierzNazwe(), stacjaZrodlowa.pobierzNazwe());
    QCOMPARE(stacjaKopia2.pobierzSzerokosc(), stacjaZrodlowa.pobierzSzerokosc());
    QCOMPARE(stacjaKopia2.pobierzDlugosc(), stacjaZrodlowa.pobierzDlugosc());
    QCOMPARE(stacjaKopia2.pobierzMiasto(), stacjaZrodlowa.pobierzMiasto());
    QCOMPARE(stacjaKopia2.pobierzAdres(), stacjaZrodlowa.pobierzAdres());
    QCOMPARE(stacjaKopia2.pobierzWojewodztwo(), stacjaZrodlowa.pobierzWojewodztwo());
}

// Helper do obliczania odległości między punktami geograficznymi
double obliczOdlegloscGeograficzna(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // promień Ziemi w km
    const double toRad = M_PI / 180.0;
    
    double dLat = (lat2 - lat1) * toRad;
    double dLon = (lon2 - lon1) * toRad;
    
    double a = sin(dLat/2) * sin(dLat/2) +
              cos(lat1 * toRad) * cos(lat2 * toRad) * 
              sin(dLon/2) * sin(dLon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

void TestStacjaPomiarowa::test_obliczOdleglosc()
{
    // Ustalenie dwóch stacji w różnych lokalizacjach
    StacjaPomiarowa stacja1(1, "Warszawa", 52.2297, 21.0122);
    StacjaPomiarowa stacja2(2, "Kraków", 50.0647, 19.9450);
    
    // Obliczenie oczekiwanej odległości za pomocą wzoru haversine
    double oczekiwanaOdleglosc = obliczOdlegloscGeograficzna(
        stacja1.pobierzSzerokosc(), stacja1.pobierzDlugosc(),
        stacja2.pobierzSzerokosc(), stacja2.pobierzDlugosc()
    );
    
    // Ponieważ w oryginalnej klasie StacjaPomiarowa nie ma metody do obliczania odległości,
    // tutaj tylko demonstracyjnie wyświetlamy obliczoną odległość
    qDebug() << "Odległość między stacjami w Warszawie i Krakowie:" << oczekiwanaOdleglosc << "km";
    
    // Test czy odległość jest w przybliżonym zakresie (około 250-260 km)
    QVERIFY(oczekiwanaOdleglosc > 250.0);
    QVERIFY(oczekiwanaOdleglosc < 260.0);
}

QTEST_APPLESS_MAIN(TestStacjaPomiarowa)

#include "tst_teststacjapomiarowa.moc"