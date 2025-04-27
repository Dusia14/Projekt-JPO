// include/IndeksJakosci.h
#pragma once
#include <string>
#include <map>
#include <vector>

/**
 * @brief Klasa reprezentująca indeks jakości powietrza (CAQI).
 * 
 * Klasa odpowiada za przechowywanie i interpretację poziomów indeksu 
 * jakości powietrza zgodnie ze skalą stosowaną przez GIOŚ.
 */
class IndeksJakosci {
public:
    /**
     * @brief Konstruktor domyślny.
     */
    IndeksJakosci();
    
    /**
     * @brief Konstruktor z poziomem indeksu.
     * @param poziomIndeksu Poziom indeksu jakości (0-5).
     */
    IndeksJakosci(int poziomIndeksu);
    
    /**
     * @brief Pobiera poziom indeksu.
     * @return Poziom indeksu jakości (0-5).
     */
    int pobierzPoziom() const { return poziom; }
    
    /**
     * @brief Pobiera nazwę poziomu indeksu.
     * @return Tekstowy opis poziomu indeksu.
     */
    std::string pobierzOpisIndeksu() const;
    
    /**
     * @brief Pobiera zalecenia zdrowotne dla aktualnego poziomu.
     * @return Zalecenia zdrowotne jako tekst.
     */
    std::string pobierzZaleceniaZdrowotne() const;
    
    /**
     * @brief Pobiera kolor odpowiadający danemu poziomowi.
     * @return Kolor w formacie HTML (#RRGGBB).
     */
    std::string pobierzKolor() const;
    
    /**
     * @brief Ustawia poziom indeksu.
     * @param nowyPoziom Nowy poziom indeksu (0-5).
     */
    void ustawPoziom(int nowyPoziom);
    
private:
    int poziom; ///< Poziom indeksu jakości (0-5)
    
    // Statyczne mapowanie poziomów na opisy, zalecenia i kolory
    static const std::map<int, std::string> opisyIndeksow;
    static const std::map<int, std::string> zaleceniaZdrowotne;
    static const std::map<int, std::string> koloryIndeksow;
};