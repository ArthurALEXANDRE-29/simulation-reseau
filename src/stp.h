#pragma once

#include "adresse.h"
#include "switch.h" 
#include "graphe.h"

// Structure pour les BPDU
typedef struct {
    mac_addr_t root_mac;
    int root_priority;
    int root_cost;
    mac_addr_t sender_mac;
    int sender_priority;
} bpdu_t;

// Structure pour les informations STP du switch
typedef struct {
    mac_addr_t root_mac;        // MAC du pont racine
    int root_priority;          // Priorité du pont racine
    int root_cost;             // Coût vers le pont racine
    int root_port;             // Port racine (-1 si c'est le pont racine)
    switch_t *sw;              // Référence vers le switch
} switch_stp_t;

// Fonctions de base
void init_stp(switch_stp_t *stp, switch_t *sw);
void deinit_stp(switch_stp_t *stp);

// Fonctions utilitaires
int comparer_mac(mac_addr_t mac1, mac_addr_t mac2);
bool bpdu_est_meilleur(switch_stp_t *stp, bpdu_t *bpdu);

// Fonctions de traitement STP
void traiter_bpdu(switch_stp_t *stp, int port, bpdu_t *bpdu);
void recalculer_roles_ports(switch_stp_t *stp);
bpdu_t creer_bpdu(switch_stp_t *stp);

// Fonction principale de calcul STP
void calculer_stp_simple(switch_stp_t switches[], size_t nb_switches, graphe const *g);

// Fonction d'affichage
void afficher_etat_stp(switch_stp_t *stp);