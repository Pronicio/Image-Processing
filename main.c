#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./src/utils/utils.h"
#include "./src/bmp8.h"
#include "./src/color.h"
#include "./src/histogram.h"


int main(void) {
    t_bmp8 *img = NULL;
    t_bmp24 *img24 = NULL;

    int choice;
    BMP_Type image_type = BMP_UNKNOWN;

    printf("👋 Welcome to the BMP Image Processing Program!\n");

    while (1) {
        printf("#️⃣ Please select an option: \n");
        printf(
            "1. Open image \n2. Save image \n3. Apply filter \n4. Equalize image \n5. Display current image information \n6. Exit\n");
        printf(">>> Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char filename[256];
                printf("Enter the image to open (in ./images/...): ");
                scanf("%s", filename);

                // Check if the file is a BMP file (8-bit or 24-bit)
                image_type = bmp_getFileType(filename);

                if (image_type == BMP_ERROR) {
                    printf("⚠️ Error: Invalid BMP file or unsupported format.\n");
                    break;
                }

                if (image_type == BMP_8BIT) {
                    printf("Loading an 8-bit BMP image...\n");
                    img = bmp8_loadImage(filename);
                    if (img != NULL) printf("✨ Image loaded successfully!\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    printf("Loading a 24-bit BMP image...\n");
                    img24 = bmp24_loadImage(filename);
                    if (img24 != NULL) printf("✨ Image loaded successfully!\n");
                    break;
                }

                printf("⚠️ Unsupported image type.\n");
                break;
            }
            case 2: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ No image loaded to save.\n");
                    break;
                }

                char filename[256];
                printf("Enter the filename to save (in ./images/...): ");
                scanf("%s", filename);

                if (image_type == BMP_8BIT) {
                    bmp8_saveImage(img, filename);
                    printf("✨ Image saved successfully!\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    bmp24_saveImage(img24, filename);
                    printf("✨ Image saved successfully!\n");
                    break;
                }

                break;
            }
            case 3: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ No image loaded to change.\n");
                    break;
                }

                int filter;
                printf("#️⃣ Please select a filter: \n");
                printf(
                    "1. Negative \n2. Brightness \n3. Threshold \n4. Box Blur \n5. Gaussian Blur \n6. Outline \n7. Emboss \n8. Sharpen\n");
                printf(">>> Your choice: ");
                scanf("%d", &filter);

                switch (filter) {
                    case 1: {
                        if (image_type == BMP_24BIT) bmp24_negative(img24);
                        else bmp8_negative(img);
                        printf("✨ Negative filter applied!\n");
                        break;
                    }
                    case 2: {
                        if (image_type == BMP_24BIT) bmp24_brightness(img24, 50);
                        else bmp8_brightness(img, 50);
                        printf("✨ Grayscale filter applied!\n");
                        break;
                    }
                    case 3: {
                        if (image_type == BMP_24BIT) bmp24_grayscale(img24);
                        else bmp8_threshold(img, 128);
                        printf("✨ Threshold filter applied!\n");
                        break;
                    }
                    case 4: {
                        if (image_type == BMP_24BIT) bmp24_boxBlur(img24);
                        else bmp8_box_blur(img);
                        printf("✨ Box Blur filter applied!\n");
                        break;
                    }
                    case 5: {
                        if (image_type == BMP_24BIT) bmp24_gaussianBlur(img24);
                        else bmp8_gaussian_blur(img);
                        printf("✨ Gaussian Blur filter applied!\n");
                        break;
                    }
                    case 6: {
                        if (image_type == BMP_24BIT) bmp24_outline(img24);
                        else bmp8_outline(img);
                        printf("✨ Outline filter applied!\n");
                    }
                    case 7: {
                        if (image_type == BMP_24BIT) bmp24_emboss(img24);
                        else bmp8_emboss(img);
                        printf("✨ Emboss filter applied!\n");
                        break;
                    }
                    case 8: {
                        if (image_type == BMP_24BIT) bmp24_sharpen(img24);
                        else bmp8_sharpen(img);
                        printf("✨ Sharpen filter applied!\n");
                        break;
                    }
                    default: {
                        printf("✨ Invalid filter choice.\n");
                        break;
                    }
                }

                break;
            }
            case 4: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ No image loaded to equalize.\n");
                    break;
                }

                if (image_type == BMP_24BIT) {
                    bmp24_equalize(img24);
                    printf("✨ Image equalized successfully!\n");
                    break;
                }

                if (image_type == BMP_8BIT) {
                    bmp8_equalize(img);
                    printf("✨ Image equalized successfully!\n");
                }

                break;
            }
            case 5: {
                if ((img == NULL && img24 == NULL) || image_type == BMP_UNKNOWN) {
                    printf("⚠️ No image loaded to save.\n");
                    break;
                }

                printf("------------- #️⃣ Image Informations -------------\n");
                if (image_type == BMP_24BIT) bmp24_printInfo(img24);
                else bmp8_printInfo(img);
                printf("------------------------------------------------\n");
                break;
            }
            case 6: {
                printf("👋Exiting...\n");
                if (img != NULL) bmp8_free(img);
                if (img24 != NULL) bmp24_free(img24);
                printf("✨ Thank you for using the BMP Image Processing Program!\n");
                return 0;
            }
            default: {
                printf("⚠️ Invalid choice. Please retry.\n");
                break;
            }
        }
    }
}
