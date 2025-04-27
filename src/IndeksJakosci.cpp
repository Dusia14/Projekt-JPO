// src/IndeksJakosci.cpp
#include "IndeksJakosci.h"
#include <stdexcept>

// Inicjalizacja statycznych map
const std::map<int, std::string> IndeksJakosci::opisyIndeksow = {
    {0, "Bardzo dobry"},
    {1, "Dobry"},
    {2, "Umiarkowany"},
    {3, "Dostateczny"},
    {4, "Zły"},
    {5, "Bardzo zły"}
};

const std::map<int, std::string> IndeksJakosci::zaleceniaZdrowotne = {
    {0, "Jakość powietrza jest bardzo dobra, warunki idealne do aktywności na zewnątrz."},
    {1, "Jakość powietrza jest zadowalająca, można przebywać na zewnątrz."},
    {2, "Jakość powietrza jest akceptowalna, warunki odpowiednie do aktywności na zewnątrz z wyjątkiem osób wrażliwych."},
    {3, "Osoby wrażliwe powinny ograniczyć przebywanie na zewnątrz, pozostałe osoby mogą odczuwać negatywne skutki zdrowotne."},
    {4, "Osoby chore powinny bezwzględnie ograniczyć przebywanie na zewnątrz. Pozostałe osoby powinny ograniczyć aktywności na zewnątrz."},
    {5, "Wskazane ograniczenie do minimum wszelkich aktywności na zewnątrz. Osoby chore powinny bezwzględnie pozostać w pomieszczeniach."}
};

const std::map<int, std::string> IndeksJakosci::koloryIndeksow = {
    {0, "#00FF00"}, // Zielony
    {1, "#99FF00"}, // Jasnozielony
    {2, "#FFFF00"}, // Żółty
    {3, "#FFBB00"}, // Pomarańczowy
    {4, "#FF0000"}, // Czerwony
    {5, "#990000"}  // Ciemnoczerwony
};

IndeksJakosci::IndeksJakosci() : poziom(0) {}

IndeksJakosci::IndeksJakosci(int poziomIndeksu) {
    ustawPoziom(poziomIndeksu);
}

std::string IndeksJakosci::pobierzOpisIndeksu() const {
    auto it = opisyIndeksow.find(poziom);
    if (it != opisyIndeksow.end()) {
        return it->second;
    }
    return "Nieznany";
}

std::string IndeksJakosci::pobierzZaleceniaZdrowotne() const {
    auto it = zaleceniaZdrowotne.find(poziom);
    if (it != zaleceniaZdrowotne.end()) {
        return it->second;
    }
    return "Brak zaleceń";
}

std::string IndeksJakosci::pobierzKolor() const {
    auto it = koloryIndeksow.find(poziom);
    if (it != koloryIndeksow.end()) {
        return it->second;
    }
    return "#CCCCCC"; // Domyślny szary
}

void IndeksJakosci::ustawPoziom(int nowyPoziom) {
    if (nowyPoziom < 0 || nowyPoziom > 5) {
        throw std::invalid_argument("Poziom indeksu musi być w zakresie 0-5");
    }
    poziom = nowyPoziom;
}