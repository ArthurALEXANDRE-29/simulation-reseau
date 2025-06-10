#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "adresse.h"
#include "station.h"
#include "switch.h"
#include "trame.h"
#include "graphe.h"
#include "configuration.h"

// Structure pour représenter une station de tramway
typedef struct {
    char nom[64];
    station_t equipement_reseau;
    int ligne_principale;      // Ligne principale (A=1, B=2, C=3, D=4, E=5, F=6)
    bool interconnection;      // Station d'interconnexion entre lignes
    int lignes_secondaires[3]; // Autres lignes qui passent par cette station
} station_tramway_t;

// Structure pour représenter le réseau complet
typedef struct {
    station_tramway_t stations[200];  // Maximum 200 stations
    switch_t switchs[50];            // Switchs pour interconnecter les lignes
    graphe reseau;
    int nb_stations;
    int nb_switchs;
} reseau_tramway_t;

// Fonction pour créer une adresse MAC basée sur la ligne et la position
mac_addr_t generer_mac_station(int ligne, int position) {
    return creer_mac(0x02, 0x00, 0x67, ligne, position >> 8, position & 0xFF);
}

// Fonction pour créer une adresse IP basée sur la ligne et la position
ip_addr_t generer_ip_station(int ligne, int position) {
    return creer_ip(192, 168, ligne, position);
}

