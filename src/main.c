#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "adresse.h"
#include "switch.h"
#include "station.h"
#include "stp.h"
#include "graphe.h"

/* ================== Couleurs / styles ================== */
#define C_RESET   "\033[0m"
#define C_TITLE   "\033[1;96m"
#define C_SECTION "\033[1;93m"
#define C_OK      "\033[1;92m"
#define C_INFO    "\033[0;94m"
#define C_WARN    "\033[1;95m"
#define C_DIM     "\033[2m"
#define C_FRAME   "\033[1;90m"

/* ================== Structures de simulation ================== */

typedef struct {
    station_t *station;
    switch_t  *sw;
    int        port;
} liaison_station_t;

typedef struct {
    switch_t *sw1;
    int       port1;
    switch_t *sw2;
    int       port2;
} lien_switch_t;

/* ================== Utilitaires ================== */

static void print_sep(const char *title) {
    printf("\n%s========== %s ==========%s\n", C_SECTION, title, C_RESET);
}

static void afficher_mac_inline(mac_addr_t mac, char *buf) {
    MAC_to_string(mac, buf);
}

/* Trouver station par MAC */
static station_t *trouver_station_par_mac(liaison_station_t *liaisons, size_t nb_liaisons, mac_addr_t mac) {
    for (size_t i = 0; i < nb_liaisons; ++i) {
        if (mac_equals(liaisons[i].station->mac, mac)) {
            return liaisons[i].station;
        }
    }
    return NULL;
}

/* Récupérer la liaison station pour port d'un switch */
static liaison_station_t *station_sur_port(liaison_station_t *liaisons, size_t nb_liaisons, switch_t *sw, int port) {
    for (size_t i = 0; i < nb_liaisons; ++i) {
        if (liaisons[i].sw == sw && liaisons[i].port == port) {
            return &liaisons[i];
        }
    }
    return NULL;
}

/* Donne les liens inter-switch pour un switch et port donné */
static lien_switch_t *trouver_lien(lien_switch_t *liens, size_t nb_liens, switch_t *sw, int port) {
    for (size_t i = 0; i < nb_liens; ++i) {
        if ((liens[i].sw1 == sw && liens[i].port1 == port) ||
            (liens[i].sw2 == sw && liens[i].port2 == port)) {
            return &liens[i];
        }
    }
    return NULL;
}

/* Obtenir le switch voisin via un lien */
static switch_t *voisin_via_lien(lien_switch_t *l, switch_t *courant, int *out_port_voisin) {
    if (l->sw1 == courant) {
        if (out_port_voisin) *out_port_voisin = l->port2;
        return l->sw2;
    } else if (l->sw2 == courant) {
        if (out_port_voisin) *out_port_voisin = l->port1;
        return l->sw1;
    }
    return NULL;
}

/* ================== Affichage tables MAC ================== */

static void afficher_tables_switches(switch_t **switches, size_t nb) {
    print_sep("Tables de commutation");
    for (size_t i = 0; i < nb; ++i) {
        printf("%s[Switch %zu]%s\n", C_INFO, i, C_RESET);
        afficher_table_commutation(&switches[i]->table);
    }
}

/* ================== Propagation / envoi de trame ================== */

typedef struct {
    mac_addr_t src;
    mac_addr_t dst;
    const char *payload;
    bool is_broadcast;
} trame_simulee_t;

/* Vérifie si port utilisable pour forwarding (état STP) */
static bool port_peut_transmettre(switch_t *sw, int port) {
    port_state_t st = get_port_state(sw, port);
    return (st == PORT_STATE_FORWARDING);
}

static void apprendre_source(switch_t *sw, mac_addr_t src_mac, int ingress_port) {
    ajouter_entree_table(&sw->table, src_mac, ingress_port);
}

