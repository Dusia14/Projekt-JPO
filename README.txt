# Monitor Jakości Powietrza
Projekt zaliczeniowy z przedmiotu Języki Programowania Obiektowego

## Opis projektu
Aplikacja służy do monitorowania jakości powietrza w Polsce. Umożliwia pobieranie danych 
pomiarowych ze stacji Głównego Inspektoratu Ochrony Środowiska (GIOŚ) poprzez ich publiczne API. 
Aplikacja pozwala na:
- Przeglądanie stacji pomiarowych w Polsce
- Filtrowanie stacji według miast
- Pobieranie danych z czujników (np. PM10, PM2.5)
- Wyświetlanie danych w postaci wykresów
- Analizowanie danych (wartości minimalne, maksymalne, średnie, trendy)
- Zapisywanie danych lokalnie do formatu JSON
- Pracę offline dzięki danym zapisanym lokalnie
- Wielowątkowe pobieranie danych

## Funkcjonalności szczegółowe
- Obsługa błędów i wyjątków podczas pobierania danych z API
- Automatyczne przełączanie między danymi online i offline
- Analiza pomiarów: wartości min/max z datami wystąpienia, wartość średnia, trend
- Wizualizacja danych na wykresie z możliwością wyboru zakresu dat
- Wielowątkowe pobieranie danych dla lepszej responsywności interfejsu

## Wymagania
Do poprawnego działania aplikacji potrzebne są:
- Kompilator C++ z obsługą C++17
- Qt 5.15 lub nowszy
- CURL dla obsługi zapytań HTTP
- nlohmann/json dla obsługi formatu JSON

Szczegółowe zależności znajdują się w pliku requirements.txt.

## Kompilacja projektu
1. Upewnij się, że masz zainstalowane wszystkie wymagane zależności.
2. Utwórz katalog build: `mkdir build && cd build`
3. Uruchom QtCreator
4. Skompiluj projekt

## Uruchamianie
Po kompilacji, uruchom aplikację:
```
./MonitorPowietrza
```

## Uruchamianie testów
Aby uruchomić testy jednostkowe:
```
Otworzyc projekt tests w Qt Creator 
```

## Struktura projektu
- `include/` - Pliki nagłówkowe (.h)
- `src/` - Pliki źródłowe (.cpp)
- `tests/` - Testy jednostkowe
- `dane/` - Katalog na dane lokalne (tworzony automatycznie)

## Dokumentacja
Projekt zawiera pełną dokumentację w stylu Doxygen. Aby wygenerować dokumentację:
```
doxygen Doxyfile
```
Dokumentacja zostanie wygenerowana w katalogu `doc/`.


## Autor
Klaudia Szczepankiewicz
Nr indeksu: 160456
Data: 21 Kwiecień 2025