// Initialisation du réseau de tramway de Strasbourg
void init_reseau_tramway(reseau_tramway_t *reseau) {
    init_graphe(&reseau->reseau);
    reseau->nb_stations = 0;
    reseau->nb_switchs = 0;
    
    // LIGNE A (Parc des Sports ↔ Illkirch-Graffenstaden)
    char* stations_ligne_a[] = {
        "Parc des Sports", "Le Galet", "Cervantès", "Dante", "Hôpital de Hautepierre",
        "Ducs d'Alsace", "Saint-Florent", "Rotonde", "Gare Centrale", "Ancienne Synagogue Les Halles",
        "Homme de Fer", "Langstross Grand Rue", "Porte de l'Hôpital", "Étoile Bourse", "Étoile Polygone",
        "Schluthfeld", "Krimmeri Stade de la Meinau", "Émile Mathis", "Hohwart", "Baggersee",
        "Colonne", "Leclerc", "Campus d'Illkirch", "Illkirch Lixenbuhl", "Illkirch Graffenstaden"
    };
    
    // LIGNE B (Hoenheim Gare ↔ Kehl Bahnhof)
    char* stations_ligne_b[] = {
        "Hoenheim Gare", "Futura Glacière", "Faubourg National", "Faubourg de Saverne", "République",
        "Gallia", "Université", "Observatoire", "Esplanade", "Winston Churchill", "Landsberg",
        "Jean Jaurès", "Lycée Jean Monnet", "Gravière", "Kibitzenau", "Saint-Christophe",
        "Neuhof Rodolphe Reuss", "Kehl Bahnhof"
    };
    
    // LIGNE C (Neuhof Rodolphe Reuss ↔ Gare Centrale)
    char* stations_ligne_c[] = {
        "Neuhof Rodolphe Reuss", "Kibitzenau", "Gravière", "Lycée Jean Monnet", "Jean Jaurès",
        "Landsberg", "Winston Churchill", "Esplanade", "Observatoire", "Université", "Gallia",
        "République", "Faubourg de Saverne", "Faubourg National", "Gare Centrale"
    };
    
    // LIGNE D (Poteries ↔ Kehl Rathaus)
    char* stations_ligne_d[] = {
        "Poteries", "Marcel Rudloff", "Paul Éluard", "Dante", "Hôpital de Hautepierre",
        "Ducs d'Alsace", "Saint-Florent", "Rotonde", "Gare Centrale", "Ancienne Synagogue Les Halles",
        "Homme de Fer", "Langstross Grand Rue", "Porte de l'Hôpital", "Étoile Bourse", "Étoile Polygone",
        "Landsberg", "Jean Jaurès", "Aristide Briand", "Citadelle", "Starcoop", "Port du Rhin",
        "Kehl Rathaus"
    };
    
    // LIGNE E (Campus d'Illkirch ↔ Robertsau L'Escale)
    char* stations_ligne_e[] = {
        "Campus d'Illkirch", "Illkirch Lixenbuhl", "Illkirch Graffenstaden", "Leclerc", "Colonne",
        "Baggersee", "Émile Mathis", "Hohwart", "Krimmeri Stade de la Meinau", "Schluthfeld",
        "Étoile Polygone", "Landsberg", "Winston Churchill", "Esplanade", "Observatoire", "Université",
        "Gallia", "République", "Parc du Contades", "Lycee Kléber", "Wacken", "Parlement Europeen",
        "Droit de l'Homme", "Robertsau L'Escale"
    };
    
    // LIGNE F (Comtes ↔ Place d'Islande)
    char* stations_ligne_f[] = {
        "Comtes", "Place d'Islande", "Observatoire", "Université", "Gallia", "République",
        "Broglie", "Homme de Fer", "Alt Winmärik – Vieux Marché aux Vins", "Faubourg National",
        "Porte Blanche", "Parc des Romains", "Comtes"
    };
    
    int position = 1;
    
    // Ajout des stations de la ligne A
    for (int i = 0; i < 25; i++) {
        strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_a[i]);
        reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
            generer_mac_station(1, position),
            generer_ip_station(1, position)
        );
        reseau->stations[reseau->nb_stations].ligne_principale = 1;
        reseau->stations[reseau->nb_stations].interconnection = false;
        
        // Initialiser les lignes secondaires à -1 (aucune ligne)
        for (int j = 0; j < 3; j++) {
            reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
        }
        
        // Marquer les stations d'interconnexion et leurs lignes secondaires
        if (strcmp(stations_ligne_a[i], "Homme de Fer") == 0) {
            reseau->stations[reseau->nb_stations].interconnection = true;
            reseau->stations[reseau->nb_stations].lignes_secondaires[0] = 2; // Ligne B
            reseau->stations[reseau->nb_stations].lignes_secondaires[1] = 3; // Ligne C
            reseau->stations[reseau->nb_stations].lignes_secondaires[2] = 4; // Ligne D
        }
        else if (strcmp(stations_ligne_a[i], "Gare Centrale") == 0) {
            reseau->stations[reseau->nb_stations].interconnection = true;
            reseau->stations[reseau->nb_stations].lignes_secondaires[0] = 2; // Ligne B
            reseau->stations[reseau->nb_stations].lignes_secondaires[1] = 3; // Ligne C
            reseau->stations[reseau->nb_stations].lignes_secondaires[2] = 4; // Ligne D
        }
        else if (strcmp(stations_ligne_a[i], "Université") == 0) {
            reseau->stations[reseau->nb_stations].interconnection = true;
            reseau->stations[reseau->nb_stations].lignes_secondaires[0] = 2; // Ligne B
            reseau->stations[reseau->nb_stations].lignes_secondaires[1] = 3; // Ligne C
            reseau->stations[reseau->nb_stations].lignes_secondaires[2] = 6; // Ligne F
        }
        else if (strcmp(stations_ligne_a[i], "Étoile Bourse") == 0) {
            reseau->stations[reseau->nb_stations].interconnection = true;
            reseau->stations[reseau->nb_stations].lignes_secondaires[0] = 2; // Ligne B
            reseau->stations[reseau->nb_stations].lignes_secondaires[1] = 4; // Ligne D
            reseau->stations[reseau->nb_stations].lignes_secondaires[2] = 5; // Ligne E
        }
        else if (strcmp(stations_ligne_a[i], "République") == 0) {
            reseau->stations[reseau->nb_stations].interconnection = true;
            reseau->stations[reseau->nb_stations].lignes_secondaires[0] = 2; // Ligne B
            reseau->stations[reseau->nb_stations].lignes_secondaires[1] = 3; // Ligne C
            reseau->stations[reseau->nb_stations].lignes_secondaires[2] = 6; // Ligne F
        }
        
        ajouter_sommet(&reseau->reseau);
        reseau->nb_stations++;
        position++;
    }
    
    // Ajout des stations de la ligne B (en évitant les doublons)
    position = 1;
    for (int i = 0; i < 18; i++) {
        bool existe_deja = false;
        int index_existant = -1;
        
        for (int j = 0; j < reseau->nb_stations; j++) {
            if (strcmp(reseau->stations[j].nom, stations_ligne_b[i]) == 0) {
                existe_deja = true;
                index_existant = j;
                break;
            }
        }
        
        if (!existe_deja) {
            strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_b[i]);
            reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
                generer_mac_station(2, position),
                generer_ip_station(2, position)
            );
            reseau->stations[reseau->nb_stations].ligne_principale = 2;
            reseau->stations[reseau->nb_stations].interconnection = false;
            
            // Initialiser les lignes secondaires à -1 (aucune ligne)
            for (int j = 0; j < 3; j++) {
                reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
            }
            
            // Marquer les stations d'interconnexion et leurs lignes secondaires
            if (strcmp(stations_ligne_b[i], "Homme de Fer") == 0 ||
                strcmp(stations_ligne_b[i], "Gare Centrale") == 0 ||
                strcmp(stations_ligne_b[i], "Université") == 0 ||
                strcmp(stations_ligne_b[i], "Étoile Bourse") == 0 ||
                strcmp(stations_ligne_b[i], "République") == 0) {
                reseau->stations[reseau->nb_stations].interconnection = true;
                // Les lignes secondaires seront ajoutées lors de l'ajout des autres lignes
            }
            
            ajouter_sommet(&reseau->reseau);
            reseau->nb_stations++;
        } else {
            // Si la station existe déjà, ajouter la ligne B comme ligne secondaire
            for (int j = 0; j < 3; j++) {
                if (reseau->stations[index_existant].lignes_secondaires[j] == -1) {
                    reseau->stations[index_existant].lignes_secondaires[j] = 2;
                    break;
                }
            }
        }
        position++;
    }
    
    // Ajout des stations de la ligne C (en évitant les doublons)
    position = 1;
    for (int i = 0; i < 15; i++) {
        bool existe_deja = false;
        int index_existant = -1;
        
        for (int j = 0; j < reseau->nb_stations; j++) {
            if (strcmp(reseau->stations[j].nom, stations_ligne_c[i]) == 0) {
                existe_deja = true;
                index_existant = j;
                break;
            }
        }
        
        if (!existe_deja) {
            strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_c[i]);
            reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
                generer_mac_station(3, position),
                generer_ip_station(3, position)
            );
            reseau->stations[reseau->nb_stations].ligne_principale = 3;
            reseau->stations[reseau->nb_stations].interconnection = false;
            
            // Initialiser les lignes secondaires à -1 (aucune ligne)
            for (int j = 0; j < 3; j++) {
                reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
            }
            
            // Marquer les stations d'interconnexion
            if (strcmp(stations_ligne_c[i], "Homme de Fer") == 0 ||
                strcmp(stations_ligne_c[i], "Gare Centrale") == 0 ||
                strcmp(stations_ligne_c[i], "Université") == 0 ||
                strcmp(stations_ligne_c[i], "Étoile Bourse") == 0 ||
                strcmp(stations_ligne_c[i], "République") == 0) {
                reseau->stations[reseau->nb_stations].interconnection = true;
            }
            
            ajouter_sommet(&reseau->reseau);
            reseau->nb_stations++;
        } else {
            // Si la station existe déjà, ajouter la ligne C comme ligne secondaire
            for (int j = 0; j < 3; j++) {
                if (reseau->stations[index_existant].lignes_secondaires[j] == -1) {
                    reseau->stations[index_existant].lignes_secondaires[j] = 3;
                    break;
                }
            }
        }
        position++;
    }
    
    // Ajout des stations de la ligne D (en évitant les doublons)
    position = 1;
    for (int i = 0; i < 22; i++) {
        bool existe_deja = false;
        int index_existant = -1;
        
        for (int j = 0; j < reseau->nb_stations; j++) {
            if (strcmp(reseau->stations[j].nom, stations_ligne_d[i]) == 0) {
                existe_deja = true;
                index_existant = j;
                break;
            }
        }
        
        if (!existe_deja) {
            strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_d[i]);
            reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
                generer_mac_station(4, position),
                generer_ip_station(4, position)
            );
            reseau->stations[reseau->nb_stations].ligne_principale = 4;
            reseau->stations[reseau->nb_stations].interconnection = false;
            
            // Initialiser les lignes secondaires à -1 (aucune ligne)
            for (int j = 0; j < 3; j++) {
                reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
            }
            
            // Marquer les stations d'interconnexion
            if (strcmp(stations_ligne_d[i], "Homme de Fer") == 0 ||
                strcmp(stations_ligne_d[i], "Gare Centrale") == 0 ||
                strcmp(stations_ligne_d[i], "Université") == 0 ||
                strcmp(stations_ligne_d[i], "Étoile Bourse") == 0 ||
                strcmp(stations_ligne_d[i], "République") == 0) {
                reseau->stations[reseau->nb_stations].interconnection = true;
            }
            
            ajouter_sommet(&reseau->reseau);
            reseau->nb_stations++;
        } else {
            // Si la station existe déjà, ajouter la ligne D comme ligne secondaire
            for (int j = 0; j < 3; j++) {
                if (reseau->stations[index_existant].lignes_secondaires[j] == -1) {
                    reseau->stations[index_existant].lignes_secondaires[j] = 4;
                    break;
                }
            }
        }
        position++;
    }
    
    // Ajout des stations de la ligne E (en évitant les doublons)
    position = 1;
    for (int i = 0; i < 24; i++) {
        bool existe_deja = false;
        int index_existant = -1;
        
        for (int j = 0; j < reseau->nb_stations; j++) {
            if (strcmp(reseau->stations[j].nom, stations_ligne_e[i]) == 0) {
                existe_deja = true;
                index_existant = j;
                break;
            }
        }
        
        if (!existe_deja) {
            strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_e[i]);
            reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
                generer_mac_station(5, position),
                generer_ip_station(5, position)
            );
            reseau->stations[reseau->nb_stations].ligne_principale = 5;
            reseau->stations[reseau->nb_stations].interconnection = false;
            
            // Initialiser les lignes secondaires à -1 (aucune ligne)
            for (int j = 0; j < 3; j++) {
                reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
            }
            
            // Marquer les stations d'interconnexion
            if (strcmp(stations_ligne_e[i], "Homme de Fer") == 0 ||
                strcmp(stations_ligne_e[i], "Gare Centrale") == 0 ||
                strcmp(stations_ligne_e[i], "Université") == 0 ||
                strcmp(stations_ligne_e[i], "Étoile Bourse") == 0 ||
                strcmp(stations_ligne_e[i], "République") == 0) {
                reseau->stations[reseau->nb_stations].interconnection = true;
            }
            
            ajouter_sommet(&reseau->reseau);
            reseau->nb_stations++;
        } else {
            // Si la station existe déjà, ajouter la ligne E comme ligne secondaire
            for (int j = 0; j < 3; j++) {
                if (reseau->stations[index_existant].lignes_secondaires[j] == -1) {
                    reseau->stations[index_existant].lignes_secondaires[j] = 5;
                    break;
                }
            }
        }
        position++;
    }
    
    // Ajout des stations de la ligne F (en évitant les doublons)
    position = 1;
    for (int i = 0; i < 13; i++) {
        bool existe_deja = false;
        int index_existant = -1;
        
        for (int j = 0; j < reseau->nb_stations; j++) {
            if (strcmp(reseau->stations[j].nom, stations_ligne_f[i]) == 0) {
                existe_deja = true;
                index_existant = j;
                break;
            }
        }
        
        if (!existe_deja) {
            strcpy(reseau->stations[reseau->nb_stations].nom, stations_ligne_f[i]);
            reseau->stations[reseau->nb_stations].equipement_reseau = creer_station(
                generer_mac_station(6, position),
                generer_ip_station(6, position)
            );
            reseau->stations[reseau->nb_stations].ligne_principale = 6;
            reseau->stations[reseau->nb_stations].interconnection = false;
            
            // Initialiser les lignes secondaires à -1 (aucune ligne)
            for (int j = 0; j < 3; j++) {
                reseau->stations[reseau->nb_stations].lignes_secondaires[j] = -1;
            }
            
            // Marquer les stations d'interconnexion
            if (strcmp(stations_ligne_f[i], "Homme de Fer") == 0 ||
                strcmp(stations_ligne_f[i], "Gare Centrale") == 0 ||
                strcmp(stations_ligne_f[i], "Université") == 0 ||
                strcmp(stations_ligne_f[i], "Étoile Bourse") == 0 ||
                strcmp(stations_ligne_f[i], "République") == 0) {
                reseau->stations[reseau->nb_stations].interconnection = true;
            }
            
            ajouter_sommet(&reseau->reseau);
            reseau->nb_stations++;
        } else {
            // Si la station existe déjà, ajouter la ligne F comme ligne secondaire
            for (int j = 0; j < 3; j++) {
                if (reseau->stations[index_existant].lignes_secondaires[j] == -1) {
                    reseau->stations[index_existant].lignes_secondaires[j] = 6;
                    break;
                }
            }
        }
        position++;
    }
    
    // Ajout de switchs pour interconnecter les lignes aux stations majeures
    char* stations_switch[] = {
        "Homme de Fer – Langstross Grand Rue",
        "Gare Centrale",
        "Université",
        "Étoile Bourse",
        "République"
    };
    for (int i = 0; i < 5; i++) {
        reseau->switchs[reseau->nb_switchs] = creer_switch(
            generer_mac_station(10, i + 1),  // MAC spéciale pour les switchs
            8,  // 8 ports par switch
            i + 1  // Priorité
        );
        ajouter_sommet(&reseau->reseau);
        reseau->nb_switchs++;
    }
}

