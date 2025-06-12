#pragma once

#include "adresse.h"
#include "switch.h" 
#include "graphe.h"

// Structure pour les BPDU
typedef struct {
    mac_addr_t root_mac;
    int root_priority;
    int cost;
    mac_addr_t sender_mac;
} bpdu_t;

// Structure pour les informations STP du switch
typedef struct {
    mac_addr_t root_mac;
    int root_priority;
    int root_cost;
    int root_port;
} switch_stp_t;

// Structure étendue pour les informations STP des ports
typedef struct {
    bpdu_t last_bpdu;
    bool received_bpdu;
    // Les autres infos (state, role, cost) sont dans port_info_t du switch
} port_stp_extended_t;

// Fonctions de base
void init_stp(switch_stp_t *stp, switch_t *sw);
void deinit_stp(switch_stp_t *stp);

// Fonctions utilitaires
int compare_mac(const mac_addr_t *mac1, const mac_addr_t *mac2);
bool is_better_bpdu(const bpdu_t *bpdu1, const bpdu_t *bpdu2);
bpdu_t create_bpdu(switch_stp_t *stp, switch_t *sw, int port_cost);

// Fonctions de traitement STP
void send_bpdu(switch_t *sw, int port, bpdu_t *bpdu, graphe *g);
bool process_received_bpdu(switch_stp_t *stp, switch_t *sw, bpdu_t *received_bpdu, 
                          int receiving_port, int port_cost);
void determine_port_roles(switch_stp_t *stp, switch_t *sw, graphe *g, 
                         port_stp_extended_t *port_ext_info, int *port_costs);
void update_port_states(switch_t *switches, int nb_switches);

// Fonction principale et affichage
void run_stp_algorithm(graphe *g, switch_t *switches, int nb_switches, int *port_costs);
void print_stp_state(switch_stp_t *stp_info, switch_t *switches, 
                     port_stp_extended_t **port_ext_infos, int nb_switches);

// Dijkstra
void dijkstra(graphe const *g, sommet s, int const *poids_arete, int *distance_sommet);