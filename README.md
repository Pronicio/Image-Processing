# 🖼️ Bibliothèque de Traitement d'Images BMP

Cette bibliothèque fournit des outils pour manipuler des images au format BMP, notamment pour effectuer des opérations d'analyse et d'amélioration d'image via l'égalisation d'histogramme. Elle prend en charge les images 8 bits (niveaux de gris) et 24 bits (couleur).

## 🪄 Fonctionnalités principales

- **Traitement d'images BMP 8 bits et 24 bits**
- **Analyses statistiques**:
    - Calcul d'histogrammes de niveaux de gris
    - Calcul de fonction de distribution cumulative (CDF)
- **Amélioration d'images**:
    - Égalisation d'histogramme pour images 8 bits (niveaux de gris)
    - Égalisation d'histogramme pour images 24 bits (couleur) utilisant l'espace YUV
- **Filtres d'images:**
    - Filtres de convolution (flou, netteté, détection de contours)
    - Filtres médian et gaussien pour réduction du bruit

## 🌱 Structure du projet

```
├── main.c                  # Fichier principal pour l'exécution du programme
├── src/
│   ├── bmp8.c/h            # Gestion des images BMP 8 bits
│   ├── color.c/h           # Gestion des images BMP 24 bits
│   ├── histogram.c/h       # Fonctions d'analyse et égalisation d'histogramme
│   ├── utils/
│   │   └── utils.c/h       # Fonctions utilitaires pour le traitement d'images
│   └── [...]
├── images/                 # Dossier pour les images d'exemple
│   └── [...]
```

## 🔧 Principes techniques

### 📟 Égalisation d'histogramme

L'égalisation d'histogramme est une technique de traitement d'images qui améliore le contraste global en redistribuant les intensités de pixels. Le processus comprend:

1. Calcul de l'histogramme de l'image
2. Calcul de la fonction de distribution cumulative (CDF)
3. Normalisation de la CDF pour obtenir la fonction de transformation
4. Application de la fonction de transformation à chaque pixel

### 🎨 Traitement d'images couleur

Pour les images 24 bits (RGB), l'égalisation est réalisée selon le processus suivant:
1. Conversion de l'espace RGB vers YUV
2. Égalisation de la composante Y (luminance) uniquement
3. Conservation des composantes U et V (chrominance)
4. Reconversion de YUV vers RGB

## Compilation et utilisation

```bash
# Exemple de compilation
gcc -o image_processor src/*.c -lm

# Exemple d'utilisation
./image_processor
```

## Exemples de code

```c
// Charger une image BMP 8 bits
t_bmp8 *img = bmp8_load("input.bmp");

// Égaliser l'histogramme
bmp8_equalize(img);

// Sauvegarder l'image traitée
bmp8_save(img, "output.bmp");

// Libérer la mémoire
bmp8_free(img);
```

## Licence

[MIT License](LICENSE)