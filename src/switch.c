#include "switch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Initialise une table de commutation vide
 * Cette table associera les adresses MAC aux ports du switch
 */
void init_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

/**
 * Libère la mémoire de la table de commutation
 * Évite les fuites mémoire
 */
void deinit_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    
    free(table->entrees);
    table->entrees = NULL;
    table->taille = 0;
    table->capacite = 0;
}

/**
 * Ajoute ou met à jour une entrée MAC→Port dans la table
 * Le switch "apprend" ainsi où se trouvent les équipements
 */
bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port) {
    if (table == NULL || port < 0) return false;
    
    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac)) {
            table->entrees[i].port = port;
            return true;
        }
    }
    
    if (table->taille >= table->capacite) {
        int nouvelle_capacite = (table->capacite == 0) ? 8 : table->capacite * 2;
        table_entree_t *nouvelles_entrees = realloc(table->entrees, 
                                                     nouvelle_capacite * sizeof(table_entree_t));
        if (nouvelles_entrees == NULL) return false;
        
        table->entrees = nouvelles_entrees;
        table->capacite = nouvelle_capacite;
    }
    
    table->entrees[table->taille].mac = mac;
    table->entrees[table->taille].port = port;
    table->taille++;
    
    return true;
}

/**
 * Trouve sur quel port se trouve une adresse MAC
 * Permet au switch de diriger les trames vers le bon port
 */
int chercher_port_mac(const table_commutation_t *table, mac_addr_t mac) {
    if (table == NULL) return -1;
    
    for (int i = 0; i < table->taille; i++) {
        if (mac_equals(table->entrees[i].mac, mac)) {
            return table->entrees[i].port;
        }
    }
    
    return -1; 
}

/**
 * Vide la table de commutation
 * Utile pour redémarrer l'apprentissage ou en cas de changement de topologie
 */
void vider_table_commutation(table_commutation_t *table) {
    if (table == NULL) return;
    table->taille = 0;
}

/**
 * Affiche le contenu de la table de commutation
 * Essentiel pour le débogage et la supervision
 */
void afficher_table_commutation(const table_commutation_t *table) {
    if (table == NULL) {
        printf("Table de commutation: NULL\n");
        return;
    }
    
    printf("Table de commutation (%d entrées):\n", table->taille);
    if (table->taille == 0) {
        printf("  (vide)\n");
        return;
    }
    
    for (int i = 0; i < table->taille; i++) {
        printf("  - MAC: ");
        afficher_mac(table->entrees[i].mac);
        printf(" -> Port: %d\n", table->entrees[i].port);
    }
}

/**
 * Initialise un switch avec des valeurs par défaut
 * Prépare la structure avant configuration
 */
void init_switch(switch_t *sw) {
    if (sw == NULL) return;
    
    init_mac(&sw->mac);
    sw->nb_ports = 0;
    sw->priorite = 0;
    init_table_commutation(&sw->table);
    sw->ports = NULL;
}

/**
 * Libère toute la mémoire associée au switch (table, ports)
 * Nettoyage en fin de vie
 */
void deinit_switch(switch_t *sw) {
    if (sw == NULL) return;
    
    deinit_table_commutation(&sw->table);
    free(sw->ports);
    sw->ports = NULL;
    sw->nb_ports = 0;
}

/**
 * Crée un switch complet avec ses caractéristiques
 * Chaque switch a une MAC unique, un nombre de ports et une priorité STP
 */
switch_t creer_switch(mac_addr_t mac, int nb_ports, int priorite) {
    switch_t sw;
    sw.mac = mac;
    sw.nb_ports = nb_ports;
    sw.priorite = priorite;
    init_table_commutation(&sw.table);
    
    sw.ports = malloc(nb_ports * sizeof(port_info_t));
    if (sw.ports != NULL) {
        for (int i = 0; i < nb_ports; i++) {
            sw.ports[i].state = PORT_DISABLED;
            sw.ports[i].role = PORT_UNKNOWN;
            sw.ports[i].actif = false;
            sw.ports[i].cost = 1; // Coût par défaut
        }
    }
    
    return sw;
}

/**
 * Affiche toutes les informations du switch (MAC, ports, table)
 * Indispensable pour le diagnostic
 */
