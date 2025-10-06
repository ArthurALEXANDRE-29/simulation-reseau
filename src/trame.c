#include "trame.h"
#include <stdio.h>
#include <string.h>

bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees){
    if(t == NULL || donnees == NULL){
        return false;
    }
    t->source = src;
    t->destination = dest;
    t->type = type;
    t->donnees = donnees;
    t->taille_donnees = taille_donnees;
    return true;
}

void deinit_trame(trame *t){
    if(t == NULL){
        return;
    }
    free(t->donnees);
    t->donnees = NULL;
    t->taille_donnees = 0;
}
<<<<<<< HEAD

/**
 * Calcule le FCS (Frame Check Sequence) d'une trame
 * Utilise l'algorithme CRC-32
 */
uint32_t calculer_fcs(const trame *t) {
    if (t == NULL) return 0;
    
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *data = (const uint8_t *)&t->destination;
    size_t len = sizeof(MAC) * 2 + sizeof(uint16_t) + t->taille_donnees;
    
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

/**
 * Vérifie si une adresse MAC est une adresse de broadcast 
 * Une adresse broadcast contient uniquement des 0xFF
 */
bool est_broadcast(MAC mac) {
    for (int i = 0; i < 6; i++) {
        if (mac.octet[i] != 0xFF) {
            return false;
        }
    }
    return true;
}
=======
>>>>>>> 45889d9 (Remove merge mark)
