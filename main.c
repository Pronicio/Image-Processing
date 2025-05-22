#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./src/bmp8.h"
#include "./src/bmp24.h"
#include "src/histogram.h"

void menu_partie2();
void test();

int main(void) {
    test();
    return 1;

    t_bmp8 *img = NULL;
    int choice;

    while (1) {
        printf("Please select an option: \n");
        printf("1. Open image \n2. Save image \n3. Apply filter \n4. Display image information \n5. Image couleur (24 bits) \n6. Exit\n");
        printf(">>> Your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char filename[256];
                printf("Enter the filename to open (in ./images/...): ");
                scanf("%s", filename);

                img = bmp8_loadImage(filename);
                if (img != NULL) {
                    printf("Image loaded successfully!\n");
                } else {
                    break;
                }
                break;
            }
            case 2: {
                char filename[256];
                printf("Enter the filename to save (in ./images/...): ");
                scanf("%s", filename);

                bmp8_saveImage(filename, img);
                printf("Image saved successfully!\n");
                break;
            }
            case 3: {
                int filter;
                printf("Please select a filter: \n");
                printf("1. Negative \n2. Brightness \n3. Threshold \n");
                printf(">>> Your choice: ");
                scanf("%d", &filter);

                switch (filter) {
                    case 1: {
                        bmp8_negative(img);
                        printf("Negative filter applied!\n");
                        break;
                    }
                    case 2: {
                        bmp8_brightness(img, 50);
                        printf("Grayscale filter applied!\n");
                        break;
                    }
                    case 3: {
                        bmp8_threshold(img, 128);
                        printf("Threshold filter applied!\n");
                        break;
                    }
                    default: {
                        printf("Invalid filter choice.\n");
                        break;
                    }
                }

                break;
            }
            case 4: {
                char filename[256];
                printf("Enter the filename to display information: ");
                scanf("%s", filename);

                t_bmp8 *temp = bmp8_loadImage(filename);
                if (img != NULL) {
                    printf("Image loaded successfully!\n");
                } else {
                    break;
                }

                printf("------ Image Information ---\n");
                bmp8_printInfo(img);
                printf("----------------------------\n");

                bmp8_free(temp);
                break;
            }
            case 5: {
                // ← nouvelle case : uniquement lancement du menu Partie 2
                menu_partie2();
                break;
            }
            case 6: {
                printf("Exiting...\n");
                return 0;
            }
            default: {
                printf("Invalid choice. Please retry.\n");
                break;
            }
        }
    }
}

// -------- Menu Partie 2 : 24 bits --------
void menu_partie2() {
    t_bmp24 *image24 = NULL;
    int choix;
    do {
        printf("\n--- Menu Image Couleur (24 bits) ---\n");
        printf("1. Ouvrir image couleur\n");
        printf("2. Sauvegarder image couleur\n");
        printf("3. Appliquer un filtre (neg, gray, bright)\n");
        printf("4. Appliquer un filtre de convolution\n");
        printf("5. Afficher les infos\n");
        printf("6. Retour\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choix);

        if (choix == 1) {
            char path[256];
            printf("Chemin de l'image : ");
            scanf("%s", path);
            image24 = bmp24_loadImage(path);
        } else if (choix == 2 && image24) {
            char path[256];
            printf("Chemin de sortie : ");
            scanf("%s", path);
            bmp24_saveImage(image24, path);
        } else if (choix == 3 && image24) {
            int f;
            printf("1. Négatif\n2. Niveaux de gris\n3. Luminosité\n>>> ");
            scanf("%d", &f);
            if      (f == 1) bmp24_negative(image24);
            else if (f == 2) bmp24_grayscale(image24);
            else if (f == 3) {
                int v;
                printf("Valeur : ");
                scanf("%d", &v);
                bmp24_brightness(image24, v);
            }
        } else if (choix == 4 && image24) {
            int conv;
            printf("1. Box blur\n2. Gaussian blur\n3. Contours\n4. Relief\n5. Netteté\n>>> ");
            scanf("%d", &conv);
            if      (conv == 1) bmp24_boxBlur(image24);
            else if (conv == 2) bmp24_gaussianBlur(image24);
            else if (conv == 3) bmp24_outline(image24);
            else if (conv == 4) bmp24_emboss(image24);
            else if (conv == 5) bmp24_sharpen(image24);
        } else if (choix == 5 && image24) {
            bmp24_printInfo(image24);
        }
    } while (choix != 6);

    if (image24) bmp24_free(image24);
}

void test() {
    const t_bmp8 * img = bmp8_loadImage("lena_gray.bmp");

    if (img != NULL) {
        printf("Image loaded successfully!\n");
    } else {
        printf("⚠️ Error loading image!\n");
    }

    bmp8_equalize(img);
    bmp8_saveImage("lena_gray_eq.bmp", img);

}