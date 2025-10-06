# Simulation Réseau 🌐  
[English version below](#english-version)

Simulation d’une architecture réseau locale en C : switches, stations, trames Ethernet (structures de trame) et protocole Spanning Tree (STP).

---

## 👥 Contributeurs
| Nom | GitHub |
|-----|--------|
| Arthur ALEXANDRE | [@ArthurALEXANDRE-29](https://github.com/ArthurALEXANDRE-29) |
| Andy PHAN | [@cestlelheure](https://github.com/cestlelheure) |
| Adrien THIERRY |  |

---

## 🖼️ Aperçu

| Topologie STP | Trame (hex) |
|---------------|-------------|
| ![Topologie STP (exemple)](assets/topologie_cycle.png) | ![Affichage trame en hex](assets/trame_hex.png) |

➡️ Exemples détaillés (fichier de config + transcription + schéma) : voir [docs/EXEMPLES.md](docs/EXEMPLES.md)

---

## ✨ Fonctionnalités (résumé)
- Chargement d’une topologie (fichiers `src/config*.txt` ou `src/exemple_reseau.txt`)
- Parsing & construction d’un graphe (graphe.c / graphe.h)
- Switches / stations (MAC, IP, priorité STP)
- Représentation & manipulation d’adresses (adresse.c / adresse.h)
- Trames (construction / affichage lisible & hex) via `trame.c / trame.h`
- Table de commutation (apprentissage MAC) dans `switch.c`
- Spanning Tree Protocol (STP) : élection racine, rôles de ports, blocage boucles (`stp.c`)
- Gestion multi-configurations (ex: config1.txt, config2.txt…)
- Exemple pédagogique minimal séparé (exemple_reseau.txt)

---

## 🚀 Installation & Exécution (version simplifiée)
```bash
git clone https://github.com/ArthurALEXANDRE-29/simulation-reseau.git
cd simulation-reseau/src
make
./main exemple_reseau.txt
```
Autre (ex: config existante) :
```bash
./main config1.txt
```

Nettoyage :
```bash
make clean
```

Prérequis : gcc, make (optionnel: valgrind).

---

## 📁 Structure (simplifiée)
```
.
├── README.md
├── Makefile
├── assets/
│   ├── topologie_cycle.png
│   └── trame_hex.png
├── docs/
│   └── EXEMPLES.md
└── src/
    ├── Makefile                  (si utilisé localement)
    ├── main.c
    ├── adresse.c / adresse.h
    ├── configuration.c / configuration.h
    ├── graphe.c / graphe.h
    ├── station.c / station.h
    ├── switch.c / switch.h
    ├── stp.c / stp.h
    ├── trame.c / trame.h
    ├── config1.txt
    ├── config2.txt
    ├── config3.txt
    ├── config4.txt               (gros fichier)
    ├── test_config.txt
    ├── exemple_reseau.txt        (exemple léger pédagogique)
    └── (objets *.o après compilation)
```

---

## 📋 Cahier des charges (avancement)
| Élément | Statut |
|---------|--------|
| Structures (stations / switches) | ✔️ |
| Parsing fichiers config | ✔️ |
| Graphe & liens | ✔️ |
| Trames (construction / affichage) | ✔️ |
| Table de commutation (MAC learning) | ✔️ |
| STP (ports / convergence) | ✔️ |
| Exemples & fichiers de test | ✔️ |
| Optimisations | 🔜 |
| Documentation approfondie | 🔜 |

---

## 📝 Licence
Projet académique (Université de Strasbourg) – usage pédagogique.

---

# English Version

## Overview
Local network simulation in C: switches, hosts, frame structures, MAC learning and STP (root election, port roles, loop avoidance).

---

## 👥 Contributors
| Name | GitHub |
|------|--------|
| Arthur ALEXANDRE | [@ArthurALEXANDRE-29](https://github.com/ArthurALEXANDRE-29) |
| Andy PHAN | [@cestlelheure](https://github.com/cestlelheure) |
| Adrien THIERRY |  |

---

## 🖼️ Screenshots
| STP Topology | Frame (hex) |
|--------------|-------------|
| ![STP topology (example)](assets/topologie_cycle.png) | ![Frame hex dump](assets/trame_hex.png) |

➡️ Detailed examples (config file + parsed view + diagram): see [docs/EXEMPLES.md](docs/EXEMPLES.md)

---

## ✨ Features (summary)
- Load topology from config files (`src/config*.txt` / `src/exemple_reseau.txt`)
- Graph building (graphe.c)
- Switch / host entities (priority, MAC, IP)
- Address handling (adresse.c)
- Frame building & hex display (trame.c)
- MAC learning switching logic (switch.c)
- Spanning Tree Protocol (stp.c)
- Multiple ready-to-run sample configs
- Lightweight pedagogical example (exemple_reseau.txt)

---

## 🚀 Quick Setup
```bash
make
./main src/exemple_reseau.txt
```
Other sample:
```bash
./main config1.txt
```
Cleanup:
```bash
make clean
```

Requirements: gcc, make (optional: valgrind).

---

## 📁 Structure (simplified)
```
(see French section – identical layout)
```

---

## 📋 Requirements Progress
| Item | Status |
|------|--------|
| Core entities | ✔️ |
| Config parsing | ✔️ |
| Graph & links | ✔️ |
| Frames (build / print) | ✔️ |
| MAC learning | ✔️ |
| STP (roles / convergence) | ✔️ |
| Sample configs | ✔️ |
| Optimizations | 🔜 |
| Extended docs | 🔜 |

---

## 📝 License
Academic project (University of Strasbourg) – educational use.

---
