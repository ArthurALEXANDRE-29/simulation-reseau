#include "station.h"
#include <stdio.h>
#include <string.h>

/**
 * Initialise une station avec des adresses MAC et IP nulles
 * Prépare la structure avant configuration
 */
void init_station(station_t *station) {
    if (station == NULL) return;
    
    init_mac(&station->mac);
    init_ip(&station->ip);
}

/**
 * Crée une station complète avec ses adresses MAC et IP
 * Représente un terminal sur le réseau
 */
station_t creer_station(mac_addr_t mac, ip_addr_t ip) {
    station_t station;
    station.mac = mac;
    station.ip = ip;
    return station;
}

/**
 * Affiche les informations d'une station (MAC et IP)
 * Utile pour le débogage et la supervision
 */
void afficher_station(const station_t *station) {
    if (station == NULL) {
        printf("Station: NULL\n");
        return;
    }
    
    printf("Station - MAC: ");
    afficher_mac(station->mac);
    printf(" | IP: ");
    afficher_ip(station->ip);
    printf("\n");
}

/**
 * Compare deux stations pour égalité
 * Permet d'identifier de manière unique chaque terminal
 */
bool station_equals(const station_t *s1, const station_t *s2) {
    if (s1 == NULL || s2 == NULL) return false;
    
    return mac_equals(s1->mac, s2->mac) && ip_equals(s1->ip, s2->ip);
}

/**
 * Copie les données d'une station vers une autre
 * Utile pour la duplication ou la sauvegarde
 */
void copier_station(station_t *dest, const station_t *src) {
    if (dest == NULL || src == NULL) return;
    
    dest->mac = src->mac;
    dest->ip = src->ip;
}