/* Flood contrôlé : envoie sur tous les ports forwarding sauf port source */
static void flood_trame(switch_t *sw,
                        int ingress_port,
                        trame_simulee_t *tr,
                        lien_switch_t *liens, size_t nb_liens,
                        liaison_station_t *liaisons, size_t nb_liaisons,
                        switch_t **visite, size_t *nb_visite,
                        size_t profondeur)
{
    // Marquage visite
    for (size_t i = 0; i < *nb_visite; ++i) {
        if (visite[i] == sw) {
            return; // déjà traité
        }
    }
    visite[(*nb_visite)++] = sw;

    char buf_src[18], buf_dst[18];
    afficher_mac_inline(tr->src, buf_src);
    afficher_mac_inline(tr->dst, buf_dst);

    // Parcours des ports
    for (int p = 0; p < sw->nb_ports; ++p) {
        if (p == ingress_port) continue;
        if (!port_peut_transmettre(sw, p)) {
            continue;
        }

        // Station locale ?
        liaison_station_t *liaison = station_sur_port(liaisons, nb_liaisons, sw, p);
        if (liaison) {
            if (tr->is_broadcast) {
                printf("%s[BROADCAST]%s Delivery: Switch(", C_FRAME, C_RESET);
                afficher_mac(liaison->sw->mac);
                printf(") -> Station ");
                afficher_mac(liaison->station->mac);
                printf(" | \"%s\"\n", tr->payload);
            } else if (mac_equals(liaison->station->mac, tr->dst)) {
                printf("%s[DELIVER]%s Unicast Switch(", C_OK, C_RESET);
                afficher_mac(sw->mac);
                printf(") -> Station ");
                afficher_mac(liaison->station->mac);
                printf(" | \"%s\"\n", tr->payload);
            }
            continue;
        }

        // Lien inter-switch ?
        lien_switch_t *l = trouver_lien(liens, nb_liens, sw, p);
        if (!l) continue;

        int port_voisin = -1;
        switch_t *sw_voisin = voisin_via_lien(l, sw, &port_voisin);
        if (!sw_voisin) continue;

        printf("%s[FWD]%s %s -> %s via port %d -> switch ",
               C_INFO, C_RESET, buf_src, tr->is_broadcast ? "FF:FF:FF:FF:FF:FF" : buf_dst, p);
        afficher_mac(sw_voisin->mac);
        printf("\n");

        // Recherche unicast possible sur le voisin ?
        if (!tr->is_broadcast) {
            int port_cible = chercher_port_mac(&sw_voisin->table, tr->dst);
            if (port_cible >= 0 && port_peut_transmettre(sw_voisin, port_cible)) {
                // Apprendre la source sur le voisin avant forwarding
                apprendre_source(sw_voisin, tr->src, port_voisin);
                liaison_station_t *ls = station_sur_port(liaisons, nb_liaisons, sw_voisin, port_cible);
                if (ls && mac_equals(ls->station->mac, tr->dst)) {
                    printf("%s[DELIVER]%s Switch ", C_OK, C_RESET);
                    afficher_mac(sw_voisin->mac);
                    printf(" -> Station ");
                    afficher_mac(ls->station->mac);
                    printf(" | \"%s\"\n", tr->payload);
                    continue; // Pas besoin de flood supplémentaire
                }
            }
        }

        // Propagation (broadcast ou unicast inconnu)
        flood_trame(sw_voisin, port_voisin, tr, liens, nb_liens, liaisons, nb_liaisons, visite, nb_visite, profondeur + 1);
    }
}

static void envoyer_trame(switch_t *point_entree, int port_entree,
                          station_t *src_station,
                          trame_simulee_t *tr,
                          lien_switch_t *liens, size_t nb_liens,
                          liaison_station_t *liaisons, size_t nb_liaisons)
{
    char buf_src[18], buf_dst[18];
    afficher_mac_inline(tr->src, buf_src);
    afficher_mac_inline(tr->dst, buf_dst);

    printf("\n%s=== Envoi de trame ===%s\n", C_TITLE, C_RESET);
    printf("Source Station: "); afficher_mac(src_station->mac);
    printf(" -> Dest: %s  | Payload: \"%s\"\n",
           tr->is_broadcast ? "FF:FF:FF:FF:FF:FF" : buf_dst,
           tr->payload);

    // MAC learning de la source au point d'entrée
    apprendre_source(point_entree, tr->src, port_entree);

    // Vérifier si unicast connu
    bool connu = false;
    int port_cible = -1;
    if (!tr->is_broadcast) {
        port_cible = chercher_port_mac(&point_entree->table, tr->dst);
        if (port_cible >= 0 && port_peut_transmettre(point_entree, port_cible)) {
            connu = true;
        }
    }

    if (connu) {
        printf("%s[UNICAST]%s Chemin direct trouvé (port %d)\n", C_OK, C_RESET, port_cible);
        liaison_station_t *ls = station_sur_port(liaisons, nb_liaisons, point_entree, port_cible);
        if (ls && mac_equals(ls->station->mac, tr->dst)) {
            printf("%s[DELIVER]%s Local delivery on same switch: ", C_OK, C_RESET);
            afficher_mac(ls->station->mac);
            printf("\n");
            return;
        }
        // Sinon traverser le lien
    } else {
        printf("%s[LEARNING/FLOOD]%s Destination %s\n",
               C_WARN, C_RESET, tr->is_broadcast ? "BROADCAST" : "MAC inconnue -> FWD = FLOOD");
    }

    // Flood / unicast inconnu
    switch_t *visites[32];
    size_t nb_visite = 0;
    flood_trame(point_entree, port_entree, tr,
                liens, nb_liens,
                liaisons, nb_liaisons,
                visites, &nb_visite,
                0);
}

/* ================== Création topologie ================== */

