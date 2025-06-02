#pragma once

#include "adresse.h"
#include "switch.h"
#include "graphe.h"

typedef struct {
    mac_addr_t root_mac;
    int root_priority;
    int cost;
    mac_addr_t sender_mac;
} bpdu_t;

typedef struct {
    mac_addr_t root_mac;
    int root_priority;
    int root_cost;
    int root_port;
} switch_stp_t;

void init_stp(switch_stp_t *stp, switch_t *sw);
void deinit_stp(switch_stp_t *stp);
void dijkstra(graphe const *g, sommet s, int const *poids_arete, int *distance_sommet);