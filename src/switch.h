#pragma once

#include "adresse.h"
#include <stdbool.h>
#include <stddef.h>

/* ===================== Rôles et États STP ===================== */

typedef enum {
    PORT_ROLE_UNKNOWN = 0,
    PORT_ROLE_ROOT,
    PORT_ROLE_DESIGNATED,
    PORT_ROLE_BLOCKED
} port_role_t;

typedef enum {
    PORT_STATE_DISABLED = 0,
    PORT_STATE_BLOCKING,
    PORT_STATE_FORWARDING
} port_state_t;

/* ===================== Structures ===================== */

/* Entrée de table de commutation */
typedef struct {
    mac_addr_t mac;
    int        port;
} table_entree_t;

typedef struct {
    table_entree_t *entrees;
    int             taille;
    int             capacite;
} table_commutation_t;

/* Informations sur un port physique/logique */
typedef struct {
    port_role_t  role;
    port_state_t state;
    bool         actif;
    int          cost;   /* coût STP (par défaut 1) */
} port_info_t;

typedef struct {
    mac_addr_t          mac;
    int                 nb_ports;
    unsigned int        priorite;
    table_commutation_t table;
    port_info_t        *ports;
} switch_t;

/* ===================== Table de commutation ===================== */
void init_table_commutation(table_commutation_t *table);
void deinit_table_commutation(table_commutation_t *table);
bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port);
int  chercher_port_mac(const table_commutation_t *table, mac_addr_t mac);
void vider_table_commutation(table_commutation_t *table);
void afficher_table_commutation(const table_commutation_t *table);

/* ===================== Switch ===================== */
void     init_switch(switch_t *sw);
void     deinit_switch(switch_t *sw);
switch_t creer_switch(mac_addr_t mac, int nb_ports, unsigned int priorite);
void     afficher_switch(const switch_t *sw);

/* Ports */
void init_ports(switch_t *sw);
void set_port_role(switch_t *sw, int port, port_role_t role);
void set_port_state(switch_t *sw, int port, port_state_t state);
port_role_t  get_port_role(const switch_t *sw, int port);
port_state_t get_port_state(const switch_t *sw, int port);
void activer_port(switch_t *sw, int port);
void desactiver_port(switch_t *sw, int port);
bool port_est_actif(const switch_t *sw, int port);
void set_port_cost(switch_t *sw, int port, int cost);

/* Comparaison */
bool switch_equals(const switch_t *s1, const switch_t *s2);

/* Conversions rôle/état vers chaînes */
const char *port_role_to_string(port_role_t role);
const char *port_state_to_string(port_state_t state);

/* ===================== Compatibilité ancienne version ===================== */
/* Si ton ancien code utilisait une seule enum et fonctions set_etat_port etc.,
   voici des wrappers pour éviter de tout casser d’un coup. */
typedef port_role_t  etat_port_t; /* ancien alias approximatif */

#define PORT_INCONNU  PORT_ROLE_UNKNOWN
#define PORT_RACINE   PORT_ROLE_ROOT
#define PORT_DESIGNE  PORT_ROLE_DESIGNATED
#define PORT_BLOQUE   PORT_ROLE_BLOCKED

static inline void set_etat_port(switch_t *sw, int port, etat_port_t e) { set_port_role(sw, port, (port_role_t)e); }
static inline etat_port_t get_etat_port(const switch_t *sw, int port) { return (etat_port_t)get_port_role(sw, port); }
static inline const char *etat_port_to_string(etat_port_t e) { return port_role_to_string((port_role_t)e); }
