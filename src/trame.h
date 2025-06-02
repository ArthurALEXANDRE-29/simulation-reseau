#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "adresse.h"

typedef struct {
    uint8_t preambule[7];
    uint8_t sfd;
    MAC destination;
    MAC source;
    uint16_t type;
    uint8_t *donnees;
    size_t taille_donnees;
    uint32_t fcs;
} trame;

// Types de trames Ethernet
#define TYPE_IPV4 0x0800
#define TYPE_ARP  0x0806
#define TYPE_IPV6 0x86DD
#define TYPE_STP  0x0027

bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees);
void deinit_trame(trame *t);
void afficher_trame(const trame *t);
void afficher_trame_utilisateur(const trame *t);
void afficher_trame_hexa(const trame *t);
void calculer_fcs(trame *t);
bool verifier_trame(const trame *t);
trame dupliquer_trame(const trame *t);