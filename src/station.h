#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <adresse.h>

typedef char string[256];

typedef struct station{
    string nom;
    IPv4 adresse_ip;
    MAC adresse_mac;
    IPv4 masque;
    IPv4 passerelle;
}station;

static const size_t UNKNOWN_INDEX = -1;

//fonctions
init_Station(station *pc);