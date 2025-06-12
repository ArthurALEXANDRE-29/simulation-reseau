#pragma once

#include "adresse.h"
#include <stdbool.h>

// Structure pour une entrée de la table de commutation
typedef struct {
    mac_addr_t mac;
    int port;
} table_entree_t;

// Structure pour la table de commutation
typedef struct {
    table_entree_t *entrees;
    int taille;
    int capacite;
} table_commutation_t;

// États STP des ports (conformes au standard IEEE 802.1D)
typedef enum {
    PORT_DISABLED = 0,
    PORT_BLOCKING,
    PORT_LISTENING,
    PORT_LEARNING,
    PORT_FORWARDING
} port_state_t;

// Rôles STP des ports
typedef enum {
    PORT_UNKNOWN = 0,
    PORT_ROOT,
    PORT_DESIGNATED,
    PORT_BLOCKED
} port_role_t;

// Informations complètes d'un port
typedef struct {
    port_state_t state;
    port_role_t role;
    bool actif;
    int cost;
    // Autres informations STP si nécessaire
} port_info_t;

typedef struct {
    mac_addr_t mac;
    int nb_ports;
    int priorite;
    table_commutation_t table;
    port_info_t *ports;
} switch_t;

// Fonctions de table de commutation
void init_table_commutation(table_commutation_t *table);
void deinit_table_commutation(table_commutation_t *table);
bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port);
int chercher_port_mac(const table_commutation_t *table, mac_addr_t mac);
void vider_table_commutation(table_commutation_t *table);
void afficher_table_commutation(const table_commutation_t *table);

// Fonctions de switch
void init_switch(switch_t *sw);
void deinit_switch(switch_t *sw);
switch_t creer_switch(mac_addr_t mac, int nb_ports, int priorite);
void afficher_switch(const switch_t *sw);

// Fonctions de gestion des ports
void init_ports(switch_t *sw);
void set_port_state(switch_t *sw, int port, port_state_t state);
port_state_t get_port_state(const switch_t *sw, int port);
void set_port_role(switch_t *sw, int port, port_role_t role);
port_role_t get_port_role(const switch_t *sw, int port);
void activer_port(switch_t *sw, int port);
void desactiver_port(switch_t *sw, int port);
bool port_est_actif(const switch_t *sw, int port);

// Fonctions utilitaires
bool switch_equals(const switch_t *s1, const switch_t *s2);
const char* port_state_to_string(port_state_t state);
const char* port_role_to_string(port_role_t role);