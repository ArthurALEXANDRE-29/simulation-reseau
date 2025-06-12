#include "switch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void init_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

void deinit_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    
    free(table->entrees);
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port) {
    if (table == NULL || port < 0) return false;
    
    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac)) {
            table->entrees[i].port = port;
            return true;
        }
    }
    
    if (table->taille >= table->capacite) {
        int nouvelle_capacite = (table->capacite == 0) ? 8 : table->capacite * 2;
        table_entree_t *nouvelles_entrees = realloc(table->entrees, 
                                                     nouvelle_capacite * sizeof(table_entree_t));
        if (nouvelles_entrees == NULL) return false;
        
        table->entrees = nouvelles_entrees;
        table->capacite = nouvelle_capacite;
    }
    
    table->entrees[table->taille].mac = mac;
    table->entrees[table->taille].port = port;
    table->taille++;
    
    return true;
}

int chercher_port_mac(const table_commutation_t *table, mac_addr_t mac) {
    if (table == NULL) return -1;
    
    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac)) {
            return table->entrees[i].port;
        }
    }
    
    return -1; 
}

void vider_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    table->taille = 0;
}

void afficher_table_commutation(const table_commutation_t *table) {
    if (table == NULL) {
        printf("Table de commutation: NULL\n");
        return;
    }
    
    printf("Table de commutation (%d entrées):\n", table->taille);
    if (table->taille == 0) {
        printf("  (vide)\n");
        return;
    }
    
    for (int i = 0; i < table->taille; i++) {
        printf("  - MAC: ");
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
            sw.ports[i].state = PORT_DISABLED;
            sw.ports[i].role = PORT_UNKNOWN;
            sw.ports[i].actif = false;
            sw.ports[i].cost = 1; // Coût par défaut
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
            printf("  Port %d: %s/%s (coût: %d, %s)\n", i, 
                   port_state_to_string(sw->ports[i].state),
                   port_role_to_string(sw->ports[i].role),
                   sw->ports[i].cost,
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
            sw->ports[i].state = PORT_DISABLED;
            sw->ports[i].role = PORT_UNKNOWN;
            sw->ports[i].actif = false;
            sw->ports[i].cost = 1; // Coût par défaut
        }
    }
}

void set_port_state(switch_t *sw, int port, port_state_t state) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].state = state;
}

port_state_t get_port_state(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return PORT_DISABLED;
    }
    
    return sw->ports[port].state;
}

void set_port_role(switch_t *sw, int port, port_role_t role) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].role = role;
}

port_role_t get_port_role(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return PORT_UNKNOWN;
    }
    
    return sw->ports[port].role;
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

const char* port_state_to_string(port_state_t state) {
    switch (state) {
        case PORT_DISABLED: return "DISABLED";
        case PORT_BLOCKING: return "BLOCKING";
        case PORT_LISTENING: return "LISTENING";
        case PORT_LEARNING: return "LEARNING";
        case PORT_FORWARDING: return "FORWARDING";
        default: return "UNKNOWN";
    }
}

const char* port_role_to_string(port_role_t role) {
    switch (role) {
        case PORT_UNKNOWN: return "UNKNOWN";
        case PORT_ROOT: return "ROOT";
        case PORT_DESIGNATED: return "DESIGNATED";
        case PORT_BLOCKED: return "BLOCKED";
        default: return "INVALID";
    }
}