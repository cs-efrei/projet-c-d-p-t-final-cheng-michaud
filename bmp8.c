#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

t_bmp8 * chargerImage(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "rb");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", nomFichier);
        return NULL;
    }

    t_bmp8 *image = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (image == NULL) {
        printf("Erreur : mémoire insuffisante\n");
        fclose(fichier);
        return NULL;
    }

    fread(image->header, sizeof(unsigned char), 54, fichier);

    image->width = *(unsigned int*)&image->header[18];
    image->height = *(unsigned int*)&image->header[22];
    image->colorDepth = *(unsigned short*)&image->header[28];
    image->dataSize = *(unsigned int*)&image->header[34];

    if (image->colorDepth != 8) {
        printf("Erreur : l'image n'est pas en niveaux de gris 8 bits.\n");
        free(image);
        fclose(fichier);
        return NULL;
    }

    fread(image->colorTable, sizeof(unsigned char), 1024, fichier);

    image->data = (unsigned char *)malloc(image->dataSize * sizeof(unsigned char));
    if (image->data == NULL) {
        printf("Erreur : mémoire insuffisante pour les données de l'image\n");
        free(image);
        fclose(fichier);
        return NULL;
    }

    fread(image->data, sizeof(unsigned char), image->dataSize, fichier);

    fclose(fichier);
    return image;
}

void sauvegarderImage(const char *nomFichier, t_bmp8 *image) {
    FILE *fichier = fopen(nomFichier, "wb");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier pour écriture %s\n", nomFichier);
        return;
    }

    fwrite(image->header, sizeof(unsigned char), 54, fichier);
    fwrite(image->colorTable, sizeof(unsigned char), 1024, fichier);
    fwrite(image->data, sizeof(unsigned char), image->dataSize, fichier);

    fclose(fichier);
}

void libererImage(t_bmp8 *image) {
    if (image != NULL) {
        if (image->data != NULL) {
            free(image->data);
        }
        free(image);
    }
}

void afficherInfosImage(t_bmp8 *image) {
    if (image != NULL) {
        printf("Infos de l'image :\n");
        printf("Largeur : %u\n", image->width);
        printf("Hauteur : %u\n", image->height);
        printf("Profondeur Couleur : %u\n", image->colorDepth);
        printf("Taille des Données : %u\n", image->dataSize);
    } else {
        printf("Image vide.\n");
    }
}