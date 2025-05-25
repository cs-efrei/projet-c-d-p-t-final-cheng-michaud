Projet de Traitement d'Images BMP (8 bits & 24 bits)

Contexte

Ce projet universitaire en langage C permet de charger, modifier et sauvegarder des images au format BMP. Il prend en charge les images 8 bits (palette) et 24 bits (couleur RGB). L’utilisateur interagit via un menu en ligne de commande.

Auteurs:

Thibault Michaud/Eloi Cheng

Fonctionnalités

Chargement
- Chargement automatique de BMP 8 bits ou 24 bits.
- Lecture des en-têtes, de la palette (8 bits), et des pixels.
- Gestion correcte du padding et de l’ordre des lignes (bottom-up).

Filtres disponibles

Pour images BMP 8 bits
- Négatif : inversion des niveaux de gris.
- Luminosité : ajustement de -255 à +255.
- Binarisation : noir/blanc selon un seuil.
- Égalisation d'histogramme : amélioration du contraste.
- Convolution (manuelle) : disponible dans le code (hors menu).

Pour images BMP 24 bits
- Négatif
- Luminosité
- Niveaux de gris
- Flou (Box Blur)
- Flou gaussien
- Netteté
- Contours
- Relief (Emboss)

### Sauvegarde
- Enregistrement dans un nouveau fichier `.bmp` (8 ou 24 bits).

### Infos image
- Affiche : largeur, hauteur, profondeur, compression.


Prérequis

- CMake (≥ 3.10)
- GCC ou équivalent
- Système : Linux, macOS ou Windows (avec MinGW)


Utilisation

-Lancer l'exécutable.
-Entrer le nom d'une image .bmp à charger.
-Appliquer un ou plusieurs filtres.
-Sauvegarder l’image modifiée sous un autre nom.


Compilation et Exécution

```bash
mkdir build
cd build
cmake ..
make
./ProjetC
