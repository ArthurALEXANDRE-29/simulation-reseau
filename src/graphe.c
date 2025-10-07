#include "graphe.h"

void init_graphe(graphe *g)
{
    if(g == NULL){
        return;
    }
    // initialise les champs internes du graphe g
    g->ordre  = 0;
    // - allocation d'un tableau d'arêtes de capacité initiale 8
    g->aretes_capacite = 8;
    g->aretes = malloc(g->aretes_capacite * sizeof(arete));
    // - le graphe ne contient initialement ni sommet ni arête
    g->nb_aretes = 0;
}

void deinit_graphe(graphe *g)
{
    if(g == NULL){
        return;
    }
    // libère la mémoire qui avait été allouée dans la fonction init_graphe
    free(g->aretes);
    g->aretes = NULL;
    // réinitialise les champs internes du graphe g
    g->ordre = 0;
    g->aretes_capacite = 0;
    g->nb_aretes = 0;
}

size_t ordre(graphe const *g)
{
    return g->ordre;
}

size_t nb_aretes(graphe const *g)
{
    return g->nb_aretes;
}

void ajouter_sommet(graphe *g)
{
    g->ordre += 1;
}

size_t index_sommet(graphe const *g, sommet s)
{
    // retourne l'index du sommet s dans le graphe g
    // la valeur UNKNOWN_INDEX si le sommet n'existe pas dans g
    if (s < g->ordre) {
        return s;
    }
    return UNKNOWN_INDEX;
}

// une fonction locale "static arete swap_sommets(arete a)" pourra être utile
static arete swap_sommets(arete a){
    arete areteInversee = {a.s2, a.s1};
    return areteInversee;
}
// cette fonction retourne une nouvelle arête dont les sommets sont les même que l'arête reçue mais inversés

bool existe_arete(graphe const *g, arete a)
{
    // retourne true si l'arête a est contenue dans le graphe g, false sinon
    for(size_t i = 0; i < g->nb_aretes; i++){
        if ((g->aretes[i].s1 == a.s1 && g->aretes[i].s2 == a.s2) || (g->aretes[i].s1 == a.s2 && g->aretes[i].s2 == a.s1)){
            return true;
    }
    }
    // /!\ l'arête (s1,s2) et l'arête (s2,s1) sont considérées équivalentes
    return false;
}

bool ajouter_arete(graphe *g, arete a)
{
    if (g == NULL)
        return false;
    // l'arête a n'est ajoutée que si les conditions suivantes sont remplies :
    //  - les sommets s1 et s2 de a existent dans g
    if (g == NULL)
        return false;
    
    // Vérifier que les sommets existent dans g
    if (a.s1 >= g->ordre || a.s2 >= g->ordre)
        return false;
    
    // Vérifier que les sommets sont distincts
    if (a.s1 == a.s2)
        return false;
    
    // Vérifier que l'arête n'existe pas déjà
    // (existe_arete vérifie déjà les deux sens)
    if(existe_arete(g, a)){
        return false;
    }
    // /!\ si la capacité actuelle du tableau d'arêtes n'est pas suffisante,
    // /!\ il faut le réallouer.
    // /!\ on peut par exemple doubler la capacité du tableau actuel.
    if(g->nb_aretes >= g->aretes_capacite){
        size_t nouvelle_capacite = (g->aretes_capacite == 0) ? 8 : g->aretes_capacite * 2;
        arete *nouveau_tableau = (arete *)realloc(g->aretes, nouvelle_capacite * sizeof(arete));
        if(nouveau_tableau == NULL)
            return false;
        g->aretes = nouveau_tableau;
        g->aretes_capacite = nouvelle_capacite;
    }
    
    // Ajouter l'arête
    g->aretes[g->nb_aretes] = a;
    g->nb_aretes++;
    return true;

    // retourne true si l'arête a bien été ajoutée, false sinon
}

size_t index_arete(graphe const *g, arete a)
{
    // retourne l'index de l'arête au sein du tableau d'arêtes de g si l'arête a existe dans g,
    // la valeur UNKNOWN_INDEX sinon
    if(existe_arete(g, a) == true || existe_arete(g, swap_sommets(a)) == true){
        for(size_t i = 0; i < g->nb_aretes;i++){
            if ((g->aretes[i].s1 == a.s1 && g->aretes[i].s2 == a.s2) || (g->aretes[i].s1 == a.s2 && g->aretes[i].s2 == a.s1)){
                return i;
            }
        }
    }
    return UNKNOWN_INDEX;
}

size_t sommets_adjacents(graphe const *g, sommet s, sommet sa[])
{
    // remplit le tableau sa avec les sommets adjacents de s dans g
    // et retourne le nombre de sommets ainsi stockés
    // (on suppose que s fait bien partie du graphe g)
    // (le tableau sa est supposé assez grand pour contenir les sommets adjacents de s)
    size_t index = 0;
    for(size_t i = 0; i < g->nb_aretes; i++){
        if(g->aretes[i].s1 == s){
            sa[index] = g->aretes[i].s2;  // Le sommet adjacent
            index++;
        }
        if(g->aretes[i].s2 == s){
            sa[index] = g->aretes[i].s1;  // Le sommet adjacent
            index++;
        }
    }
    return index;
}
