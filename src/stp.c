#include "stp.h"
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

void init_stp(switch_stp_t *stp, switch_t *sw) 
{
    stp->root_mac = sw->mac;
    stp->root_priority = 0;
    stp->root_cost = 0;
    stp->root_port = -1;
}

void deinit_stp(switch_stp_t *stp) 
{
    (void)stp;
}

void dijkstra(graphe const *g, sommet s, int const *poids_arete, int *distance_sommet)
{
    if (s >= g->ordre) {
        return;
    }
    
    for (size_t i = 0; i < g->ordre; i++) {
        distance_sommet[i] = INT_MAX;
    }
    distance_sommet[s] = 0;
    
    bool *visite = malloc(sizeof(bool) * g->ordre);
    if (!visite) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    
    for (size_t i = 0; i < g->ordre; i++) {
        visite[i] = false;
    }
    
    for (size_t count = 0; count < g->ordre; count++) {
        sommet u = (sommet)-1;
        int min_distance = INT_MAX;
        
        for (size_t v = 0; v < g->ordre; v++) {
            if (!visite[v] && distance_sommet[v] < min_distance) {
                min_distance = distance_sommet[v];
                u = v;
            }
        }
        
        if (u == (sommet)-1) {
            break;
        }
        
        visite[u] = true;
        
        for (size_t i = 0; i < g->nb_aretes; i++) {
            sommet voisin = (sommet)-1;
            
            if (g->aretes[i].s1 == u) {
                voisin = g->aretes[i].s2;
            } else if (g->aretes[i].s2 == u) {
                voisin = g->aretes[i].s1;
            }
            
            if (voisin != (sommet)-1 && !visite[voisin]) {
                int nouvelle_distance = distance_sommet[u] + poids_arete[i];
                
                if (nouvelle_distance < distance_sommet[voisin]) {
                    distance_sommet[voisin] = nouvelle_distance;
                }
            }
        }
    }
    
    free(visite);
}