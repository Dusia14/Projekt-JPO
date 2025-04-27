// include/BazaDanych.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include "StacjaPomiarowa.h"
#include "CzujnikPomiarowy.h"
#include "Pomiar.h"

class BazaDanych {
public:
    BazaDanych(const std::string& sciezka = "dane/");
    
    // Zapis danych stacji
    bool zapiszStacje(const std::vector<StacjaPomiarowa>& stacje);
    
    // Zapis danych czujników
    bool zapiszCzujniki(int idStacji, const std::vector<CzujnikPomiarowy>& czujniki);
    
    // Zapis danych pomiarowych
    bool zapiszPomiary(int idCzujnika, const std::vector<Pomiar>& pomiary);
    
    // Odczyt danych stacji
    std::optional<std::vector<StacjaPomiarowa>> wczytajStacje();
    
    // Odczyt danych czujników
    std::optional<std::vector<CzujnikPomiarowy>> wczytajCzujniki(int idStacji);
    
    // Odczyt danych pomiarowych
    std::optional<std::vector<Pomiar>> wczytajPomiary(int idCzujnika);
    
    // Sprawdzenie czy dane są dostępne lokalnie
    bool czyDaneIstnieja(const std::string& typ, int id = 0);

private:
    std::string sciezka;
    
    // Pomocnicze metody do operacji na plikach
    bool zapiszJsonDoPliku(const std::string& sciezkaPliku, const std::string& trescJson);
    std::optional<std::string> wczytajJsonZPliku(const std::string& sciezkaPliku);
    
    // Generowanie ścieżek do plików
    std::string pobierzSciezkeStacji();
    std::string pobierzSciezkeCzujnikow(int idStacji);
    std::string pobierzSciezkePomiarow(int idCzujnika);
};