// src/KlientAPI.cpp
#include "KlientAPI.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

// Callback dla cURL do zapisywania odpowiedzi
static size_t CallbackZapisu(void* dane, size_t rozmiar, size_t liczba, std::string* s) {
    size_t nowyRozmiar = rozmiar * liczba;
    try {
        s->append((char*)dane, nowyRozmiar);
        return nowyRozmiar;
    } catch(std::bad_alloc& e) {
        // Jeśli brak pamięci
        return 0;
    }
}

KlientAPI::KlientAPI() {
    // Inicjalizacja cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

KlientAPI::~KlientAPI() {
    // Zwalnianie zasobów cURL
    curl_global_cleanup();
}

// src/KlientAPI.cpp
std::string KlientAPI::wykonajZapytanieHTTP(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string buforOdczytu;

    if(curl) {
        // Usuńmy ścieżkę bezwzględną do certyfikatu, która jest specyficzna dla jednego komputera
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\\Users\\User\\Desktop\\MonitorPowietrza\\certs.pem");

        // Zamiast tego pomiń weryfikację certyfikatu (nie zalecane w produkcji, ale dobre do testów)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallbackZapisu);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buforOdczytu);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);  // zwiększmy timeout do 15 sekund

        // Dodajmy User-Agent - niektóre API tego wymagają
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "MonitorPowietrza/1.0");

        CURLcode wynik = curl_easy_perform(curl);

        if(wynik != CURLE_OK) {
            std::cerr << "Błąd curl_easy_perform(): " << curl_easy_strerror(wynik) << std::endl;
            // Dodajmy więcej informacji o błędzie
            std::cerr << "URL: " << url << std::endl;
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::cerr << "Kod HTTP: " << httpCode << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Nie można zainicjalizować libcurl!" << std::endl;
    }

    // Debugowanie - wyświetlmy pierwsze 200 znaków odpowiedzi
    if (!buforOdczytu.empty()) {
        std::cerr << "Otrzymano odpowiedź z API (pierwsze 200 znaków): "
                  << buforOdczytu.substr(0, 200) << std::endl;
    } else {
        std::cerr << "Pusta odpowiedź z API!" << std::endl;
    }

    return buforOdczytu;
}

// src/KlientAPI.cpp
std::vector<StacjaPomiarowa> KlientAPI::pobierzStacje() {
    std::vector<StacjaPomiarowa> stacje;

    std::string odpowiedz = wykonajZapytanieHTTP(bazowy_url + "/station/findAll");

    if (odpowiedz.empty()) {
        std::cerr << "Otrzymano pustą odpowiedź z API stacji." << std::endl;
        return stacje;
    }

    try {
        json stacjeJson = json::parse(odpowiedz);

        for (const auto& stacjaJson : stacjeJson) {
            try {
                // Bezpieczne parsowanie wartości - obsługujemy zarówno stringi jak i liczby
                int id;
                if (stacjaJson["id"].is_string()) {
                    id = std::stoi(stacjaJson["id"].get<std::string>());
                } else {
                    id = stacjaJson["id"].get<int>();
                }

                std::string nazwa = stacjaJson["stationName"].get<std::string>();

                double szerokosc;
                if (stacjaJson["gegrLat"].is_string()) {
                    szerokosc = std::stod(stacjaJson["gegrLat"].get<std::string>());
                } else {
                    szerokosc = stacjaJson["gegrLat"].get<double>();
                }

                double dlugosc;
                if (stacjaJson["gegrLon"].is_string()) {
                    dlugosc = std::stod(stacjaJson["gegrLon"].get<std::string>());
                } else {
                    dlugosc = stacjaJson["gegrLon"].get<double>();
                }

                StacjaPomiarowa stacja(id, nazwa, szerokosc, dlugosc);

                // Dodatkowe informacje o lokalizacji
                if (stacjaJson.contains("city") && !stacjaJson["city"].is_null()) {
                    if (stacjaJson["city"].contains("name") && !stacjaJson["city"]["name"].is_null()) {
                        stacja.ustawMiasto(stacjaJson["city"]["name"].get<std::string>());
                    }

                    if (stacjaJson["city"].contains("commune") && !stacjaJson["city"]["commune"].is_null()) {
                        if (stacjaJson["city"]["commune"].contains("provinceName") &&
                            !stacjaJson["city"]["commune"]["provinceName"].is_null()) {
                            stacja.ustawWojewodztwo(stacjaJson["city"]["commune"]["provinceName"].get<std::string>());
                        }
                    }
                }

                if (stacjaJson.contains("addressStreet") && !stacjaJson["addressStreet"].is_null()) {
                    stacja.ustawAdres(stacjaJson["addressStreet"].get<std::string>());
                }

                stacje.push_back(stacja);
            } catch (const std::exception& e) {
                std::cerr << "Błąd parsowania pojedynczej stacji: " << e.what() << std::endl;
                if (stacjaJson.contains("id")) {
                    std::cerr << "ID problematycznej stacji: " << stacjaJson["id"] << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania JSON stacji: " << e.what() << std::endl;
    }

    std::cerr << "Pobrano " << stacje.size() << " stacji z API." << std::endl;
    return stacje;
}

std::vector<CzujnikPomiarowy> KlientAPI::pobierzCzujniki(int idStacji) {
    std::vector<CzujnikPomiarowy> czujniki;

    std::string odpowiedz = wykonajZapytanieHTTP(bazowy_url + "/station/sensors/" + std::to_string(idStacji));

    try {
        json czujnikiJson = json::parse(odpowiedz);

        for (const auto& czujnikJson : czujnikiJson) {
            int id = czujnikJson["id"];
            int idSt = czujnikJson["stationId"];

            std::string nazwaPar = "";
            std::string symbolPar = "";
            std::string kodPar = "";
            int idPar = 0;

            if (czujnikJson.contains("param") && !czujnikJson["param"].is_null()) {
                if (czujnikJson["param"].contains("paramName") && !czujnikJson["param"]["paramName"].is_null()) {
                    nazwaPar = czujnikJson["param"]["paramName"];
                }

                if (czujnikJson["param"].contains("paramFormula") && !czujnikJson["param"]["paramFormula"].is_null()) {
                    symbolPar = czujnikJson["param"]["paramFormula"];
                }

                if (czujnikJson["param"].contains("paramCode") && !czujnikJson["param"]["paramCode"].is_null()) {
                    kodPar = czujnikJson["param"]["paramCode"];
                }

                if (czujnikJson["param"].contains("idParam") && !czujnikJson["param"]["idParam"].is_null()) {
                    idPar = czujnikJson["param"]["idParam"];
                }
            }

            CzujnikPomiarowy czujnik(id, idSt, nazwaPar, symbolPar, kodPar, idPar);
            czujniki.push_back(czujnik);
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania czujników: " << e.what() << std::endl;
    }

    return czujniki;
}

std::vector<Pomiar> KlientAPI::pobierzPomiary(int idCzujnika) {
    std::vector<Pomiar> pomiary;

    std::string odpowiedz = wykonajZapytanieHTTP(bazowy_url + "/data/getData/" + std::to_string(idCzujnika));

    try {
        json daneJson = json::parse(odpowiedz);

        if (daneJson.contains("values") && !daneJson["values"].is_null()) {
            for (const auto& wartoscJson : daneJson["values"]) {
                std::string data = wartoscJson["date"];
                std::optional<double> wartosc;

                if (!wartoscJson["value"].is_null()) {
                    wartosc = wartoscJson["value"];
                }

                pomiary.emplace_back(data, wartosc);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania pomiarów: " << e.what() << std::endl;
    }

    return pomiary;
}

std::optional<int> KlientAPI::pobierzIndeksJakosci(int idStacji) {
    std::string odpowiedz = wykonajZapytanieHTTP(bazowy_url + "/aqindex/getIndex/" + std::to_string(idStacji));

    try {
        json indeksJson = json::parse(odpowiedz);

        if (indeksJson.contains("stIndexLevel") && !indeksJson["stIndexLevel"].is_null()) {
            if (indeksJson["stIndexLevel"].contains("id") && !indeksJson["stIndexLevel"]["id"].is_null()) {
                return indeksJson["stIndexLevel"]["id"];
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd parsowania indeksu jakości: " << e.what() << std::endl;
    }

    return std::nullopt;
}
