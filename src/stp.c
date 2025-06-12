#include "stp.h"
#include "switch.h"
#include "graphe.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init_stp(switch_stp_t *stp, switch_t *sw) 
{
    if (!stp || !sw) {
        return;
    }
    
    // Au début, chaque switch pense qu'il est la racine
    stp->root_mac = sw->mac;
    stp->root_priority = sw->priorite;
    stp->root_cost = 0;
    stp->root_port = -1;
    stp->sw = sw;
    
    // Initialiser tous les ports en état BLOCKING par défaut
    for (int i = 0; i < sw->nb_ports; i++) {
        set_port_state(sw, i, PORT_BLOCKING);
        set_port_role(sw, i, PORT_UNKNOWN);
    }
}

void deinit_stp(switch_stp_t *stp) 
{
    if (!stp) {
        return;
    }
    
    // Désactiver tous les ports
    if (stp->sw) {
        for (int i = 0; i < stp->sw->nb_ports; i++) {
            set_port_state(stp->sw, i, PORT_DISABLED);
        }
    }
    
    stp->sw = NULL;
}

// Compare deux adresses MAC (retourne -1 si mac1 < mac2, 0 si égales, 1 si mac1 > mac2)
int comparer_mac(mac_addr_t mac1, mac_addr_t mac2)
{
    for (int i = 0; i < 6; i++) {
        if (mac1.octet[i] < mac2.octet[i]) return -1;
        if (mac1.octet[i] > mac2.octet[i]) return 1;
    }
    return 0;
}

// Détermine si un BPDU est meilleur que les informations actuelles
bool bpdu_est_meilleur(switch_stp_t *stp, bpdu_t *bpdu)
{
    // Comparer par priorité de racine
    if (bpdu->root_priority < stp->root_priority) return true;
    if (bpdu->root_priority > stp->root_priority) return false;
    
    // Si même priorité, comparer par MAC de racine
    int cmp_root = comparer_mac(bpdu->root_mac, stp->root_mac);
    if (cmp_root < 0) return true;
    if (cmp_root > 0) return false;
    
    // Si même racine, comparer par coût
    if (bpdu->root_cost < stp->root_cost) return true;
    if (bpdu->root_cost > stp->root_cost) return false;
    
    // Si même coût, comparer par priorité de l'expéditeur
    if (bpdu->sender_priority < stp->sw->priorite) return true;
    if (bpdu->sender_priority > stp->sw->priorite) return false;
    
    // Si même priorité, comparer par MAC de l'expéditeur
    return comparer_mac(bpdu->sender_mac, stp->sw->mac) < 0;
}

// Traite un BPDU reçu sur un port
void traiter_bpdu(switch_stp_t *stp, int port, bpdu_t *bpdu)
{
    if (!stp || !bpdu || port < 0 || port >= stp->sw->nb_ports) {
        return;
    }
    
    // Ajouter le coût du lien au coût annoncé
    bpdu_t bpdu_avec_cout = *bpdu;
    bpdu_avec_cout.root_cost += stp->sw->ports[port].cost;
    
    if (bpdu_est_meilleur(stp, &bpdu_avec_cout)) {
        // Mettre à jour les informations de racine
        stp->root_mac = bpdu->root_mac;
        stp->root_priority = bpdu->root_priority;
        stp->root_cost = bpdu_avec_cout.root_cost;
        stp->root_port = port;
        
        // Le port devient port racine
        set_port_role(stp->sw, port, PORT_ROOT);
        set_port_state(stp->sw, port, PORT_FORWARDING);
        
        // Recalculer les rôles des autres ports
        recalculer_roles_ports(stp);
    }
}

// Recalcule les rôles de tous les ports
void recalculer_roles_ports(switch_stp_t *stp)
{
    if (!stp || !stp->sw) return;
    
    // Si ce switch est la racine
    if (mac_equals(stp->root_mac, stp->sw->mac) && stp->root_priority == stp->sw->priorite) {
        stp->root_port = -1;
        // Tous les ports sont designated
        for (int i = 0; i < stp->sw->nb_ports; i++) {
            set_port_role(stp->sw, i, PORT_DESIGNATED);
            set_port_state(stp->sw, i, PORT_FORWARDING);
        }
        return;
    }
    
    // Pour les autres switches
    for (int i = 0; i < stp->sw->nb_ports; i++) {
        if (i == stp->root_port) {
            // Port racine déjà configuré
            continue;
        }
        
        // Par défaut, bloquer les autres ports
        // Dans une implémentation complète, on comparerait avec les BPDUs reçus
        set_port_role(stp->sw, i, PORT_BLOCKED);
        set_port_state(stp->sw, i, PORT_BLOCKING);
    }
}

