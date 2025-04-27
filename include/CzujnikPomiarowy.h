// include/CzujnikPomiarowy.h
#pragma once
#include <string>

class CzujnikPomiarowy {
public:
    CzujnikPomiarowy() = default;
    CzujnikPomiarowy(int id, int idStacji, const std::string& nazwaPar, 
                     const std::string& symbolPar, const std::string& kodPar, int idPar);

    int pobierzId() const { return id; }
    int pobierzIdStacji() const { return idStacji; }
    std::string pobierzNazweParametru() const { return nazwaParametru; }
    std::string pobierzSymbolParametru() const { return symbolParametru; }
    std::string pobierzKodParametru() const { return kodParametru; }
    int pobierzIdParametru() const { return idParametru; }

private:
    int id = 0;
    int idStacji = 0;
    std::string nazwaParametru;
    std::string symbolParametru;
    std::string kodParametru;
    int idParametru = 0;
};