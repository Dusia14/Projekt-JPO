// include/KlientAPI.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "StacjaPomiarowa.h"
#include "CzujnikPomiarowy.h"
#include "Pomiar.h"

/**
 * @brief Klasa klienta API GIOŚ
 * 
 * Klasa odpowiedzialna za komunikację z REST API Głównego Inspektoratu Ochrony Środowiska.
 * Umożliwia pobieranie danych o stacjach pomiarowych, czujnikach, pomiarach i indeksach jakości powietrza.
 */
class KlientAPI {
public:
    /**
     * @brief Konstruktor
     * 
     * Inicjalizuje bibliotekę cURL i przygotowuje klienta do wykonywania zapytań
     */
    KlientAPI();
    
    /**
     * @brief Destruktor
     * 
     * Zwalnia zasoby biblioteki cURL
     */
    ~KlientAPI();

    /**
     * @brief Pobiera listę wszystkich stacji pomiarowych
     * 
     * Wykonuje zapytanie HTTP GET do API GIOŚ w celu pobrania listy wszystkich dostępnych 
     * stacji pomiarowych i ich szczegółów
     * 
     * @return std::vector<StacjaPomiarowa> Wektor obiektów reprezentujących stacje pomiarowe
     */
    std::vector<StacjaPomiarowa> pobierzStacje();

    /**
     * @brief Pobiera listę czujników dla danej stacji
     * 
     * Wykonuje zapytanie HTTP GET do API GIOŚ w celu pobrania listy czujników 
     * dostępnych w określonej stacji pomiarowej
     * 
     * @param idStacji Identyfikator stacji pomiarowej
     * @return std::vector<CzujnikPomiarowy> Wektor obiektów reprezentujących czujniki
     */
    std::vector<CzujnikPomiarowy> pobierzCzujniki(int idStacji);

    /**
     * @brief Pobiera pomiary dla określonego czujnika
     * 
     * Wykonuje zapytanie HTTP GET do API GIOŚ w celu pobrania danych pomiarowych 
     * z określonego czujnika pomiarowego
     * 
     * @param idCzujnika Identyfikator czujnika pomiarowego
     * @return std::vector<Pomiar> Wektor obiektów reprezentujących pomiary
     */
    std::vector<Pomiar> pobierzPomiary(int idCzujnika);

    /**
     * @brief Pobiera indeks jakości powietrza dla danej stacji
     * 
     * Wykonuje zapytanie HTTP GET do API GIOŚ w celu pobrania indeksu jakości 
     * powietrza dla określonej stacji pomiarowej
     * 
     * @param idStacji Identyfikator stacji pomiarowej
     * @return std::optional<int> Opcjonalna wartość indeksu jakości (0-5), 
     *         std::nullopt w przypadku braku danych
     */
    std::optional<int> pobierzIndeksJakosci(int idStacji);

private:
    /**
     * @brief Wykonuje zapytanie HTTP do podanego URL
     * 
     * Metoda pomocnicza, która wykonuje zapytanie HTTP GET do określonego adresu URL
     * i zwraca odpowiedź serwera jako string
     * 
     * @param url Adres URL, do którego ma zostać wykonane zapytanie
     * @return std::string Odpowiedź serwera jako ciąg znaków
     */
    std::string wykonajZapytanieHTTP(const std::string& url);

    /**
     * @brief Bazowy adres URL API GIOŚ
     */
    const std::string bazowy_url = "https://api.gios.gov.pl/pjp-api/rest";
};