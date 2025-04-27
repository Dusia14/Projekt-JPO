# tests.pro - Główny plik zarządzający testami

TEMPLATE = subdirs

# Podkatalogi z testami
SUBDIRS += \
    TestBazaDanych \
    TestStacjaPomiarowa

# Konfiguracja dla wszystkich testów
CONFIG += ordered
