#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "./src/utils/utils.h"
#include "./src/bmp8.h"
#include "./src/color.h"
#include "./src/histogram.h"

/**
 * Fonction principale du programme de traitement d'images BMP
 */
int main(void) {
    // Configurer la console pour utiliser l'UTF-8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Activer les polices Unicode dans la console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    t_bmp8 *img = NULL;
    t_bmp24 *img24 = NULL;

    int choice;
    BMP_Type image_type = BMP_UNKNOWN;

    printf("✨ Bienvenue dans le programme de traitement d'images BMP !\n");

    while (1) {
        printf("#️⃣ Veuillez sélectionner une option : \n");
        printf(
            "1. Ouvrir une image \n2. Sauvegarder l'image \n3. Appliquer un filtre \n4. Égaliser l'image \n5. Afficher les informations de l'image \n6. Quitter\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char filename[256];
                printf("Entrez le nom de l'image à ouvrir (dans ./images/...) : ");
                scanf("%s", filename);

                // Vérifier si le fichier est une image BMP (8-bit ou 24-bit)
                image_type = bmp_getFileType(filename);

                if (image_type == BMP_ERROR) {
                    printf("⚠️ Erreur : Fichier BMP invalide ou format non supporté.\n");
                    break;
                }

                if (image_type == BMP_8BIT) {
                    printf("Chargement d'une image BMP 8-bit...\n");
                    img = bmp8_loadImage(filename);
                    if (img != NULL) printf("✨ Image chargée avec succès !\n\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    printf("Chargement d'une image BMP 24-bit...\n");
                    img24 = bmp24_loadImage(filename);
                    if (img24 != NULL) printf("✨ Image chargée avec succès !\n\n");
                    break;
                }

                printf("⚠️ Type d'image non supporté.\n");
                break;
            }
            case 2: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ Aucune image chargée à sauvegarder.\n");
                    break;
                }

                char filename[256];
                printf("Entrez le nom du fichier pour la sauvegarde (dans ./images/...) : ");
                scanf("%s", filename);

                if (image_type == BMP_8BIT) {
                    bmp8_saveImage(img, filename);
                    printf("✨ Image sauvegardée avec succès !\n\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    bmp24_saveImage(img24, filename);
                    printf("✨ Image sauvegardée avec succès !\n\n");
                    break;
                }

                break;
            }
            case 3: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ Aucune image chargée à modifier.\n");
                    break;
                }

                int filter;
                printf("#️⃣ Veuillez sélectionner un filtre : \n");
                printf(
                    "1. Négatif \n2. Luminosité \n3. Binarisation \n4. Flou rectangulaire \n5. Flou gaussien \n6. Contour \n7. Relief \n8. Netteté\n");
                printf(">>> Votre choix : ");
                scanf("%d", &filter);

                switch (filter) {
                    case 1: {
                        if (image_type == BMP_24BIT) bmp24_negative(img24);
                        else bmp8_negative(img);
                        printf("✨ Filtre négatif appliqué !\n\n");
                        break;
                    }
                    case 2: {
                        if (image_type == BMP_24BIT) bmp24_brightness(img24, 50);
                        else bmp8_brightness(img, 50);
                        printf("✨ Filtre de luminosité appliqué !\n\n");
                        break;
                    }
                    case 3: {
                        if (image_type == BMP_24BIT) bmp24_grayscale(img24);
                        else bmp8_threshold(img, 128);
                        printf("✨ Filtre de binarisation appliqué !\n\n");
                        break;
                    }
                    case 4: {
                        if (image_type == BMP_24BIT) bmp24_boxBlur(img24);
                        else bmp8_box_blur(img);
                        printf("✨ Filtre de flou rectangulaire appliqué !\n\n");
                        break;
                    }
                    case 5: {
                        if (image_type == BMP_24BIT) bmp24_gaussianBlur(img24);
                        else bmp8_gaussian_blur(img);
                        printf("✨ Filtre de flou gaussien appliqué !\n\n");
                        break;
                    }
                    case 6: {
                        if (image_type == BMP_24BIT) bmp24_outline(img24);
                        else bmp8_outline(img);
                        printf("✨ Filtre de contour appliqué !\n\n");
                    }
                    case 7: {
                        if (image_type == BMP_24BIT) bmp24_emboss(img24);
                        else bmp8_emboss(img);
                        printf("✨ Filtre de relief appliqué !\n\n");
                        break;
                    }
                    case 8: {
                        if (image_type == BMP_24BIT) bmp24_sharpen(img24);
                        else bmp8_sharpen(img);
                        printf("✨ Filtre de netteté appliqué !\n\n");
                        break;
                    }
                    default: {
                        printf("⚠️ Choix de filtre invalide.\n");
                        break;
                    }
                }

                break;
            }
            case 4: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ Aucune image chargée à égaliser.\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    bmp24_equalize(img24);
                    printf("✨ Image égalisée avec succès !\n\n");
                    break;
                }

                if (image_type == BMP_8BIT) {
                    bmp8_equalize(img);
                    printf("✨ Image égalisée avec succès !\n\n");
                }

                break;
            }
            case 5: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ Aucune image chargée à afficher.\n");
                    break;
                }

                printf("------------- #️⃣ Informations sur l'image -------------\n");
                if (image_type == BMP_24BIT) bmp24_printInfo(img24);
                else bmp8_printInfo(img);
                printf("------------------------------------------------\n\n");
                break;
            }
            case 6: {
                printf("✨ Fermeture du programme...\n");
                if (img != NULL) bmp8_free(img);
                if (img24 != NULL) bmp24_free(img24);
                printf("✨ Merci d'avoir utilisé le programme de traitement d'images BMP !\n");
                return 0;
            }
            default: {
                printf("⚠️ Choix invalide. Veuillez réessayer.\n\n");
                break;
            }
        }
    }
}
