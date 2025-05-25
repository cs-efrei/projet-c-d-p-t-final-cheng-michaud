/*
* Fichier : bmp8.c
 * Auteur  : Thibault Michaud et Eloi Cheng
 * Rôle    : Implémente les fonctions pour charger, sauvegarder, libérer et traiter des images BMP en 8 bits.
 *           Contient également des fonctions de filtrage, d'ajustement de la luminosité, de négatif, de binarisation,
 *           d'égalisation d'histogramme et de convolution.
 */





#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Fonction utilitaire pour lire un entier sur 4 octets à partir d'un tableau d'octets
unsigned int lire_entier(const unsigned char *buffer, int offset) {
    unsigned int b0 = buffer[offset];
    unsigned int b1 = buffer[offset + 1] << 8;
    unsigned int b2 = buffer[offset + 2] << 16;
    unsigned int b3 = buffer[offset + 3] << 24;
    return b0 | b1 | b2 | b3;
}

// Chargement d'une image BMP 8 bits
t_bmp8 * bmp8_loadImage(const char * filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return NULL;
    }

    // Allocation mémoire pour une image t_bmp8
    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (img == NULL) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée pour l'image.\n");
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête BMP (54 octets)
    if (fread(img->header, sizeof(unsigned char), 54, file) != 54) {
        fprintf(stderr, "Erreur : Impossible de lire l'en-tête BMP.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Vérification de la signature BMP
    if (img->header[0] != 'B' || img->header[1] != 'M') {
        fprintf(stderr, "Erreur : Ce n'est pas un fichier BMP valide.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture de la profondeur de couleur (offset 28)
    img->colorDepth = lire_entier(img->header, 28);
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur : L'image n'est pas en 8 bits (profondeur = %d).\n", img->colorDepth);
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture de la table de couleurs (1024 octets pour 8 bits)
    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Erreur : Impossible de lire la table de couleurs.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Récupération des dimensions et taille des données
    img->width = lire_entier(img->header, 18);     // offset 18
    img->height = lire_entier(img->header, 22);    // offset 22
    img->dataSize = lire_entier(img->header, 34);  // offset 34

    // Si dataSize est nul, on le calcule manuellement
    if (img->dataSize == 0) {
        img->dataSize = img->width * img->height;
    }

    // Allocation mémoire pour les données de l'image
    img->data = (unsigned char *)malloc(sizeof(unsigned char) * img->dataSize);
    if (img->data == NULL) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée pour les données d'image.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture des données de l'image
    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Erreur : Impossible de lire toutes les données de l'image.\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return img;
}

// Sauvegarde d'une image BMP
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Erreur : Image invalide pour la sauvegarde.\n");
        return;
    }

    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s en écriture.\n", filename);
        return;
    }

    // Écriture de l'en-tête BMP (54 octets)
    fwrite(img->header, sizeof(unsigned char), 54, f);

    // Écriture de la table de couleurs (1024 octets pour image 8 bits)
    fwrite(img->colorTable, sizeof(unsigned char), 1024, f);

    // Écriture des données de l'image (pixels)
    fwrite(img->data, sizeof(unsigned char), img->dataSize, f);

    fclose(f);
    printf("Image sauvegardée dans %s\n", filename);
}

// Libération de la mémoire d'une image
void bmp8_free(t_bmp8 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free(img->data);
        }
        free(img);
    }
}

// Affichage des informations de l'image
void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        printf("Aucune image à afficher.\n");
        return;
    }

    printf("\n--- Informations de l'image ---\n");
    printf("Largeur      : %u pixels\n", img->width);
    printf("Hauteur      : %u pixels\n", img->height);
    printf("Profondeur   : %u bits\n", img->colorDepth);
    printf("Taille image : %u octets\n", img->dataSize);
    printf("-------------------------------\n");
}

// Ajustement de la luminosité
void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image non chargée ou invalide.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int pixel = img->data[i] + value;

        // Clamp dans l'intervalle [0, 255]
        if (pixel > 255) {
            pixel = 255;
        } else if (pixel < 0) {
            pixel = 0;
        }

        img->data[i] = (unsigned char)pixel;
    }

    printf("Luminosité ajustée de %+d.\n", value);
}

// Application d'un filtre négatif
void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image non chargée ou invalide.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }

    printf("Filtre négatif appliqué avec succès.\n");
}

// Binarisation avec seuil
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image non chargée ou invalide.\n");
        return;
    }

    if (threshold < 0 || threshold > 255) {
        printf("Erreur : le seuil doit être entre 0 et 255.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        if (img->data[i] >= threshold) {
            img->data[i] = 255;  // Blanc
        } else {
            img->data[i] = 0;    // Noir
        }
    }

    printf("Binarisation appliquée avec un seuil de %d.\n", threshold);
}

// Application d'un filtre de convolution
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image non chargée ou invalide.\n");
        return;
    }

    if (kernel == NULL || kernelSize <= 0 || kernelSize % 2 == 0) {
        printf("Erreur : noyau de convolution invalide (doit être impair et > 0).\n");
        return;
    }

    int width = img->width;
    int height = img->height;
    int offset = kernelSize / 2;

    // Allocation temporaire pour stocker les nouvelles valeurs
    unsigned char *newData = malloc(img->dataSize);
    if (newData == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour l'image filtrée.\n");
        return;
    }

    // Initialiser avec les valeurs originales (pour les bordures)
    for (unsigned int i = 0; i < img->dataSize; i++) {
        newData[i] = img->data[i];
    }

    // Appliquer la convolution sur chaque pixel (hors bordures)
    for (int y = offset; y < height - offset; y++) {
        for (int x = offset; x < width - offset; x++) {
            float sum = 0.0f;

            // Appliquer le noyau autour du pixel (x, y)
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int ix = x + kx;
                    int iy = y + ky;
                    int imageIndex = iy * width + ix;
                    float pixel = (float)img->data[imageIndex];
                    sum += pixel * kernel[ky + offset][kx + offset];
                }
            }

            // Clamp dans [0,255]
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            newData[y * width + x] = (unsigned char)sum;
        }
    }

    // Copier les nouvelles valeurs dans l'image
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = newData[i];
    }

    free(newData);
    printf("Filtre appliqué avec succès.\n");
}
//part 3

unsigned int * bmp8_computeHistogram(t_bmp8 * img) {
    if (img == NULL || img->data == NULL) return NULL;

    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) return NULL;

    for (unsigned int i = 0; i < img->dataSize; i++) {
        hist[img->data[i]]++;
    }

    return hist;
}

unsigned int * bmp8_computeCDF(unsigned int * hist) {
    if (!hist) return NULL;

    unsigned int *hist_eq = malloc(256 * sizeof(unsigned int));
    if (!hist_eq) return NULL;

    unsigned int cdf[256] = {0};
    unsigned int total = 0;
    unsigned int cdf_min = 0;

    // Calcul du CDF brut
    for (int i = 0; i < 256; i++) {
        total += hist[i];
        cdf[i] = total;
    }

    // Trouver cdf_min (la première valeur non nulle)
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Normalisation du CDF pour obtenir les nouvelles valeurs [0, 255]
    for (int i = 0; i < 256; i++) {
        hist_eq[i] = round(((float)(cdf[i] - cdf_min) / (total - cdf_min)) * 255);
    }

    return hist_eq;
}
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq) {
    if (!img || !img->data || !hist_eq) return;

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char) hist_eq[img->data[i]];
    }
}
