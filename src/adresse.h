#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Types */
typedef struct {
    uint8_t octet[6];
} mac_addr_t;

typedef struct {
    uint8_t octet[4];
} ip_addr_t;

/* Alias historiques */
typedef mac_addr_t MAC;
typedef ip_addr_t  IPv4;

/* Initialisation */
void init_mac(mac_addr_t *mac);
void init_ip(ip_addr_t *ip);

/* Création */
mac_addr_t creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6);
ip_addr_t  creer_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4);

/* Affichage direct */
void afficher_mac(mac_addr_t mac);
void afficher_ip(ip_addr_t ip);

/* Conversions vers chaîne (buffer fourni par l'appelant) */
char *mac_to_string(mac_addr_t mac, char *str_mac);
char *ip_to_string(ip_addr_t ip, char *str_ip);

/* Alias pour compatibilité avec du code existant */
char *MAC_to_string(mac_addr_t mac, char *str_mac);
char *IPv4_to_string(ip_addr_t ip, char *str_ip);

/* Parsing */
bool parse_mac(const char *str, mac_addr_t *mac);
bool parse_ip(const char *str, ip_addr_t *ip);

/* Comparaisons */
bool mac_equals(mac_addr_t mac1, mac_addr_t mac2);
bool ip_equals(ip_addr_t ip1, ip_addr_t ip2);