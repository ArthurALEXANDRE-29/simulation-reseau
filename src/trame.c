#include "trame.h"
#include "switch.h"
#include <stdio.h>
#include <string.h>

bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees){
    if(t == NULL){
        return false;
    }
    
    t->source = src;
    t->destination = dest;
    t->type = type;
    t->taille_donnees = taille_donnees;
    
    if (donnees != NULL && taille_donnees > 0) {
        t->donnees = malloc(taille_donnees);
        if (t->donnees == NULL) {
            return false;
        }
        memcpy(t->donnees, donnees, taille_donnees);
    } else {
        t->donnees = NULL;
        t->taille_donnees = 0;
    }
    
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

void envoyer_trame_sur_port(trame *t, int port) {
    if (t == NULL || port < 0) {
        return;
    }
    
    uint32_t fcs_calcule = calculer_fcs(t);
    if (fcs_calcule != t->fcs) {
        printf("Erreur FCS: trame corrompue, envoi annulé\n");
        return;
    }
    
    printf("Envoi de la trame sur le port %d:\n", port);
    afficher_trame(t);
}

void envoyer_trame(trame *t, switch_t *sw) {
    if (t == NULL || sw == NULL) {
        return;
    }

    if (est_broadcast(t->destination)) {
        for (int port = 0; port < sw->nb_ports; port++) {
            if (port_est_actif(sw, port)) {
                envoyer_trame_sur_port(t, port);
            }
        }
    } else {
        int port = chercher_port_mac(&sw->table, t->destination);
        
        if (port >= 0 && port < sw->nb_ports && port_est_actif(sw, port)) {
            envoyer_trame_sur_port(t, port);
        } else {
            printf("Destination non trouvée dans la table, envoi en broadcast\n");
            for (int port = 0; port < sw->nb_ports; port++) {
                if (port_est_actif(sw, port)) {
                    envoyer_trame_sur_port(t, port);
                }
            }
        }
    }
    
    deinit_trame(t);
}

void deinit_trame(trame *t){
    if(t == NULL){
        return;
    }
    free(t->donnees);
    t->donnees = NULL;
    t->taille_donnees = 0;
}

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

bool est_broadcast(MAC mac) {
    for (int i = 0; i < 6; i++) {
        if (mac.octet[i] != 0xFF) {
            return false;
        }
    }
    return true;
}
