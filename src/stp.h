#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "adresse.h"
#include "switch.h"
#include "graphe.h"

/* Structure représentant un BPDU (simplifié) */
typedef struct {
    mac_addr_t   root_mac;
    unsigned int root_priority;
    int          root_cost;
    mac_addr_t   sender_mac;
    unsigned int sender_priority;
} bpdu_t;

/* Contexte STP d'un switch */
typedef struct {
    mac_addr_t   root_mac;
    unsigned int root_priority;
    int          root_cost;
    int          root_port;      /* -1 si ce switch est racine */
    switch_t    *sw;             /* pointeur vers le switch associé */
} switch_stp_t;

/* API STP */
void init_stp(switch_stp_t *stp, switch_t *sw);
void deinit_stp(switch_stp_t *stp);

int  comparer_mac(mac_addr_t mac1, mac_addr_t mac2);
bool bpdu_est_meilleur(switch_stp_t *stp, bpdu_t *bpdu);
void traiter_bpdu(switch_stp_t *stp, int port, bpdu_t *bpdu);
void recalculer_roles_ports(switch_stp_t *stp);
bpdu_t creer_bpdu(switch_stp_t *stp);
void calculer_stp_simple(switch_stp_t switches[], size_t nb_switches, graphe const *g);
void afficher_etat_stp(switch_stp_t *stp);