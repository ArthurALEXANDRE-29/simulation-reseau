#pragma once

#include <stdlib.h>
#include <stdbool.h>



typedef struct IPv4{
    unsigned char IP[4]
}IPv4;


typedef struct MAC{
    unsigned char MAC[6];
}MAC;


static const size_t UNKNOWN_INDEX = -1;

//fonctions

void initIPv4(IPv4 *adresse)
void initMAC(IPv4 *adresse)

char afficherIPv4(const IPv4 *adresse)
char afficherMAC(const MAC *adresse)