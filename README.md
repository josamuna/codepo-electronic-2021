[![]()](https://monitor-engine.com)

# Monitor Engine - ULB / CODEPO

> Cette partie est le complément logique du module logiciel fait par les étudiants de l'[ISIG-Goma](https://www.isig.ac.cd/) qui se trouve successivement dans ces deux répository ([Backend](https://github.com/josamuna/codepo-backend) et [Frontend](https://github.com/josamuna/codepo-fontend)).

## Table des Matières

1. [Introduction](#Introduction)
2. [Liste des composants électroniques](#Composants)
3. [Schémas électroniques et PCB](#Schémas)
4. [Outils de developpement du code source](#Outils-developpement)
5. [Structure du projet](#Structure)
6. [Utilisation des commandes AT et Thingstream](#Commandes-AT-Thingstream)
7. [Fonctionnement du module électronique](#Fonctionnement)
8. [Récommandations](#Récommandations)
9. [Sponsors](#Sponsors)
10. [Contributeurs](#Contributeurs)
11. [Licence](#Licence)

### Introduction

***

### Liste des composants électroniques

***

### Schémas électroniques et PCB

***

### Outils de developpement du code source

***

### Structure du projet

***

La structure du projet reprend les dossiers et fichiers suivants détaillés dans cette arborescence:

```
.\
│
├── build\
│   └── default\
│       └── production\
│           ├── adc.o
│           ├── adc.o.d
│           ├── init.o
│           ├── init.o.d
│           ├── main.o
│           ├── main.o.d
│           ├── thingstream.o
│           ├── thingstream.o.d
│           ├── uart1.o
│           └── uart1.o.d
│
├── debug\
│   └── default\
│
│
├── dist\
│   └── default\
│       └── production\
│           ├── codepo-electronic-2021.production.elf
│           ├── codepo-electronic-2021.production.hex
│           ├── codepo-electronic-2021.production.map
│           └── memoryfile.xml
│
├── nbproject\
│   ├── private\
│   │   ├── configurations.xml
│   │   ├── private.xml
│   │   └── SuppressibleMessageMemo.properties
│   │
│   ├── configurations.xml
│   ├── Makefile-default.mk
│   ├── Makefile-genesis.properties
│   ├── Makefile-impl.mk
│   ├── Makefile-local-default.mk
│   ├── Makefile-variables.mk
│   ├── Package-default.bash
│   └── project.xml
│
├── adc.c
├── adc.h
├── init.c
├── init.h
├── LICENSE
├── main.c
├── main.c.dump
├── Makefile
├── README.md
├── thingstream.c
├── thingstream.h
├── uart1.c
├── uart1.h
├── uart1.h.dump
├── uart2.c
└── uart2.h
```

- `..\build\` est le répertoire principal de génération des fichiers objects après compilation. 
- `..\dist\` répertorie de génération des fichier d'utilisation en production.
- `..\nbproject\` répertoire de configuration de l'IDE.
- `..\adc.c` est le fichier permettant d'initialiser la conversion anologue vers digital
- `..\adc.h` est le header du fichier adc.c
- `..\init.c` est le fichier permettant d'initialiser les paramètres du microcontroleur
- `..\init.h` est le header du fichier init.c
- `..\LICENSE` ...............
- `..\main.c` est le fichier principal d'entré du programme. C'est ici que commence l'éxécution du code par le compilateur.
- `..\main.c.dump` ...............
- `..\Makefile` ...............
- `..\README.md` le fichier contenant la documentation du projet.
- `..\thingstream.c` est le fichier contenant les différentes fonctions qui permettent d'envoyer des commandes au module de communication Thingstream Click via l'UART afin de, par exemple, se connecter au reseau, recevoir des messages, en envoyer, etc.
- `..\thingstream.h` est le header du fichier thingstream.c
- `..\uart1.c` est le fichier qui permet la communication UART via 2 pattes (TX et RX) du micro-controleur (communication entre le micro-controleur et le module de communication Thingstream Click)
- `..\uart1.h` est le header du ficier uart1.c
- `..\uart1.dump` ...............
- `..\uart2.c`  est le fichier qui permet la communication UART via 2 autres pattes (TX et RX) du micro-controleur
- `..\uart2.h` est le header du fichier uart2.c
- `..\adc.c` ...............

### Utilisation des commandes AT et Thingstream

***

### Fonctionnement du module électronique

***

### Récommandations

***

### Sponsors

1. En tant que sponsors prépondérant, l'[ULB (Université Libre de Bruxelles)-CODEPO (Cellule de Coopération au Développement de l'Ecole Polytechnique de Bruxelles)](https://polytech.ulb.be/fr/international/cellule-de-cooperation-au-developpement) au sein de la CODEPO assure la mise au point de ce module électronique.

[![ULB](https://user-images.githubusercontent.com/15903230/74433560-4c7cc780-4e69-11ea-8c20-62a458ae1ffb.png)](https://polytech.ulb.be/fr/international/cellule-de-cooperation-au-developpement)

2. Comme financier, l'[ARES(Académie de Recherche et d'Enseignement Supérieur)](https://www.ares-ac.be/fr/) finance bon nombre des activités du projet. ***Il est le bailleur de fond numéro 1***. Néanmoins, l'***ULB*** en tant qu'Institution d'implémentation de la solution, en assure les financements subsidiaires et fournit sa main d'oeuvre (Encadrants, Experts et étudiants).

3. Pour sa par prépondérante dans la collaboration, l'[ISIG (Institut Supérieur d'Informatique et de Gestion de Goma)](https://www.isig.ac.cd/) assure la mise en place de la plateforme logiciel devant communiquer avec le module électronique.
  
[![ISIG Goma](https://user-images.githubusercontent.com/15903230/74431114-f7d84d00-4e66-11ea-9b20-d3db26e0db89.png)](https://www.isig.ac.cd/)

### Contributeurs

***

 Plusieurs personnes ont apporté leurs pierres dans la mise au point de ce logiciel pour la part de l'ULB et de lISIG, nous rémercions: 
- **Alban Dietrich**: <alban.dietrich@ulb.ac.be>.
- **Antoine Nonclercq** : `+3226503086` , <anoncler@ulb.ac.cd>.
- **Arnaud Depuydt**: <Arnaud.Depuydt@ulb.be>.
- **Cecile Castiaux**. <Cecile.Castiaux@ulb.be>.
- **Josué Isamuna** : `+243972727527` , <josueisamuna@isig.ac.cd>, <https://github.com/josamuna>.
- **Margaux Mannaerts** : <Margaux.Mannaerts@ulb.be>.
- **Maxime Petre** : <vitaltakanic@gmail.com>.
- **Michel Osée** : `+243994814807` , <Michel.Osee@ulb.be>, <https://github.com/miosee>.
- **Nathan Gartner** : <Nathan.Gartner@ulb.be>.
- **Nicolas Wallemacq** : `+32474769804` , <Nicolas.Wallermacq@ulb.be>, <https://github.com/NicolasWa>.
- **Orianne Bastin** : `+3226502838` , <orianne.bastin@ulb.be>.
- **Philippe Mutijima** : <Philippe.Mwansa.Mutijima@ulb.be>.
- **Philippe Questel** : `+32474769804` , <Philippe.Questel@ulb.be>.

### License

***

Ce projet est par ailleurs soumis au droits relatifs à la licence [MIT](https://mit-license.org/).
