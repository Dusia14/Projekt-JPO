// include/Pomiar.h
#pragma once
#include <string>
#include <optional>

class Pomiar {
public:
    Pomiar() = default;
    Pomiar(const std::string& data, std::optional<double> wartosc);
    
    std::string pobierzDate() const { return data; }
    std::optional<double> pobierzWartosc() const { return wartosc; }
    
private:
    std::string data; // data i czas pomiaru
    std::optional<double> wartosc;  // wartość może być null, więc używamy optional
};