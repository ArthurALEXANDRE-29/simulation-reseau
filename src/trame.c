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

void afficher_trame(const trame *t){
    if(t == NULL){
        return;
    }
    char str_mac[18];
    printf("Trame: %s\n", t->donnees);
    MAC_to_string(t->source, str_mac);
    printf("Source: %s\n", str_mac);
    MAC_to_string(t->destination, str_mac);
    printf("Destination: %s\n", str_mac);
    printf("Type: %d\n", t->type);
    printf("Taille: %zu\n", t->taille_donnees);
    printf("FCS: %d\n", t->fcs);
}

void deinit_trame(trame *t){
    if(t == NULL){
        return;
    }
    free(t->donnees);
    t->donnees = NULL;
    t->taille_donnees = 0;
}

void afficher_adresse_mac(MAC mac) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X", 
           mac.octet[0], mac.octet[1], mac.octet[2], 
           mac.octet[3], mac.octet[4], mac.octet[5]);
}

uint32_t calculer_crc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    static const uint32_t table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        // ... (table complète omise pour la concision)
    };
    
    for (size_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ table[(crc ^ data[i]) & 0xFF];
    }
    
    return ~crc;
}
