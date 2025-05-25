//
// Created by eloic on 25/05/2025.
//


#include "bmp24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>   // pour round()

// --- Fonctions d'allocation ---

// Allocation d'un tableau 2D de pixels
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    if (width <= 0 || height <= 0) {
        printf("Erreur : dimensions invalides (%d x %d).\n", width, height);
        return NULL;
    }

    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        printf("Erreur d'allocation : lignes de pixels.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {
            // Libérer les lignes déjà allouées en cas d'échec
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            printf("Erreur d'allocation : ligne %d.\n", i);
            return NULL;
        }
    }

    return pixels;
}

// Libération d'un tableau 2D de pixels
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels != NULL) {
        for (int i = 0; i < height; i++) {
            if (pixels[i] != NULL) {
                free(pixels[i]);
            }
        }
        free(pixels);
    }
}

// Allocation d'une structure t_bmp24 complète
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    if (width <= 0 || height <= 0 || colorDepth != 24) {
        printf("Erreur : paramètres invalides (w=%d, h=%d, depth=%d).\n", width, height, colorDepth);
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        printf("Erreur d'allocation de t_bmp24.\n");
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);

    if (img->data == NULL) {
        free(img);
        return NULL;
    }

    return img;
}

// Libération complète d'une image BMP24
void bmp24_free(t_bmp24 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            bmp24_freeDataPixels(img->data, img->height);
        }
        free(img);
    }
}

// --- Fonctions de base ---

// Chargement d'une image BMP 24 bits (CORRIGÉ)
t_bmp24 *bmp24_loadImage(const char *filename) {
    if (filename == NULL) {
        printf("Erreur : nom de fichier invalide.\n");
        return NULL;
    }

    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Erreur : impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    // Lire l'en-tête de fichier BMP (14 octets)
    t_bmp_header header;
    if (fread(&header, sizeof(t_bmp_header), 1, f) != 1) {
        printf("Erreur : impossible de lire l'en-tête BMP.\n");
        fclose(f);
        return NULL;
    }

    // Vérifier la signature BMP
    if (header.type != BMP_TYPE) {
        printf("Erreur : le fichier n'est pas un BMP valide (type = 0x%X, attendu 0x%X).\n",
               header.type, BMP_TYPE);
        fclose(f);
        return NULL;
    }

    // Lire l'en-tête d'information (40 octets)
    t_bmp_info info;
    if (fread(&info, sizeof(t_bmp_info), 1, f) != 1) {
        printf("Erreur : impossible de lire les informations BMP.\n");
        fclose(f);
        return NULL;
    }

    // Vérifier que c'est bien du 24 bits
    if (info.bits != 24) {
        printf("Erreur : l'image n'est pas en 24 bits (bits = %d).\n", info.bits);
        fclose(f);
        return NULL;
    }

    // Récupérer les dimensions
    int width = info.width;
    int height = abs(info.height);  // Utiliser abs() pour gérer les hauteurs négatives
    bool topDown = (info.height < 0);

    printf("Debug: Dimensions lues - largeur: %d, hauteur: %d (original: %d)\n",
           width, height, info.height);
    printf("Debug: Offset des données: %u\n", header.offset);

    // Allouer la structure complète
    t_bmp24 *img = bmp24_allocate(width, height, 24);
    if (img == NULL) {
        fclose(f);
        return NULL;
    }

    // Stocker les en-têtes
    img->header = header;
    img->header_info = info;

    // Se positionner au début des données
    if (fseek(f, header.offset, SEEK_SET) != 0) {
        printf("Erreur : impossible de se positionner aux données (offset %u).\n", header.offset);
        bmp24_free(img);
        fclose(f);
        return NULL;
    }

    // Calcul de la taille d'une ligne avec padding (multiple de 4)
    int bytesPerPixel = 3;
    int rowSize = ((width * bytesPerPixel + 3) / 4) * 4;
    printf("Debug: Taille de ligne avec padding: %d octets\n", rowSize);

    unsigned char *line = malloc(rowSize);
    if (line == NULL) {
        bmp24_free(img);
        fclose(f);
        printf("Erreur : allocation ligne temporaire.\n");
        return NULL;
    }

    // Lecture des données pixels
    // Les BMP sont stockés du bas vers le haut par défaut (sauf si hauteur négative)
    for (int i = 0; i < height; i++) {
        size_t bytesRead = fread(line, 1, rowSize, f);
        if (bytesRead != rowSize) {
            printf("Erreur : lecture incomplète ligne %d (%zu octets lus sur %d attendus).\n",
                   i, bytesRead, rowSize);
            free(line);
            bmp24_free(img);
            fclose(f);
            return NULL;
        }

        // Déterminer la ligne de destination
        int destRow = topDown ? i : (height - 1 - i);

        // Copier les pixels (format BGR vers RGB)
        for (int j = 0; j < width; j++) {
            int pixelOffset = j * bytesPerPixel;
            img->data[destRow][j].blue  = line[pixelOffset];
            img->data[destRow][j].green = line[pixelOffset + 1];
            img->data[destRow][j].red   = line[pixelOffset + 2];
        }
    }

    free(line);
    fclose(f);
    printf("Image %s chargée avec succès (%dx%d, %s).\n",
           filename, width, height, topDown ? "top-down" : "bottom-up");
    return img;
}

