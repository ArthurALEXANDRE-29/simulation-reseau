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
HEAD
    
    for (int i = 0; i < table->taille; i++) {
        printf("  - MAC: ");
HEAD
        afficher_mac(table->entrees[i].mac);
        printf(" -> Port: %d\n", table->entrees[i].port);
    }
}


void init_switch(switch_t *sw) {
    if (sw == NULL) return;
    
    init_mac(&sw->mac);
    sw->nb_ports = 0;
    sw->priorite = 0;
    init_table_commutation(&sw->table);
    sw->ports = NULL;
        afficher_MAC(&table.entrees[i].mac);
        printf(" -> Port: %d\n", table.entrees[i].port);
    }
}

void afficher_switch(switch_t s) {
    printf("Switch - MAC: ");
    afficher_MAC(&s.mac);
    printf(" | Ports: %d | Priorité: %d\n", s.nb_ports, s.priorite);
    afficher_table(s.table);
}

void deinit_switch(switch_t *sw) {
    if (sw == NULL) return;
    
    deinit_table_commutation(&sw->table);
    free(sw->ports);
    sw->ports = NULL;
    sw->nb_ports = 0;
}

switch_t creer_switch(mac_addr_t mac, int nb_ports, int priorite) {
    switch_t sw;
    sw.mac = mac;
    sw.nb_ports = nb_ports;
    sw.priorite = priorite;
    init_table_commutation(&sw.table);
    
    sw.ports = malloc(nb_ports * sizeof(port_info_t));
    if (sw.ports != NULL) {
        for (int i = 0; i < nb_ports; i++) {
            sw.ports[i].etat = PORT_INCONNU;
            sw.ports[i].actif = false;
        }
    }
    
    return sw;
}

void afficher_switch(const switch_t *sw) {
    if (sw == NULL) {
        printf("Switch: NULL\n");
        return;
    }
    
    printf("Switch - MAC: ");
    afficher_mac(sw->mac);
    printf(" | Ports: %d | Priorité: %d\n", sw->nb_ports, sw->priorite);
    
    if (sw->ports != NULL) {
        printf("État des ports:\n");
        for (int i = 0; i < sw->nb_ports; i++) {
            printf("  Port %d: %s (%s)\n", i, 
                   etat_port_to_string(sw->ports[i].etat),
                   sw->ports[i].actif ? "actif" : "inactif");
        }
    }
    
    afficher_table_commutation(&sw->table);
}


void init_ports(switch_t *sw) {
    if (sw == NULL || sw->nb_ports <= 0) return;
    
    sw->ports = malloc(sw->nb_ports * sizeof(port_info_t));
    if (sw->ports != NULL) {
        for (int i = 0; i < sw->nb_ports; i++) {
            sw->ports[i].etat = PORT_INCONNU;
            sw->ports[i].actif = false;
        }
    }
}

void set_etat_port(switch_t *sw, int port, etat_port_t etat) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].etat = etat;
}

etat_port_t get_etat_port(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return PORT_INCONNU;
    }
    
    return sw->ports[port].etat;
}

void activer_port(switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].actif = true;
}

void desactiver_port(switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].actif = false;
}

bool port_est_actif(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return false;
    }
    
    return sw->ports[port].actif;
}


bool switch_equals(const switch_t *s1, const switch_t *s2) {
    if (s1 == NULL || s2 == NULL) return false;
    
    return mac_equals(s1->mac, s2->mac) && 
           s1->nb_ports == s2->nb_ports && 
           s1->priorite == s2->priorite;
}

const char* etat_port_to_string(etat_port_t etat) {
    switch (etat) {
        case PORT_INCONNU: return "INCONNU";
        case PORT_RACINE: return "RACINE";
        case PORT_DESIGNE: return "DESIGNE";
        case PORT_BLOQUE: return "BLOQUE";
        default: return "INVALIDE";
    }
}
    free(t->donnees);
    t->donnees = NULL;
    t->taille_donnees = 0;
}
