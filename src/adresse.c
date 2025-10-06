#include "adresse.h"

/* Initialisation */
void init_mac(mac_addr_t *mac) {
    if (!mac) return;
    memset(mac->octet, 0, sizeof(mac->octet));
}

void init_ip(ip_addr_t *ip) {
    if (!ip) return;
    memset(ip->octet, 0, sizeof(ip->octet));
}

/* Création */
mac_addr_t creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6) {
    mac_addr_t m = {{o1,o2,o3,o4,o5,o6}};
    return m;
}

ip_addr_t creer_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) {
    ip_addr_t ip = {{o1,o2,o3,o4}};
    return ip;
}

/* Affichage */
void afficher_mac(mac_addr_t mac) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X",
           mac.octet[0], mac.octet[1], mac.octet[2],
           mac.octet[3], mac.octet[4], mac.octet[5]);
}

void afficher_ip(ip_addr_t ip) {
    printf("%u.%u.%u.%u",
           ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
}

/* Conversions */
char *mac_to_string(mac_addr_t mac, char *str_mac) {
    if (!str_mac) return NULL;
    snprintf(str_mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac.octet[0], mac.octet[1], mac.octet[2],
             mac.octet[3], mac.octet[4], mac.octet[5]);
    return str_mac;
}

char *ip_to_string(ip_addr_t ip, char *str_ip) {
    if (!str_ip) return NULL;
    snprintf(str_ip, 16, "%u.%u.%u.%u",
             ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
    return str_ip;
}

/* Aliases compat */
char *MAC_to_string(mac_addr_t mac, char *str_mac) {
    return mac_to_string(mac, str_mac);
}

char *IPv4_to_string(ip_addr_t ip, char *str_ip) {
    return ip_to_string(ip, str_ip);
}

/* Parsing */
bool parse_mac(const char *str, mac_addr_t *mac) {
    if (!str || !mac) return false;
    unsigned int tmp[6];
    if (sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
               &tmp[0], &tmp[1], &tmp[2],
               &tmp[3], &tmp[4], &tmp[5]) != 6)
        return false;
    for (int i = 0; i < 6; ++i) {
        if (tmp[i] > 255) return false;
        mac->octet[i] = (uint8_t)tmp[i];
    }
    return true;
}

bool parse_ip(const char *str, ip_addr_t *ip) {
    if (!str || !ip) return false;
    unsigned int tmp[4];
    if (sscanf(str, "%u.%u.%u.%u",
               &tmp[0], &tmp[1], &tmp[2], &tmp[3]) != 4)
        return false;
    for (int i = 0; i < 4; ++i) {
        if (tmp[i] > 255) return false;
        ip->octet[i] = (uint8_t)tmp[i];
    }
    return true;
}

/* Comparaisons */
bool mac_equals(mac_addr_t m1, mac_addr_t m2) {
    return memcmp(m1.octet, m2.octet, 6) == 0;
}

bool ip_equals(ip_addr_t i1, ip_addr_t i2) {
    return memcmp(i1.octet, i2.octet, 4) == 0;
}