static void construire_topologie_triangle(
    switch_t *sw1, switch_t *sw2, switch_t *sw3,
    lien_switch_t *liens, size_t *nb_liens_out)
{
    size_t n = 0;
    // S1 p0 <-> S2 p0
    liens[n++] = (lien_switch_t){ sw1, 0, sw2, 0 };
    // S2 p1 <-> S3 p0
    liens[n++] = (lien_switch_t){ sw2, 1, sw3, 0 };
    // S3 p1 <-> S1 p1
    liens[n++] = (lien_switch_t){ sw3, 1, sw1, 1 };
    *nb_liens_out = n;
}

/* ================== Simulation ================== */

int main(void) {

    printf("%sSimulation réseau : STP + Commutation Ethernet%s\n", C_TITLE, C_RESET);

    /* 1. Création des switches */
    mac_addr_t mac_sw1 = creer_mac(0x00,0x10,0x00,0x00,0x00,0x01);
    mac_addr_t mac_sw2 = creer_mac(0x00,0x10,0x00,0x00,0x00,0x02);
    mac_addr_t mac_sw3 = creer_mac(0x00,0x10,0x00,0x00,0x00,0x03);

    switch_t sw1 = creer_switch(mac_sw1, 3, 4096);   // plus basse priorité -> racine
    switch_t sw2 = creer_switch(mac_sw2, 3, 8192);
    switch_t sw3 = creer_switch(mac_sw3, 3, 12288);

    activer_port(&sw1, 0); activer_port(&sw1, 1); activer_port(&sw1, 2);
    activer_port(&sw2, 0); activer_port(&sw2, 1); activer_port(&sw2, 2);
    activer_port(&sw3, 0); activer_port(&sw3, 1); activer_port(&sw3, 2);

    /* 2. Topologie (liens inter-switch) */
    lien_switch_t liens[8];
    size_t nb_liens = 0;
    construire_topologie_triangle(&sw1, &sw2, &sw3, liens, &nb_liens);

    /* 3. Stations */
    station_t staA = creer_station(creer_mac(0x00,0xAA,0x00,0x00,0x00,0x01), creer_ip(192,168,1,10));
    station_t staB = creer_station(creer_mac(0x00,0xAA,0x00,0x00,0x00,0x02), creer_ip(192,168,1,11));
    station_t staC = creer_station(creer_mac(0x00,0xAA,0x00,0x00,0x00,0x03), creer_ip(192,168,1,12));

    liaison_station_t liaisons[] = {
        { &staA, &sw1, 2 },
        { &staB, &sw2, 2 },
        { &staC, &sw3, 2 }
    };
    size_t nb_liaisons = sizeof(liaisons)/sizeof(liaisons[0]);

    /* 4. Initialisation STP */
    switch_stp_t stp_ctx[3];
    init_stp(&stp_ctx[0], &sw1);
    init_stp(&stp_ctx[1], &sw2);
    init_stp(&stp_ctx[2], &sw3);

    /* Construire graphe pour ton calcul STP */
    graphe g;
    init_graphe(&g);
    // 3 sommets = 3 switches
    ajouter_sommet(&g);
    ajouter_sommet(&g);
    ajouter_sommet(&g);
    // Arêtes triangle
    ajouter_arete(&g, (arete){0,1});
    ajouter_arete(&g, (arete){1,2});
    ajouter_arete(&g, (arete){2,0});

    print_sep("Calcul STP");
    calculer_stp_simple(stp_ctx, 3, &g);

    for (int i = 0; i < 3; ++i) {
        printf("%sSwitch %d STP:%s\n", C_INFO, i, C_RESET);
        afficher_etat_stp(&stp_ctx[i]);
    }

    /* 5. Simulation d'envoi de trames */

    trame_simulee_t t1 = { staA.mac, staB.mac, "Bonjour B, ici A", false };
    envoyer_trame(&sw1, 2, &staA, &t1, liens, nb_liens, liaisons, nb_liaisons);

    afficher_tables_switches((switch_t*[]){&sw1,&sw2,&sw3}, 3);

    trame_simulee_t t2 = { staB.mac, staA.mac, "Réponse A, bien reçu", false };
    envoyer_trame(&sw2, 2, &staB, &t2, liens, nb_liens, liaisons, nb_liaisons);

    afficher_tables_switches((switch_t*[]){&sw1,&sw2,&sw3}, 3);

    trame_simulee_t t3 = { staC.mac, creer_mac(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF), "Annonce réseau (broadcast)", true };
    envoyer_trame(&sw3, 2, &staC, &t3, liens, nb_liens, liaisons, nb_liaisons);

    trame_simulee_t t4 = { staA.mac, staC.mac, "Salut C !", false };
    envoyer_trame(&sw1, 2, &staA, &t4, liens, nb_liens, liaisons, nb_liaisons);

    afficher_tables_switches((switch_t*[]){&sw1,&sw2,&sw3}, 3);

    /* 6. Nettoyage */
    deinit_graphe(&g);
    deinit_switch(&sw1);
    deinit_switch(&sw2);
    deinit_switch(&sw3);

    print_sep("Fin simulation");
    return 0;
}