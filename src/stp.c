#include "stp.h"
#include <stdio.h>
#include <string.h>

/* Implémentation STP (version que tu as fournie, retouchée légèrement) */

void init_stp(switch_stp_t *stp, switch_t *sw) 
{
    if (!stp || !sw) return;

    stp->root_mac      = sw->mac;
    stp->root_priority = sw->priorite;
    stp->root_cost     = 0;
    stp->root_port     = -1;
    stp->sw            = sw;

    for (size_t i = 0; i < (size_t)sw->nb_ports; i++) {
        set_port_state(sw, (int)i, PORT_STATE_BLOCKING);
        set_port_role(sw, (int)i, PORT_ROLE_UNKNOWN);
    }
}

void deinit_stp(switch_stp_t *stp) 
{
    if (!stp) return;
    if (stp->sw) {
        for (size_t i = 0; i < (size_t)stp->sw->nb_ports; i++) {
            set_port_state(stp->sw, (int)i, PORT_STATE_DISABLED);
        }
    }
    stp->sw = NULL;
}

int comparer_mac(mac_addr_t mac1, mac_addr_t mac2)
{
    for (int i = 0; i < 6; i++) {
        if (mac1.octet[i] < mac2.octet[i]) return -1;
        if (mac1.octet[i] > mac2.octet[i]) return 1;
    }
    return 0;
}

bool bpdu_est_meilleur(switch_stp_t *stp, bpdu_t *bpdu)
{
    if (bpdu->root_priority < stp->root_priority) return true;
    if (bpdu->root_priority > stp->root_priority) return false;

    int cmp_root = comparer_mac(bpdu->root_mac, stp->root_mac);
    if (cmp_root < 0) return true;
    if (cmp_root > 0) return false;

    if (bpdu->root_cost < stp->root_cost) return true;
    if (bpdu->root_cost > stp->root_cost) return false;

    if (bpdu->sender_priority < stp->sw->priorite) return true;
    if (bpdu->sender_priority > stp->sw->priorite) return false;

    return comparer_mac(bpdu->sender_mac, stp->sw->mac) < 0;
}

void traiter_bpdu(switch_stp_t *stp, int port, bpdu_t *bpdu)
{
    if (!stp || !bpdu || port < 0 || port >= stp->sw->nb_ports) return;

    bpdu_t bpdu_avec_cout = *bpdu;
    bpdu_avec_cout.root_cost += stp->sw->ports[port].cost;

    if (bpdu_est_meilleur(stp, &bpdu_avec_cout)) {
        stp->root_mac      = bpdu->root_mac;
        stp->root_priority = bpdu->root_priority;
        stp->root_cost     = bpdu_avec_cout.root_cost;
        stp->root_port     = port;

        set_port_role(stp->sw, port, PORT_ROLE_ROOT);
        set_port_state(stp->sw, port, PORT_STATE_FORWARDING);

        recalculer_roles_ports(stp);
    }
}

void recalculer_roles_ports(switch_stp_t *stp)
{
    if (!stp || !stp->sw) return;

    if (mac_equals(stp->root_mac, stp->sw->mac) &&
        stp->root_priority == stp->sw->priorite) {
        stp->root_port = -1;
        for (size_t i = 0; i < (size_t)stp->sw->nb_ports; i++) {
            set_port_role(stp->sw, (int)i, PORT_ROLE_DESIGNATED);
            set_port_state(stp->sw, (int)i, PORT_STATE_FORWARDING);
        }
        return;
    }

    for (size_t i = 0; i < (size_t)stp->sw->nb_ports; i++) {
        if ((int)i == stp->root_port) continue;
        set_port_role(stp->sw, (int)i, PORT_ROLE_BLOCKED);
        set_port_state(stp->sw, (int)i, PORT_STATE_BLOCKING);
    }
}

bpdu_t creer_bpdu(switch_stp_t *stp)
{
    bpdu_t bpdu;
    bpdu.root_mac       = stp->root_mac;
    bpdu.root_priority  = stp->root_priority;
    bpdu.root_cost      = stp->root_cost;
    bpdu.sender_mac     = stp->sw->mac;
    bpdu.sender_priority= stp->sw->priorite;
    return bpdu;
}

void calculer_stp_simple(switch_stp_t switches[], size_t nb_switches, graphe const *g)
{
    if (!switches || nb_switches == 0 || !g) return;

    size_t racine_idx = 0;
    for (size_t i = 1; i < nb_switches; i++) {
        if (switches[i].sw->priorite < switches[racine_idx].sw->priorite ||
            (switches[i].sw->priorite == switches[racine_idx].sw->priorite &&
             comparer_mac(switches[i].sw->mac, switches[racine_idx].sw->mac) < 0)) {
            racine_idx = i;
        }
    }

    switches[racine_idx].root_mac      = switches[racine_idx].sw->mac;
    switches[racine_idx].root_priority = switches[racine_idx].sw->priorite;
    switches[racine_idx].root_cost     = 0;
    switches[racine_idx].root_port     = -1;

    for (size_t p = 0; p < (size_t)switches[racine_idx].sw->nb_ports; p++) {
        set_port_role(switches[racine_idx].sw, (int)p, PORT_ROLE_DESIGNATED);
        set_port_state(switches[racine_idx].sw, (int)p, PORT_STATE_FORWARDING);
    }

    bool changement = true;
    size_t iterations = 0;
    const size_t max_iterations = 10;

    while (changement && iterations < max_iterations) {
        changement = false;
        iterations++;

        for (size_t a = 0; a < g->nb_aretes; a++) {
            sommet s1 = g->aretes[a].s1;
            sommet s2 = g->aretes[a].s2;
            if (s1 >= nb_switches || s2 >= nb_switches) continue;

            bpdu_t bpdu1 = creer_bpdu(&switches[s1]);
            bpdu_t bpdu2 = creer_bpdu(&switches[s2]);

            switch_stp_t ancien_s2 = switches[s2];
            traiter_bpdu(&switches[s2], 0, &bpdu1);
            if (memcmp(&ancien_s2, &switches[s2], sizeof(switch_stp_t)) != 0)
                changement = true;

            switch_stp_t ancien_s1 = switches[s1];
            traiter_bpdu(&switches[s1], 0, &bpdu2);
            if (memcmp(&ancien_s1, &switches[s1], sizeof(switch_stp_t)) != 0)
                changement = true;
        }
    }

    for (size_t i = 0; i < nb_switches; i++) {
        recalculer_roles_ports(&switches[i]);
    }
}

void afficher_etat_stp(switch_stp_t *stp)
{
    if (!stp || !stp->sw) return;

    printf("=== État STP du Switch ===\n");
    printf("Switch MAC: "); afficher_mac(stp->sw->mac); printf("\n");
    printf("Priorité: %u\n", stp->sw->priorite);
    printf("Racine MAC: "); afficher_mac(stp->root_mac); printf("\n");
    printf("Racine priorité: %u\n", stp->root_priority);
    printf("Coût vers racine: %d\n", stp->root_cost);
    printf("Port racine: %d\n", stp->root_port);

    printf("États des ports:\n");
    for (size_t i = 0; i < (size_t)stp->sw->nb_ports; i++) {
        printf("  Port %zu: %s - %s (cost=%d)\n", 
               i,
               port_role_to_string(get_port_role(stp->sw, (int)i)),
               port_state_to_string(get_port_state(stp->sw, (int)i)),
               stp->sw->ports[i].cost);
    }
    printf("========================\n");
}