// Fonction pour connecter les stations sur chaque ligne
void connecter_lignes(reseau_tramway_t *reseau) {
    // Connecter les stations consécutives sur chaque ligne
    for (int i = 0; i < reseau->nb_stations - 1; i++) {
        // Connexion basée sur la proximité des adresses IP (même ligne)
        if (reseau->stations[i].equipement_reseau.ip.octet[2] == 
            reseau->stations[i+1].equipement_reseau.ip.octet[2]) {
            arete a = {i, i+1};
            ajouter_arete(&reseau->reseau, a);
        }
    }
    
    // Connecter les stations d'interconnexion aux switchs
    int switch_offset = reseau->nb_stations;
    
    // Trouver les stations d'interconnexion et les connecter aux switchs
    for (int i = 0; i < reseau->nb_stations; i++) {
        if (strcmp(reseau->stations[i].nom, "Homme de Fer") == 0) {
            arete a = {i, switch_offset};  // Connecter au premier switch
            ajouter_arete(&reseau->reseau, a);
        }
        if (strcmp(reseau->stations[i].nom, "République") == 0) {
            arete a = {i, switch_offset + 1};  // Connecter au deuxième switch
            ajouter_arete(&reseau->reseau, a);
        }
        if (strcmp(reseau->stations[i].nom, "Université") == 0) {
            arete a = {i, switch_offset + 2};  // Connecter au troisième switch
            ajouter_arete(&reseau->reseau, a);
        }
    }
    
    // Interconnecter les switchs entre eux pour former le backbone
    for (int i = 0; i < reseau->nb_switchs - 1; i++) {
        arete a = {switch_offset + i, switch_offset + i + 1};
        ajouter_arete(&reseau->reseau, a);
    }
}

