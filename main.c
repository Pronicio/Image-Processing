#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp8.h"
#include "color.h"
#include "histogram.h"



int main(void) {
    t_bmp8 *img = NULL;
    int choice;

    while (1) {
        printf("Please select an option: \n");
        printf(
            "1. Open image \n2. Save image \n3. Apply filter \n4. Display image information \n5. Image couleur (24 bits) \n6. Exit\n");
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
                if (img == NULL) {
                     printf("Aucune image chargée. Ouvrez d'abord une image (option 1).\n");
                 } else {
                     printf("------ Informations de l'image ------\n");
                     bmp8_printInfo(img);
                     printf("-------------------------------------\n");
                 }
                 break;
            }
                    case 5: {
            t_bmp24 *img24 = NULL;
            char filename[256];

            // on charge l’image couleur
            printf("Entrez le nom de votre image couleur (24 bits) : ");
            scanf("%s", filename);
            img24 = bmp24_loadImage(filename);
            if (!img24) {
                printf("Échec du chargement.\n");
            } else {
                printf("Image couleur chargée avec succès !\n");

                // on la sauvegarde
                printf("Entrez le nom du fichier de sortie : ");
                scanf("%s", filename);
                bmp24_saveImage(img24, filename);
                printf("Image couleur sauvegardée avec succès !\n");

                // on libère la mémoire
                bmp24_free(img24);
            }

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
