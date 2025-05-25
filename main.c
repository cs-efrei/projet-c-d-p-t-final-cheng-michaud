/*
* Fichier : main.c
 * Auteur  : Thibault Michaud et Eloi Cheng
 * Rôle    : Fichier principal du programme. Contient l’interface utilisateur textuelle permettant de charger,
 *           afficher, filtrer et sauvegarder des images BMP 8 bits ou 24 bits.
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"
#include "bmp24.h"

// Variables globales pour stocker les images chargées
t_bmp8 *image8 = NULL;
t_bmp24 *image24 = NULL;
int image_type = 0; // 0: aucune, 8: BMP 8 bits, 24: BMP 24 bits

// Fonction utilitaire pour libérer la mémoire et nettoyer
void cleanup_images() {
    if (image8 != NULL) {
        bmp8_free(image8);
        image8 = NULL;
    }
    if (image24 != NULL) {
        bmp24_free(image24);
        image24 = NULL;
    }
    image_type = 0;
}

// Fonction pour charger une image
void load_image() {
    char filename[256];
    printf("Entrez le nom du fichier image (avec extension) : ");
    scanf("%255s", filename);

    // Nettoyer les images précédentes
    cleanup_images();

    // Essayer de charger comme BMP 8 bits d'abord
    image8 = bmp8_loadImage(filename);
    if (image8 != NULL) {
        image_type = 8;
        printf("Image 8 bits chargée avec succès.\n");
        return;
    }

    // Essayer de charger comme BMP 24 bits
    image24 = bmp24_loadImage(filename);
    if (image24 != NULL) {
        image_type = 24;
        printf("Image 24 bits chargee avec succes.\n");
        return;
    }

    printf("Impossible de charger l'image. Verifiez le nom du fichier et le format.\n");
}

// Fonction pour sauvegarder une image
void save_image() {
    if (image_type == 0) {
        printf("Aucune image chargée. Veuillez d'abord charger une image.\n");
        return;
    }

    char filename[256];
    printf("Entrez le nom du fichier de sortie (avec extension .bmp) : ");
    scanf("%255s", filename);

    if (image_type == 8) {
        bmp8_saveImage(filename, image8);
    } else if (image_type == 24) {
        bmp24_saveImage(filename, image24);
    }
}

// Fonction pour afficher les infos de l'image
void display_image_info() {
    if (image_type == 0) {
        printf("Aucune image chargee. Veuillez d'abord charger une image.\n");
        return;
    }

    if (image_type == 8) {
        bmp8_printInfo(image8);
    } else if (image_type == 24) {
        bmp24_printInfo(image24);
    }
}

// Fonction pour appliquer les filtres BMP 8 bits
void apply_filters_bmp8() {
    int choix_filtre = 0;
    int valeur;

    while (1) {
        printf("\n-- Filtres BMP 8 bits --\n");
        printf("1. Negatif\n");
        printf("2. Luminosite\n");
        printf("3. Binarisation\n");
        printf("4. Egalisation d'histogramme\n");
        printf("5. Retourner au menu principal\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choix_filtre);

        if (choix_filtre == 5) {
            break;
        }

        switch (choix_filtre) {
            case 1:
                bmp8_negative(image8);
                break;
            case 2:
                printf("Entrez la valeur de luminosite (-255 à +255) : ");
                scanf("%d", &valeur);
                bmp8_brightness(image8, valeur);
                break;
            case 3:
                printf("Entrez le seuil de binarisation (0 à 255) : ");
                scanf("%d", &valeur);
                bmp8_threshold(image8, valeur);
                break;
            // Dans main.c, remplacez le case 4 dans apply_filters_bmp8() par :

            case 4: {
                // Calculer l'histogramme
                unsigned int *hist = bmp8_computeHistogram(image8);
                if (hist == NULL) {
                    printf("Erreur lors du calcul de l'histogramme.\n");
                    break;
                }

                // Calculer la fonction de répartition cumulative
                unsigned int *hist_eq = bmp8_computeCDF(hist);
                if (hist_eq == NULL) {
                    printf("Erreur lors du calcul de la CDF.\n");
                    free(hist);
                    break;
                }

                // Appliquer l'égalisation
                bmp8_equalize(image8, hist_eq);
                printf("Égalisation d'histogramme appliquée avec succès.\n");

                // Libérer la mémoire
                free(hist);
                free(hist_eq);
                break;
            }
            default:
                printf("Choix de filtre invalide.\n");
        }
    }
}

// Fonction pour appliquer les filtres BMP 24 bits
void apply_filters_bmp24() {
    int choix_filtre = 0;
    int valeur;

    while (1) {
        printf("\n-- Filtres BMP 24 bits --\n");
        printf("1. Negatif\n");
        printf("2. Luminosite\n");
        printf("3. Niveaux de gris\n");
        printf("4. Flou (Box Blur)\n");
        printf("5. Flou gaussien\n");
        printf("6. Nettete\n");
        printf("7. Contours\n");
        printf("8. Relief (Emboss)\n");
        printf("9. Retourner au menu principal\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choix_filtre);

        if (choix_filtre == 9) {
            break;
        }

        switch (choix_filtre) {
            case 1:
                bmp24_negative(image24);
                break;
            case 2:
                printf("Entrez la valeur de luminosite (-255 à +255) : ");
                scanf("%d", &valeur);
                bmp24_brightness(image24, valeur);
                break;
            case 3:
                bmp24_grayscale(image24);
                break;
            case 4:
                bmp24_boxBlur(image24);
                printf("Filtre flou applique avec succes.\n");
                break;
            case 5:
                bmp24_gaussianBlur(image24);
                printf("Filtre flou gaussien applique avec succes.\n");
                break;
            case 6:
                bmp24_sharpen(image24);
                printf("Filtre nettete applique avec succes.\n");
                break;
            case 7:
                bmp24_outline(image24);
                printf("Filtre contours applique avec succes.\n");
                break;
            case 8:
                bmp24_emboss(image24);
                printf("Filtre relief applique avec succes.\n");
                break;
            default:
                printf("Choix de filtre invalide.\n");
        }
    }
}

// Fonction pour appliquer les filtres selon le type d'image
void apply_filters() {
    if (image_type == 0) {
        printf("Aucune image chargee. Veuillez d'abord charger une image.\n");
        return;
    }

    if (image_type == 8) {
        apply_filters_bmp8();
    } else if (image_type == 24) {
        apply_filters_bmp24();
    }
}

int main() {
    int choix_principal = 0;

    printf("=== EDITEUR D'IMAGES BMP ===\n");
    printf("Support des formats BMP 8 bits et 24 bits\n");

    while (1) {
        printf("\n=== Menu Principal ===\n");
        printf("1. Ouvrir une image\n");
        printf("2. Sauvegarder une image\n");
        printf("3. Appliquer un filtre\n");
        printf("4. Afficher les informations de l'image\n");
        printf("5. Quitter\n");

        if (image_type != 0) {
            printf("Image actuellement chargee : %d bits\n", image_type);
        }

        printf(">>> Votre choix : ");
        scanf("%d", &choix_principal);

        switch (choix_principal) {
            case 1:
                load_image();
                break;

            case 2:
                save_image();
                break;

            case 3:
                apply_filters();
                break;

            case 4:
                display_image_info();
                break;

            case 5:
                cleanup_images();
                printf("Merci d'avoir utilise notre code! \n");
                exit(0);

            default:
                printf("Choix invalide. Veuillez entrer un nombre entre 1 et 5.\n");
        }
    }

    return 0;
}