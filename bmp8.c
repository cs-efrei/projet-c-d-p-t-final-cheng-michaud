#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (!img) {
        fclose(f);
        return NULL;
    }

    fread(img->header, 1, 54, f);

    fread(img->colorTable, 1, 1024, f);

    img->width = *(int*)&img->header[18];
    img->height = *(int*)&img->header[22];
    img->colorDepth = img->header[28];
    img->dataSize = *(int*)&img->header[34];

    img->data = malloc(img->dataSize);
    fread(img->data, 1, img->dataSize, f);

    fclose(f);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img) return;

    FILE *f = fopen(filename, "wb");
    if (!f) return;

    fwrite(img->header, 1, 54, f);
    fwrite(img->colorTable, 1, 1024, f);
    fwrite(img->data, 1, img->dataSize, f);

    fclose(f);
}


void bmp8_free(t_bmp8 *img) {
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Pas d'image chargée\n");
        return;
    }

    printf("Taille : %d x %d pixels\n", img->width, img->height);
    printf("Profondeur : %d bits\n", img->colorDepth);
}

void bmp8_negative(t_bmp8 *img) {

    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }


    for (unsigned int i = 0; i < img->dataSize; i++) {

        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness(t_bmp8 *img, int value) {

    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }


    for (unsigned int i = 0; i < img->dataSize; i++) {
        int new_value = img->data[i] + value;


        if (new_value > 255) {
            img->data[i] = 255;
        } else if (new_value < 0) {
            img->data[i] = 0;
        } else {
            img->data[i] = (unsigned char)new_value;
        }
    }
}

void bmp8_threshold(t_bmp8 *img, int threshold) {

    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }


    if (threshold < 0) threshold = 0;
    if (threshold > 255) threshold = 255;


    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}
