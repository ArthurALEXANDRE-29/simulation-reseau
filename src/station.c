#include <stdio.h>
#include "station.h"

void afficher_station(station_t s) {
    printf("Station - MAC: ");
    afficher_MAC(&s.mac);
    printf(" | IP: ");
    afficher_IPv4(&s.ip);
    printf("\n");
}