void afficher_switch(const switch_t *sw) {
    if (sw == NULL) {
        printf("Switch: NULL\n");
        return;
    }
    
    printf("Switch - MAC: ");
    afficher_mac(sw->mac);
    printf(" | Ports: %d | Priorité: %d\n", sw->nb_ports, sw->priorite);
    
    if (sw->ports != NULL) {
        printf("État des ports:\n");
        for (int i = 0; i < sw->nb_ports; i++) {
            printf("  Port %d: %s/%s (coût: %d, %s)\n", i, 
                   port_state_to_string(sw->ports[i].state),
                   port_role_to_string(sw->ports[i].role),
                   sw->ports[i].cost,
                   sw->ports[i].actif ? "actif" : "inactif");
        }
    }
    
    afficher_table_commutation(&sw->table);
}

/**
 * Initialise tous les ports du switch à l'état désactivé
 * État par défaut avant configuration STP
 */
void init_ports(switch_t *sw) {
    if (sw == NULL || sw->nb_ports <= 0) return;
    
    sw->ports = malloc(sw->nb_ports * sizeof(port_info_t));
    if (sw->ports != NULL) {
        for (int i = 0; i < sw->nb_ports; i++) {
            sw->ports[i].state = PORT_DISABLED;
            sw->ports[i].role = PORT_UNKNOWN;
            sw->ports[i].actif = false;
            sw->ports[i].cost = 1; // Coût par défaut
        }
    }
}

/**
 * Change l'état d'un port (DISABLED, BLOCKING, LISTENING, LEARNING, FORWARDING)
 * Essentiel pour STP
 */
void set_port_state(switch_t *sw, int port, port_state_t state) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].state = state;
}

/**
 * Récupère l'état actuel d'un port
 * Permet de vérifier si un port peut transmettre des données
 */
port_state_t get_port_state(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return PORT_DISABLED;
    }
    
    return sw->ports[port].state;
}

/**
 * Définit le rôle d'un port dans STP (ROOT, DESIGNATED, BLOCKED)
 * Détermine le comportement du port
 */
void set_port_role(switch_t *sw, int port, port_role_t role) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].role = role;
}

/**
 * Récupère le rôle STP d'un port
 * Indique la fonction du port dans l'arbre de recouvrement
 */
port_role_t get_port_role(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return PORT_UNKNOWN;
    }
    
    return sw->ports[port].role;
}

/**
 * Active un port pour qu'il puisse transmettre des données
 * Rend le port opérationnel
 */
void activer_port(switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].actif = true;
}

/**
 * Désactive un port, bloquant tout trafic
 * Utile pour maintenance ou isolation
 */
void desactiver_port(switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return;
    }
    
    sw->ports[port].actif = false;
}

/**
 * Vérifie si un port est actif
 * Détermine si le port peut être utilisé pour la transmission
 */
bool port_est_actif(const switch_t *sw, int port) {
    if (sw == NULL || sw->ports == NULL || port < 0 || port >= sw->nb_ports) {
        return false;
    }
    
    return sw->ports[port].actif;
}

/**
 * Compare deux switches pour égalité
 * Permet d'identifier de manière unique chaque commutateur
 */
bool switch_equals(const switch_t *s1, const switch_t *s2) {
    if (s1 == NULL || s2 == NULL) return false;
    
    return mac_equals(s1->mac, s2->mac) && 
           s1->nb_ports == s2->nb_ports && 
           s1->priorite == s2->priorite;
}

/**
 * Convertit un état de port en chaîne lisible
 * Facilite l'affichage et le débogage
 */
const char* port_state_to_string(port_state_t state) {
    switch (state) {
        case PORT_DISABLED: return "DISABLED";
        case PORT_BLOCKING: return "BLOCKING";
        case PORT_LISTENING: return "LISTENING";
        case PORT_LEARNING: return "LEARNING";
        case PORT_FORWARDING: return "FORWARDING";
        default: return "UNKNOWN";
    }
}

/**
 * Convertit un rôle de port en chaîne lisible
 * Aide à comprendre le comportement STP
 */
const char* port_role_to_string(port_role_t role) {
    switch (role) {
        case PORT_UNKNOWN: return "UNKNOWN";
        case PORT_ROOT: return "ROOT";
        case PORT_DESIGNATED: return "DESIGNATED";
        case PORT_BLOCKED: return "BLOCKED";
        default: return "INVALID";
    }
}
