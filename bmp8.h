#ifndef BMP8_H
#define BMP8_H

typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

t_bmp8 * chargerImage(const char *nomFichier);
void sauvegarderImage(const char *nomFichier, t_bmp8 *image);
void libererImage(t_bmp8 *image);
void afficherInfosImage(t_bmp8 *image);

#endif
