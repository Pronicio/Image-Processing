#ifndef BMP8_H
#define BMP8_H

// Type structuré t_bmp8 pour représenter une image en niveaux de gris
typedef struct {
    unsigned char header[54]; // En-tête BMP
    unsigned char colorTable[1024]; // Palette de couleurs (256 couleurs, 4 octets chacune)
    unsigned char *data; // Données des pixels

    unsigned int width; // Largeur de l'image
    unsigned int height; // Hauteur de l'image
    unsigned int colorDepth; // Profondeur de couleur (doit être 8 bits)
    unsigned int dataSize; // Taille des données de pixels (tailleRangée * hauteur)
} t_bmp8;

// Prototypes des fonctions pour le traitement d'images BMP8
/**
 * Charge une image BMP 8 bits à partir d'un fichier
 *
 * @param filename Le chemin vers le fichier à charger
 * @return t_bmp8*: Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
t_bmp8 *bmp8_loadImage(const char *filename);

/**
 * Sauvegarde une image BMP 8 bits dans un fichier
 *
 * @param img L'image à sauvegarder
 * @param filename Le nom du fichier de destination
 */
void bmp8_saveImage(t_bmp8 *img, const char *filename);

/**
 * Libère la mémoire allouée pour une image BMP 8 bits
 *
 * @param img L'image à libérer
 */
void bmp8_free(t_bmp8 *img);

/**
 * Affiche les informations d'une image BMP 8 bits
 *
 * @param img L'image dont on veut afficher les informations
 */
void bmp8_printInfo(t_bmp8 *img);

// Modifications simples
/**
 * Applique un effet négatif à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_negative(t_bmp8 *img);

/**
 * Modifie la luminosité d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 * @param value La valeur à ajouter à chaque pixel (-255 à 255)
 */
void bmp8_brightness(const t_bmp8 *img, int value);

/**
 * Applique un seuillage à une image BMP 8 bits
 *
 * @param img L'image à modifier
 * @param threshold La valeur de seuil (0-255)
 */
void bmp8_threshold(t_bmp8 *img, int threshold);

/**
 * Applique un filtre à une image BMP 8 bits en utilisant un noyau de convolution
 *
 * @param img L'image à modifier
 * @param kernel Le noyau de convolution à appliquer
 * @param kernelSize La taille du noyau (doit être impair)
 */
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);

// Filtres avec convolution
/**
 * Applique un flou rectangulaire à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_box_blur(t_bmp8* img);

/**
 * Applique un flou gaussien à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_gaussian_blur(t_bmp8* img);

/**
 * Détecte les contours d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_outline(t_bmp8* img);

/**
 * Applique un effet de relief à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_emboss(t_bmp8* img);

/**
 * Améliore la netteté d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_sharpen(t_bmp8* img);


#endif //BMP8_H
