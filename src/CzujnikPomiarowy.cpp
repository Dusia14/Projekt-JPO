// src/CzujnikPomiarowy.cpp
#include "CzujnikPomiarowy.h"

CzujnikPomiarowy::CzujnikPomiarowy(int id, int idStacji, const std::string& nazwaPar, 
                                  const std::string& symbolPar, const std::string& kodPar, int idPar)
    : id(id), idStacji(idStacji), nazwaParametru(nazwaPar), 
      symbolParametru(symbolPar), kodParametru(kodPar), idParametru(idPar) {}