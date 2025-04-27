// src/StacjaPomiarowa.cpp
#include "StacjaPomiarowa.h"

StacjaPomiarowa::StacjaPomiarowa(int id, const std::string& nazwa, double szerokosc, double dlugosc) 
    : id(id), nazwaStacji(nazwa), szerokoscGeo(szerokosc), dlugoscGeo(dlugosc) {}