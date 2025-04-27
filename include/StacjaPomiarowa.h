// include/StacjaPomiarowa.h
#pragma once
#include <string>
#include <vector>

class StacjaPomiarowa {
public:
    StacjaPomiarowa() = default;
    StacjaPomiarowa(int id, const std::string& nazwa, double szerokosc, double dlugosc);

    int pobierzId() const { return id; }
    std::string pobierzNazwe() const { return nazwaStacji; }
    double pobierzSzerokosc() const { return szerokoscGeo; }
    double pobierzDlugosc() const { return dlugoscGeo; }
    
    std::string pobierzMiasto() const { return miasto; }
    std::string pobierzAdres() const { return ulica; }
    std::string pobierzWojewodztwo() const { return wojewodztwo; }

    void ustawId(int nowyId) { id = nowyId; }
    void ustawNazwe(const std::string& nowaNazwa) { nazwaStacji = nowaNazwa; }
    void ustawSzerokosc(double szerokosc) { szerokoscGeo = szerokosc; }
    void ustawDlugosc(double dlugosc) { dlugoscGeo = dlugosc; }
    
    void ustawMiasto(const std::string& noweMiasto) { miasto = noweMiasto; }
    void ustawAdres(const std::string& nowyAdres) { ulica = nowyAdres; }
    void ustawWojewodztwo(const std::string& noweWoj) { wojewodztwo = noweWoj; }

private:
    int id = 0;
    std::string nazwaStacji;
    double szerokoscGeo = 0.0;
    double dlugoscGeo = 0.0;
    std::string miasto;
    std::string ulica;
    std::string wojewodztwo;
};