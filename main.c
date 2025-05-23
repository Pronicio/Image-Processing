#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./src/bmp24.h"
#include "./src/bmp8.h"
#include "./src/color.h"
#include "./src/histogram.h"

void menu_partie2(void);
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
}   return 0
