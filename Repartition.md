**🔹 Adrien -- Conception & Structures de données**

Responsable des **étapes 1 et 3 en partie**.

**Tâches :**

- Créer les structures de données :

  - Adresse MAC

  - Adresse IP

  - Station

  - Switch (avec table de commutation, nombre de ports, priorité)

  - Réseau local basé sur un graphe étiqueté

- Implémenter les fonctions d'affichage :

  - IP en décimal pointé

  - MAC en hexadécimal

  - Tables de commutation

- Créer la structure d'une trame Ethernet :

  - Champs réalistes (préambule, SFD, MAC src/dest, type, données, FCS)

- Fonctions d'affichage des trames :

  - Vue utilisateur (lisible)

  - Vue hexadécimale (brute)

**🔹 Arthur -- Lecture et écriture des fichiers de configuration**

Responsable de **l'étape 2 + partie affichage pour la démo**.

**Tâches :**

- Écrire le parseur pour charger un réseau depuis un fichier texte
  (comme l'exemple donné)

- Créer les fonctions :

  - Détection du type d'équipement (1 ou 2)

  - Stockage des équipements et des liens

- Respecter le format :

  - Switchs en premier

  - Stations ensuite

  - Liens avec poids corrects (100, 19, 4)

- Gérer la sauvegarde éventuelle du réseau modifié

- Préparer les cas de test (petits fichiers config) pour la démo

**🔹 Andy -- Algorithmes de commutation & STP**

Responsable des **étapes 3 (diffusion trames) et 4 (STP)**.

**Tâches :**

- Implémenter l'envoi d'une trame Ethernet dans le réseau :

  - Simulation de la diffusion dans le graphe

  - Mise à jour des tables de commutation

- Implémenter le protocole STP :

  - Échange de BPDU entre switchs

  - Détermination du port racine, ports désignés, ports bloqués

  - Convergence du protocole

- Affichage des états des ports à la fin de STP

**🔧 Collaboration et intégration (à faire ensemble)**

- Définir ensemble l'architecture du projet (répertoires, makefile,
  main.c, etc.)

- Définir les interfaces entre les modules (fonctions publiques entre
  structures, fichiers, STP...)

- Faire des tests croisés (ex. : une personne écrit un fichier config,
  une autre le lit et affiche le graphe)

- Préparer la **présentation finale** en groupe :

  - Explication des choix

  - Démonstration du code

  - Répartition de la parole