// Crée un BPDU à envoyer
bpdu_t creer_bpdu(switch_stp_t *stp)
{
    bpdu_t bpdu;
    bpdu.root_mac = stp->root_mac;
    bpdu.root_priority = stp->root_priority;
    bpdu.root_cost = stp->root_cost;
    bpdu.sender_mac = stp->sw->mac;
    bpdu.sender_priority = stp->sw->priorite;
    return bpdu;
}

// Calcule le STP pour un ensemble de switches connectés
void calculer_stp_simple(switch_stp_t switches[], size_t nb_switches, graphe const *g)
{
    if (!switches || nb_switches == 0 || !g) {
        return;
    }
    
    // Phase 1: Trouver la racine
    size_t racine_idx = 0;
    for (size_t i = 1; i < nb_switches; i++) {
        if (switches[i].sw->priorite < switches[racine_idx].sw->priorite ||
            (switches[i].sw->priorite == switches[racine_idx].sw->priorite && 
             comparer_mac(switches[i].sw->mac, switches[racine_idx].sw->mac) < 0)) {
            racine_idx = i;
        }
    }
    
    // Phase 2: Configurer la racine
    switches[racine_idx].root_mac = switches[racine_idx].sw->mac;
    switches[racine_idx].root_priority = switches[racine_idx].sw->priorite;
    switches[racine_idx].root_cost = 0;
    switches[racine_idx].root_port = -1;
    
    for (int p = 0; p < switches[racine_idx].sw->nb_ports; p++) {
        set_port_role(switches[racine_idx].sw, p, PORT_DESIGNATED);
        set_port_state(switches[racine_idx].sw, p, PORT_FORWARDING);
    }
    
    // Phase 3: Simuler la propagation des BPDUs
    bool changement = true;
    int iterations = 0;
    const int max_iterations = 10; // Éviter les boucles infinies
    
    while (changement && iterations < max_iterations) {
        changement = false;
        iterations++;
        
        // Pour chaque arête dans le graphe
        for (size_t a = 0; a < nb_aretes(g); a++) {
            sommet s1 = g->aretes[a].s1;
            sommet s2 = g->aretes[a].s2;
            
            if (s1 >= nb_switches || s2 >= nb_switches) continue;
            
            // Créer les BPDUs
            bpdu_t bpdu1 = creer_bpdu(&switches[s1]);
            bpdu_t bpdu2 = creer_bpdu(&switches[s2]);
            
            // Simuler l'envoi de s1 vers s2
            switch_stp_t ancien_s2 = switches[s2];
            traiter_bpdu(&switches[s2], 0, &bpdu1); // Port 0 simplifié
            if (memcmp(&ancien_s2, &switches[s2], sizeof(switch_stp_t)) != 0) {
                changement = true;
            }
            
            // Simuler l'envoi de s2 vers s1
            switch_stp_t ancien_s1 = switches[s1];
            traiter_bpdu(&switches[s1], 0, &bpdu2); // Port 0 simplifié
            if (memcmp(&ancien_s1, &switches[s1], sizeof(switch_stp_t)) != 0) {
                changement = true;
            }
        }
    }
    
    // Phase 4: Finaliser les rôles des ports
    for (size_t i = 0; i < nb_switches; i++) {
        recalculer_roles_ports(&switches[i]);
    }
}

// Affiche l'état STP d'un switch
void afficher_etat_stp(switch_stp_t *stp)
{
    if (!stp || !stp->sw) return;
    
    printf("=== État STP du Switch ===\n");
    printf("Switch MAC: ");
    afficher_mac(stp->sw->mac);
    printf("Priorité: %d\n", stp->sw->priorite);
    printf("Racine MAC: ");
    afficher_mac(stp->root_mac);
    printf("Racine priorité: %d\n", stp->root_priority);
    printf("Coût vers racine: %d\n", stp->root_cost);
    printf("Port racine: %d\n", stp->root_port);
    
    printf("États des ports:\n");
    for (int i = 0; i < stp->sw->nb_ports; i++) {
        printf("  Port %d: %s - %s\n", i,
               port_role_to_string(get_port_role(stp->sw, i)),
               port_state_to_string(get_port_state(stp->sw, i)));
    }
    printf("========================\n");
}