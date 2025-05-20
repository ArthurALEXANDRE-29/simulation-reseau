#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <adresse.h>

typedef char string[256];

typedef struct Nintendo{
    MAC adresse_mac;
    STP jetensuppliepitie;
}Nintendo;

static const size_t UNKNOWN_INDEX = -1;

//fonctions