// Sauvegarde d'une image BMP 24 bits (CORRIGÉ)
void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    if (filename == NULL || img == NULL || img->data == NULL) {
        printf("Erreur : paramètres invalides pour la sauvegarde.\n");
        return;
    }

    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        printf("Erreur : impossible de créer le fichier %s\n", filename);
        return;
    }

    int width = img->width;
    int height = img->height;
    int bytesPerPixel = 3;
    int rowSize = ((width * bytesPerPixel + 3) / 4) * 4;
    int imageSize = rowSize * height;
    int fileSize = 54 + imageSize;  // 14 (header) + 40 (info) + données

    // Préparer l'en-tête de fichier
    t_bmp_header header;
    header.type = BMP_TYPE;
    header.size = fileSize;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = 54;


    // Préparer l'en-tête d'information
    t_bmp_info info;
    info.size = 40;
    info.width = width;
    info.height = height;  // Positif = bottom-up
    info.planes = 1;
    info.bits = 24;
    info.compression = 0;
    info.imagesize = imageSize;
    info.xresolution = 2835;  // 72 DPI
    info.yresolution = 2835;
    info.ncolors = 0;
    info.importantcolors = 0;

    // Écriture des en-têtes
    fwrite(&header, sizeof(t_bmp_header), 1, f);
    fwrite(&info, sizeof(t_bmp_info), 1, f);

    // Allouer une ligne avec padding
    unsigned char *line = calloc(rowSize, sizeof(unsigned char));
    if (line == NULL) {
        printf("Erreur d'allocation mémoire pour la ligne temporaire.\n");
        fclose(f);
        return;
    }

    // Écriture ligne par ligne (du bas vers le haut pour BMP standard)
    for (int i = 0; i < height; i++) {
        int srcRow = height - 1 - i;  // Inverser l'ordre des lignes

        // Remplir la ligne (format RGB vers BGR)
        for (int j = 0; j < width; j++) {
            int pixelOffset = j * bytesPerPixel;
            line[pixelOffset]     = img->data[srcRow][j].blue;
            line[pixelOffset + 1] = img->data[srcRow][j].green;
            line[pixelOffset + 2] = img->data[srcRow][j].red;
        }

        // Écrire la ligne complète (avec padding)
        fwrite(line, sizeof(unsigned char), rowSize, f);
    }

    free(line);
    fclose(f);
    printf("Image sauvegardée dans %s\n", filename);
}

// Affichage des informations de l'image
void bmp24_printInfo(t_bmp24 *img) {
    if (img == NULL) {
        printf("Aucune image à afficher.\n");
        return;
    }

    printf("\n--- Informations de l'image BMP24 ---\n");
    printf("Largeur      : %d pixels\n", img->width);
    printf("Hauteur      : %d pixels\n", img->height);
    printf("Profondeur   : %d bits\n", img->colorDepth);
    printf("Taille fichier : %u octets\n", img->header.size);
    printf("Offset données : %u\n", img->header.offset);
    printf("Compression  : %u\n", img->header_info.compression);
    printf("------------------------------------\n");
}

// --- Fonctions de traitement d'image ---

// Application d'un filtre négatif
void bmp24_negative(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide ou non chargée.\n");
        return;
    }

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red   = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue  = 255 - img->data[i][j].blue;
        }
    }

    printf("Filtre négatif appliqué avec succès.\n");
}

// Ajustement de la luminosité
void bmp24_brightness(t_bmp24 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide ou non chargée.\n");
        return;
    }

    if (value < -255 || value > 255) {
        printf("Attention : valeur de luminosité hors limites recommandées [-255, 255].\n");
    }

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Ajustement avec saturation dans [0, 255]
            int r = img->data[i][j].red + value;
            int g = img->data[i][j].green + value;
            int b = img->data[i][j].blue + value;

            img->data[i][j].red   = (r > 255) ? 255 : (r < 0 ? 0 : r);
            img->data[i][j].green = (g > 255) ? 255 : (g < 0 ? 0 : g);
            img->data[i][j].blue  = (b > 255) ? 255 : (b < 0 ? 0 : b);
        }
    }

    printf("Luminosité ajustée de %+d.\n", value);
}

// Conversion en niveaux de gris
void bmp24_grayscale(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide ou non chargée.\n");
        return;
    }

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            t_pixel *p = &img->data[i][j];
            // Pondération standard de luminance (ITU-R BT.709)
            uint8_t gris = (uint8_t)(0.299 * p->red + 0.587 * p->green + 0.114 * p->blue);
            p->red = p->green = p->blue = gris;
        }
    }

    printf("Filtre niveaux de gris appliqué avec succès.\n");
}

t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int n = kernelSize / 2;
    float red = 0, green = 0, blue = 0;

    for (int i = -n; i <= n; i++) {
        for (int j = -n; j <= n; j++) {
            int xi = x + i;
            int yj = y + j;

            if (xi < 0 || xi >= img->height || yj < 0 || yj >= img->width)
                continue;

            float coeff = kernel[i + n][j + n];
            t_pixel p = img->data[xi][yj];
            red   += coeff * p.red;
            green += coeff * p.green;
            blue  += coeff * p.blue;
        }
    }

    t_pixel result;
    result.red   = (red   > 255) ? 255 : (red < 0 ? 0 : round(red));
    result.green = (green > 255) ? 255 : (green < 0 ? 0 : round(green));
    result.blue  = (blue  > 255) ? 255 : (blue < 0 ? 0 : round(blue));
    return result;
}

void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL) return;

    t_pixel **copy = bmp24_allocateDataPixels(img->width, img->height);
    if (copy == NULL) return;

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            copy[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    bmp24_freeDataPixels(img->data, img->height);
    img->data = copy;
}

void bmp24_boxBlur(t_bmp24 *img) {
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    float values[3][3] = {
        {1 / 16.0f, 2 / 16.0f, 1 / 16.0f},
        {2 / 16.0f, 4 / 16.0f, 2 / 16.0f},
        {1 / 16.0f, 2 / 16.0f, 1 / 16.0f}
    };

    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_outline(t_bmp24 *img) {
    float values[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_emboss(t_bmp24 *img) {
    float values[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };

    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_sharpen(t_bmp24 *img) {
    float values[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}