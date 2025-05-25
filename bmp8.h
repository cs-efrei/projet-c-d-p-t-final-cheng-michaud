/*
* Fichier : bmp8.h
 * Auteur  : Thibault Michaud et Eloi Cheng
 * Rôle    : Déclaration des structures et des fonctions associées à la manipulation d’images BMP en 8 bits.
 *           Fournit les prototypes des fonctions de chargement, sauvegarde, traitement et filtrage d’image.
 */






#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>

// Structure pour une image BMP 8 bits
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

// Fonctions de base
t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char * filename, t_bmp8 * img);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);

// Fonctions de traitement d'image
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_negative(t_bmp8 *img);
void bmp8_threshold(t_bmp8 *img, int threshold);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

// Fonction utilitaire
unsigned int lire_entier(const unsigned char *buffer, int offset);

//part 3

unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);


#endif // BMP8_H