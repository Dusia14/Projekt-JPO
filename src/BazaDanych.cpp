// src/BazaDanych.cpp
#include "BazaDanych.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <optional>

using json = nlohmann::json;
namespace fs = std::filesystem;

BazaDanych::BazaDanych(const std::string& sciezka) : sciezka(sciezka) {
    // Upewnij się, że katalog istnieje
    fs::create_directories(sciezka);
}

bool BazaDanych::zapiszJsonDoPliku(const std::string& sciezkaPliku, const std::string& trescJson) {
    try {
        // Upewnij się, że katalog istnieje
        fs::path sciezka(sciezkaPliku);
        fs::create_directories(sciezka.parent_path());
        
        std::ofstream plik(sciezkaPliku);
        if (plik.is_open()) {
            plik << trescJson;
            plik.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd zapisu pliku " << sciezkaPliku << ": " << e.what() << std::endl;
    }
    
    return false;
}

std::optional<std::string> BazaDanych::wczytajJsonZPliku(const std::string& sciezkaPliku) {
    try {
        std::ifstream plik(sciezkaPliku);
        if (plik.is_open()) {
            std::string tresc((std::istreambuf_iterator<char>(plik)), std::istreambuf_iterator<char>());
            plik.close();
            return tresc;
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd odczytu pliku " << sciezkaPliku << ": " << e.what() << std::endl;
    }
    
    return std::nullopt;
}

std::string BazaDanych::pobierzSciezkeStacji() {
    return sciezka + "stacje.json";
}

std::string BazaDanych::pobierzSciezkeCzujnikow(int idStacji) {
    return sciezka + "stacja_" + std::to_string(idStacji) + "_czujniki.json";
}

std::string BazaDanych::pobierzSciezkePomiarow(int idCzujnika) {
    return sciezka + "czujnik_" + std::to_string(idCzujnika) + "_pomiary.json";
}

bool BazaDanych::zapiszStacje(const std::vector<StacjaPomiarowa>& stacje) {
    json stacjeJson = json::array();
    
    for (const auto& stacja : stacje) {
        json stacjaJson;
        stacjaJson["id"] = stacja.pobierzId();
        stacjaJson["nazwaStacji"] = stacja.pobierzNazwe();
        stacjaJson["szerokoscGeo"] = stacja.pobierzSzerokosc();
        stacjaJson["dlugoscGeo"] = stacja.pobierzDlugosc();
        stacjaJson["miasto"] = stacja.pobierzMiasto();
        stacjaJson["ulica"] = stacja.pobierzAdres();
        stacjaJson["wojewodztwo"] = stacja.pobierzWojewodztwo();
        
        stacjeJson.push_back(stacjaJson);
    }
    
    return zapiszJsonDoPliku(pobierzSciezkeStacji(), stacjeJson.dump(4));
}

bool BazaDanych::zapiszCzujniki(int idStacji, const std::vector<CzujnikPomiarowy>& czujniki) {
    json czujnikiJson = json::array();
    
    for (const auto& czujnik : czujniki) {
        json czujnikJson;
        czujnikJson["id"] = czujnik.pobierzId();
        czujnikJson["idStacji"] = czujnik.pobierzIdStacji();
        czujnikJson["nazwaParametru"] = czujnik.pobierzNazweParametru();
        czujnikJson["symbolParametru"] = czujnik.pobierzSymbolParametru();
        czujnikJson["kodParametru"] = czujnik.pobierzKodParametru();
        czujnikJson["idParametru"] = czujnik.pobierzIdParametru();
        
        czujnikiJson.push_back(czujnikJson);
    }
    
    return zapiszJsonDoPliku(pobierzSciezkeCzujnikow(idStacji), czujnikiJson.dump(4));
}

bool BazaDanych::zapiszPomiary(int idCzujnika, const std::vector<Pomiar>& pomiary) {
    json pomiaryJson = json::array();
    
    for (const auto& pomiar : pomiary) {
        json pomiarJson;
        pomiarJson["data"] = pomiar.pobierzDate();
        
        if (pomiar.pobierzWartosc().has_value()) {
            pomiarJson["wartosc"] = pomiar.pobierzWartosc().value();
        } else {
            pomiarJson["wartosc"] = nullptr;
        }
        
        pomiaryJson.push_back(pomiarJson);
    }
    
    return zapiszJsonDoPliku(pobierzSciezkePomiarow(idCzujnika), pomiaryJson.dump(4));
}

// src/BazaDanych.cpp
std::optional<std::vector<StacjaPomiarowa>> BazaDanych::wczytajStacje() {
    auto zawartoscJson = wczytajJsonZPliku(pobierzSciezkeStacji());
    if (!zawartoscJson.has_value()) {
        return std::nullopt;
    }

    try {
        json stacjeJson = json::parse(zawartoscJson.value());
        std::vector<StacjaPomiarowa> stacje;

        for (const auto& stacjaJson : stacjeJson) {
            // Dodajmy try-catch bezpośrednio tu dla lepszej diagnozy
            try {
                // Konwertujmy jawnie, żeby uniknąć błędów typów
                int id = stacjaJson.contains("id") ? stacjaJson["id"].get<int>() : 0;
                std::string nazwa = stacjaJson.contains("nazwaStacji") ? stacjaJson["nazwaStacji"].get<std::string>() : "";
                double szerokosc = stacjaJson.contains("szerokoscGeo") ? stacjaJson["szerokoscGeo"].get<double>() : 0.0;
                double dlugosc = stacjaJson.contains("dlugoscGeo") ? stacjaJson["dlugoscGeo"].get<double>() : 0.0;

                StacjaPomiarowa stacja(id, nazwa, szerokosc, dlugosc);

                if (stacjaJson.contains("miasto")) {
                    stacja.ustawMiasto(stacjaJson["miasto"].get<std::string>());
                }

                if (stacjaJson.contains("ulica")) {
                    stacja.ustawAdres(stacjaJson["ulica"].get<std::string>());
                }

                if (stacjaJson.contains("wojewodztwo")) {
                    stacja.ustawWojewodztwo(stacjaJson["wojewodztwo"].get<std::string>());
                }

                stacje.push_back(stacja);
            } catch (const std::exception& e) {
                std::cerr << "Błąd parsowania pojedynczej stacji: " << e.what() << std::endl;
                // Kontynuujemy pętlę, pomijając problematyczną stację
                continue;
            }
        }

        return stacje;
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania stacji z pliku: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<std::vector<CzujnikPomiarowy>> BazaDanych::wczytajCzujniki(int idStacji) {
    auto zawartoscJson = wczytajJsonZPliku(pobierzSciezkeCzujnikow(idStacji));
    if (!zawartoscJson.has_value()) {
        return std::nullopt;
    }
    
    try {
        json czujnikiJson = json::parse(zawartoscJson.value());
        std::vector<CzujnikPomiarowy> czujniki;
        
        for (const auto& czujnikJson : czujnikiJson) {
            int id = czujnikJson["id"];
            int idSt = czujnikJson["idStacji"];
            std::string nazwaPar = czujnikJson["nazwaParametru"];
            std::string symbolPar = czujnikJson["symbolParametru"];
            std::string kodPar = czujnikJson["kodParametru"];
            int idPar = czujnikJson["idParametru"];
            
            czujniki.emplace_back(id, idSt, nazwaPar, symbolPar, kodPar, idPar);
        }
        
        return czujniki;
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania czujników z pliku: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<std::vector<Pomiar>> BazaDanych::wczytajPomiary(int idCzujnika) {
    auto zawartoscJson = wczytajJsonZPliku(pobierzSciezkePomiarow(idCzujnika));
    if (!zawartoscJson.has_value()) {
        return std::nullopt;
    }
    
    try {
        json pomiaryJson = json::parse(zawartoscJson.value());
        std::vector<Pomiar> pomiary;
        
        for (const auto& pomiarJson : pomiaryJson) {
            std::string data = pomiarJson["data"];
            std::optional<double> wartosc;
            
            if (!pomiarJson["wartosc"].is_null()) {
                if (pomiarJson["wartosc"].is_number()) {
                    wartosc = pomiarJson["wartosc"].get<double>();
                } else if (pomiarJson["wartosc"].is_string()) {
                    try {
                        wartosc = std::stod(pomiarJson["wartosc"].get<std::string>());
                    } catch (...) {
                        std::cerr << "Nie można skonwertować wartosci do double!" << std::endl;
                    }
                } else {
                    std::cerr << "Nieznany typ pola wartosc!" << std::endl;
                }
            }



            
            pomiary.emplace_back(data, wartosc);
        }
        
        return pomiary;
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania pomiarów z pliku: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool BazaDanych::czyDaneIstnieja(const std::string& typ, int id) {
    if (typ == "stacje") {
        return fs::exists(pobierzSciezkeStacji());
    } else if (typ == "czujniki") {
        return fs::exists(pobierzSciezkeCzujnikow(id));
    } else if (typ == "pomiary") {
        return fs::exists(pobierzSciezkePomiarow(id));
    }
    
    return false;
}