// Fonction pour afficher le réseau
void afficher_reseau(const reseau_tramway_t *reseau) {
    printf("========== RÉSEAU DE TRAMWAY DE STRASBOURG ==========\n\n");
    printf("Nombre total de stations: %d\n", reseau->nb_stations);
    printf("Nombre de switchs d'interconnexion: %d\n", reseau->nb_switchs);
    printf("Nombre total d'équipements: %zu\n", ordre(&reseau->reseau));
    printf("Nombre de connexions: %zu\n\n", nb_aretes(&reseau->reseau));
    
    // Afficher les stations par ligne
    for (int ligne = 1; ligne <= 6; ligne++) {
        printf("=== LIGNE %c ===\n", 'A' + ligne - 1);
        for (int i = 0; i < reseau->nb_stations; i++) {
            if (reseau->stations[i].ligne_principale == ligne) {
                printf("Station: %-25s | MAC: ", reseau->stations[i].nom);
                afficher_mac(reseau->stations[i].equipement_reseau.mac);
                printf(" | IP: ");
                afficher_ip(reseau->stations[i].equipement_reseau.ip);
                
                // Afficher les lignes secondaires
                bool a_lignes_secondaires = false;
                for (int j = 0; j < 3; j++) {
                    if (reseau->stations[i].lignes_secondaires[j] != -1) {
                        if (!a_lignes_secondaires) {
                            printf(" | Lignes secondaires: ");
                            a_lignes_secondaires = true;
                        }
                        printf("%c ", 'A' + reseau->stations[i].lignes_secondaires[j] - 1);
                    }
                }
                
                if (reseau->stations[i].interconnection) {
                    printf(" [INTERCONNEXION]");
                }
                printf("\n");
            }
        }
        printf("\n");
    }
    
    // Afficher les switchs
    printf("=== SWITCHS D'INTERCONNEXION ===\n");
    for (int i = 0; i < reseau->nb_switchs; i++) {
        printf("Switch %d | MAC: ", i);
        afficher_mac(reseau->switchs[i].mac);
        printf(" | Ports: %d | Priorité: %d\n", 
               reseau->switchs[i].nb_ports, reseau->switchs[i].priorite);
    }
    printf("\n");
}

