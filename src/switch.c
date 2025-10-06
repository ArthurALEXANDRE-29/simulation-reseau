#include "switch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== Table de commutation ===================== */

void init_table_commutation(table_commutation_t *table) {
    if (!table) return;
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

void deinit_table_commutation(table_commutation_t *table) {
    if (!table) return;
    free(table->entrees);
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port) {
    if (!table || port < 0) return false;

    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac)) {
            table->entrees[i].port = port;
            return true;
        }
    }

    if (table->taille >= table->capacite) {
        int nouvelle_capacite = (table->capacite == 0) ? 8 : table->capacite * 2;
        table_entree_t *nv = realloc(table->entrees, nouvelle_capacite * sizeof(table_entree_t));
        if (!nv) return false;
        table->entrees = nv;
        table->capacite = nouvelle_capacite;
    }

    table->entrees[table->taille].mac = mac;
    table->entrees[table->taille].port = port;
    table->taille++;
    return true;
}

int chercher_port_mac(const table_commutation_t *table, mac_addr_t mac) {
    if (!table) return -1;
    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac))
            return table->entrees[i].port;
    }
    return -1;
}

void vider_table_commutation(table_commutation_t *table) {
    if (!table) return;
    table->taille = 0;
}

void afficher_table_commutation(const table_commutation_t *table) {
    if (!table) {
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

/* ===================== Switch ===================== */

void init_switch(switch_t *sw) {
    if (!sw) return;
    init_mac(&sw->mac);
    sw->nb_ports = 0;
    sw->priorite = 0;
    init_table_commutation(&sw->table);
    sw->ports = NULL;
}

void deinit_switch(switch_t *sw) {
    if (!sw) return;
    deinit_table_commutation(&sw->table);
    free(sw->ports);
    sw->ports = NULL;
    sw->nb_ports = 0;
}

void init_ports(switch_t *sw) {
    if (!sw || sw->nb_ports <= 0) return;
    sw->ports = malloc(sw->nb_ports * sizeof(port_info_t));
    if (!sw->ports) return;
    for (int i = 0; i < sw->nb_ports; i++) {
        sw->ports[i].role  = PORT_ROLE_UNKNOWN;
        sw->ports[i].state = PORT_STATE_DISABLED;
        sw->ports[i].actif = false;
        sw->ports[i].cost  = 1;
    }
}

switch_t creer_switch(mac_addr_t mac, int nb_ports, unsigned int priorite) {
    switch_t sw;
    sw.mac = mac;
    sw.nb_ports = nb_ports;
    sw.priorite = priorite;
    init_table_commutation(&sw.table);
    sw.ports = NULL;
    init_ports(&sw);
    return sw;
}

void set_port_role(switch_t *sw, int port, port_role_t role) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return;
    sw->ports[port].role = role;
}

void set_port_state(switch_t *sw, int port, port_state_t state) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return;
    sw->ports[port].state = state;
}

port_role_t get_port_role(const switch_t *sw, int port) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return PORT_ROLE_UNKNOWN;
    return sw->ports[port].role;
}

port_state_t get_port_state(const switch_t *sw, int port) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return PORT_STATE_DISABLED;
    return sw->ports[port].state;
}

void set_port_cost(switch_t *sw, int port, int cost) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return;
    sw->ports[port].cost = cost;
}

void activer_port(switch_t *sw, int port) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return;
    sw->ports[port].actif = true;
}

void desactiver_port(switch_t *sw, int port) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return;
    sw->ports[port].actif = false;
}

bool port_est_actif(const switch_t *sw, int port) {
    if (!sw || !sw->ports || port < 0 || port >= sw->nb_ports) return false;
    return sw->ports[port].actif;
}

bool switch_equals(const switch_t *s1, const switch_t *s2) {
    if (!s1 || !s2) return false;
    return mac_equals(s1->mac, s2->mac) &&
           s1->nb_ports == s2->nb_ports &&
           s1->priorite == s2->priorite;
}

const char *port_role_to_string(port_role_t role) {
    switch (role) {
        case PORT_ROLE_ROOT:       return "ROOT";
        case PORT_ROLE_DESIGNATED: return "DESIGNATED";
        case PORT_ROLE_BLOCKED:    return "BLOCKED";
        case PORT_ROLE_UNKNOWN:    return "UNKNOWN";
        default: return "INVALID_ROLE";
    }
}

const char *port_state_to_string(port_state_t state) {
    switch (state) {
        case PORT_STATE_DISABLED:  return "DISABLED";
        case PORT_STATE_BLOCKING:  return "BLOCKING";
        case PORT_STATE_FORWARDING:return "FORWARDING";
        default: return "INVALID_STATE";
    }
}

void afficher_switch(const switch_t *sw) {
    if (!sw) {
        printf("Switch: NULL\n");
        return;
    }
    printf("Switch - MAC: ");
    afficher_mac(sw->mac);
    printf(" | Ports: %d | Priorité: %u\n", sw->nb_ports, sw->priorite);
    if (sw->ports) {
        printf("Ports:\n");
        for (int i = 0; i < sw->nb_ports; i++) {
            printf("  Port %d: role=%s, state=%s, cost=%d, %s\n",
                   i,
                   port_role_to_string(sw->ports[i].role),
                   port_state_to_string(sw->ports[i].state),
                   sw->ports[i].cost,
                   sw->ports[i].actif ? "actif" : "inactif");
        }
    }
    afficher_table_commutation(&sw->table);
}