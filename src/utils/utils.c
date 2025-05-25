#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Détermine le type d'un fichier BMP (8 ou 24 bits par pixel)
 *
 * @param filename Le chemin vers le fichier BMP à analyser
 * @return BMP_Type: Le type de BMP identifié ou une valeur d'erreur
 */
BMP_Type bmp_getFileType(const char *filename) {
    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    FILE *file = fopen(path, "rb"); // Ouvrir en mode lecture binaire
    if (file == NULL) {
        fprintf(stderr, "Fichier non trouvé\n");
        return BMP_ERROR;
    }

    // Lire l'en-tête (54 octets)
    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        fprintf(stderr, "Erreur lors de la lecture de l'en-tête\n");
        fclose(file);
        return BMP_ERROR;
    }

    // Vérifier s'il s'agit d'un fichier BMP (l'en-tête doit commencer par 'BM')
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Fichier BMP non valide\n");
        fclose(file);
        return BMP_UNKNOWN;
    }

    // Extraire la profondeur de couleur de l'en-tête
    unsigned short colorDepth = *(unsigned short *) &header[28];

    fclose(file);

    // Retourner le type selon la profondeur de bits
    if (colorDepth == 8) return BMP_8BIT;
    if (colorDepth == 24) return BMP_24BIT;

    fprintf(stderr, "⚠️ Profondeur de couleur non supportée (%d bits)\n", colorDepth);
    return BMP_UNKNOWN;
}
