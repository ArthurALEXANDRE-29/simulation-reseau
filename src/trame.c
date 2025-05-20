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
