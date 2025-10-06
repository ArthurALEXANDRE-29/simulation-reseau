#include "adresse.h"
#include <string.h>

/**
 * Initialise une adresse MAC en mettant tous les octets à zéro
 * Sert de point de départ avant d'assigner une vraie adresse MAC
 */
void init_mac(mac_addr_t *mac) {
    if (mac == NULL) return;
    memset(mac->octet, 0, 6);
}

/**
 * Initialise une adresse IP en mettant tous les octets à zéro (0.0.0.0)
 * Prépare la structure avant assignation d'une IP valide
 */
void init_ip(ip_addr_t *ip) {
    if (ip == NULL) return;
    memset(ip->octet, 0, 4);
}

/**
 * Crée une adresse MAC à partir de 6 octets individuels
 * Permet de définir facilement une adresse MAC complète (ex: AA:BB:CC:DD:EE:FF)
 */
mac_addr_t creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6) {
    mac_addr_t mac;
    mac.octet[0] = o1;
    mac.octet[1] = o2;
    mac.octet[2] = o3;
    mac.octet[3] = o4;
    mac.octet[4] = o5;
    mac.octet[5] = o6;
    return mac;
}

/**
 * Crée une adresse IP à partir de 4 octets
 * Permet de définir une IP comme 192.168.1.1
 */
ip_addr_t creer_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) {
    ip_addr_t ip;
    ip.octet[0] = o1;
    ip.octet[1] = o2;
    ip.octet[2] = o3;
    ip.octet[3] = o4;
    return ip;
}

/**
 * Affiche une adresse MAC au format standard XX:XX:XX:XX:XX:XX
 * Utile pour le débogage et l'affichage des configurations réseau
 */
void afficher_mac(mac_addr_t mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x", 
           mac.octet[0], mac.octet[1], mac.octet[2], 
           mac.octet[3], mac.octet[4], mac.octet[5]);
}

/**
 * Affiche une adresse IP au format XXX.XXX.XXX.XXX
 * Essentiel pour visualiser les configurations IP des stations
 */
void afficher_ip(ip_addr_t ip) {
    printf("%u.%u.%u.%u", 
           ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
}

/**
 * Convertit une adresse MAC en chaîne de caractères
 * Permet de stocker l'adresse dans un buffer pour affichage ou sauvegarde
 */
char* MAC_to_string(mac_addr_t mac, char *str_mac) {
    if (str_mac == NULL) return NULL;
    
    sprintf(str_mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
            mac.octet[0], mac.octet[1], mac.octet[2], 
            mac.octet[3], mac.octet[4], mac.octet[5]);
    return str_mac;
}

/**
 * Convertit une adresse IP en chaîne de caractères
 * Utile pour les logs, l'affichage ou la sérialisation
 */
char* ip_to_string(ip_addr_t ip, char *str_ip) {
    if (str_ip == NULL) return NULL;
    
    sprintf(str_ip, "%u.%u.%u.%u", 
            ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
    return str_ip;
}

/**
 * Parse une chaîne au format "XX:XX:XX:XX:XX:XX" pour créer une adresse MAC
 * Essentiel pour lire les configurations depuis des fichiers
 */
bool parse_mac(const char *str, mac_addr_t *mac) {
    if (str == NULL || mac == NULL) return false;
    
    unsigned int octets[6];
    int result = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
                       &octets[0], &octets[1], &octets[2],
                       &octets[3], &octets[4], &octets[5]);
    
    if (result != 6) return false;
    
    for (int i = 0; i < 6; i++) {
        if (octets[i] > 255) return false;
        mac->octet[i] = (uint8_t)octets[i];
    }
    
    return true;
}

/**
 * Parse une chaîne au format "XXX.XXX.XXX.XXX" pour créer une adresse IP
 * Permet de lire les IPs depuis des fichiers de config
 */
bool parse_ip(const char *str, ip_addr_t *ip) {
    if (str == NULL || ip == NULL) return false;
    
    unsigned int octets[4];
    int result = sscanf(str, "%u.%u.%u.%u",
                       &octets[0], &octets[1], &octets[2], &octets[3]);
    
    if (result != 4) return false;
    
    for (int i = 0; i < 4; i++) {
        if (octets[i] > 255) return false;
        ip->octet[i] = (uint8_t)octets[i];
    }
    
    return true;
}

/**
 * Compare deux adresses MAC pour égalité
 * Utile pour identifier les équipements et gérer les tables de commutation
 */
bool mac_equals(mac_addr_t mac1, mac_addr_t mac2) {
    return memcmp(mac1.octet, mac2.octet, 6) == 0;
}

/**
 * Compare deux adresses IP pour égalité
 * Permet d'identifier les stations et gérer le routage
 */
bool ip_equals(ip_addr_t ip1, ip_addr_t ip2) {
    return memcmp(ip1.octet, ip2.octet, 4) == 0;
}