// Fonction de simulation de trafic
void simuler_trafic(reseau_tramway_t *reseau) {
    printf("=== SIMULATION DE TRAFIC SUR LE RÉSEAU ===\n\n");
    
    // Simuler quelques trames entre différentes stations
    if (reseau->nb_stations >= 10) {
        // Trame 1: De "Parc des Sports" vers "Homme de Fer"
        int station_src = 0;
        int station_dest = -1;
        
        // Trouver "Homme de Fer"
        for (int i = 0; i < reseau->nb_stations; i++) {
            if (strcmp(reseau->stations[i].nom, "Homme de Fer – Langstross Grand Rue") == 0) {
                station_dest = i;
                break;
            }
        }
        
        if (station_dest != -1) {
            printf("📡 Envoi d'une trame de '%s' vers 'Homme de Fer'\n", 
                   reseau->stations[station_src].nom);
            
            uint8_t donnees[] = "Info trafic ligne A";
            trame t;
            if (init_trame(&t, 
                          reseau->stations[station_src].equipement_reseau.mac,
                          reseau->stations[station_dest].equipement_reseau.mac,
                          TYPE_IPV4, donnees, strlen((char*)donnees))) {
                afficher_trame(&t);
                deinit_trame(&t);
            }
        }
        
        printf("\n");
        
        // Trame 2: Broadcast depuis "Gare Centrale"
        for (int i = 0; i < reseau->nb_stations; i++) {
            if (strcmp(reseau->stations[i].nom, "Gare Centrale") == 0) {
                printf("📡 Broadcast depuis 'Gare Centrale' - Annonce générale\n");
                
                MAC broadcast = creer_mac(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
                uint8_t donnees_bc[] = "Annonce: Service normal sur toutes les lignes";
                trame t_bc;
                if (init_trame(&t_bc, 
                              reseau->stations[i].equipement_reseau.mac,
                              broadcast,
                              TYPE_IPV4, donnees_bc, strlen((char*)donnees_bc))) {
                    afficher_trame(&t_bc);
                    deinit_trame(&t_bc);
                }
                break;
            }
        }
    }
}

// Fonction pour analyser la connectivité du réseau
void analyser_connectivite(const reseau_tramway_t *reseau) {
    printf("=== ANALYSE DE CONNECTIVITÉ ===\n\n");
    
    // Statistiques par ligne
    int stats_lignes[7] = {0}; // Index 0 inutilisé, 1-6 pour lignes A-F
    
    for (int i = 0; i < reseau->nb_stations; i++) {
        if (reseau->stations[i].ligne_principale >= 1 && 
            reseau->stations[i].ligne_principale <= 6) {
            stats_lignes[reseau->stations[i].ligne_principale]++;
        }
    }
    
    for (int ligne = 1; ligne <= 6; ligne++) {
        printf("Ligne %c: %d stations\n", 'A' + ligne - 1, stats_lignes[ligne]);
    }
    
    printf("\nStations d'interconnexion identifiées:\n");
    for (int i = 0; i < reseau->nb_stations; i++) {
        if (reseau->stations[i].interconnection) {
            printf("- %s\n", reseau->stations[i].nom);
        }
    }
    
    printf("\nCapacité totale du réseau:\n");
    printf("- Équipements réseau: %zu\n", ordre(&reseau->reseau));
    printf("- Connexions physiques: %zu\n", nb_aretes(&reseau->reseau));
    printf("- Switchs backbone: %d\n", reseau->nb_switchs);
}

// Fonction pour nettoyer le réseau
void deinit_reseau_tramway(reseau_tramway_t *reseau) {
    for (int i = 0; i < reseau->nb_switchs; i++) {
        deinit_switch(&reseau->switchs[i]);
    }
    deinit_graphe(&reseau->reseau);
}

int main() {
    printf("🚋 SIMULATION DU RÉSEAU DE TRAMWAY DE STRASBOURG 🚋\n");
    printf("====================================================\n\n");
    
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    // Création et initialisation du réseau
    reseau_tramway_t reseau_strasbourg;
    printf("Initialisation du réseau de tramway...\n");
    init_reseau_tramway(&reseau_strasbourg);
    
    printf("Connexion des lignes et stations...\n");
    connecter_lignes(&reseau_strasbourg);
    
    printf("Réseau initialisé avec succès!\n\n");
    
    // Affichage du réseau complet
    afficher_reseau(&reseau_strasbourg);
    
    // Analyse de la connectivité
    analyser_connectivite(&reseau_strasbourg);
    
    printf("\n");
    
    // Simulation de trafic
    simuler_trafic(&reseau_strasbourg);
    
    printf("\n=== TESTS DE FONCTIONNALITÉS ===\n\n");
    
    // Test des fonctions de recherche et manipulation
    printf("Test de recherche de station par nom:\n");
    char station_recherchee[] = "Homme de Fer";
    bool trouvee = false;
    for (int i = 0; i < reseau_strasbourg.nb_stations; i++) {
        if (strcmp(reseau_strasbourg.stations[i].nom, station_recherchee) == 0) {
            printf("✅ Station '%s' trouvée à l'index %d\n", station_recherchee, i);
            printf("   Ligne principale: %c\n", 'A' + reseau_strasbourg.stations[i].ligne_principale - 1);
            printf("   Adresse MAC: ");
            afficher_mac(reseau_strasbourg.stations[i].equipement_reseau.mac);
            printf("\n   Adresse IP: ");
            afficher_ip(reseau_strasbourg.stations[i].equipement_reseau.ip);
            printf("\n");
            trouvee = true;
            break;
        }
    }
    if (!trouvee) {
        printf("❌ Station '%s' non trouvée\n", station_recherchee);
    }
    
    printf("\nTest de connectivité entre stations adjacentes:\n");
    if (reseau_strasbourg.nb_stations >= 2) {
        arete test_arete = {0, 1};
        if (existe_arete(&reseau_strasbourg.reseau, test_arete)) {
            printf("✅ Connexion confirmée entre les stations 0 et 1\n");
        } else {
            printf("❌ Pas de connexion directe entre les stations 0 et 1\n");
        }
    }
    
    printf("\n=== RÉSUMÉ DE LA SIMULATION ===\n");
    printf("✅ Réseau de tramway de Strasbourg simulé avec succès\n");
    printf("✅ %d stations de tramway modélisées comme équipements réseau\n", reseau_strasbourg.nb_stations);
    printf("✅ %d switchs d'interconnexion déployés\n", reseau_strasbourg.nb_switchs);
    printf("✅ Topologie réseau: %zu nœuds, %zu connexions\n", 
           ordre(&reseau_strasbourg.reseau), nb_aretes(&reseau_strasbourg.reseau));
    printf("✅ Simulation de trafic effectuée\n");
    printf("✅ Tests de connectivité réalisés\n");
    
    // Nettoyage
    deinit_reseau_tramway(&reseau_strasbourg);
    
    printf("\n🎉 Simulation terminée avec succès! 🎉\n");
    
    return 0;
}