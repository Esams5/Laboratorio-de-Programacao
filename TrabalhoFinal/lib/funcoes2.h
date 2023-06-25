#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct {
    int width;
    int height;
    unsigned char** data;
} Imagens;

void freeImage(Imagens* image);
Imagens loadImage(const char* filename);
void saveCoordinates(const char* filename, const char* subImageName, int x, int y);
double calculateMeanSquaredError(const Imagens* image, const Imagens* subImage, int x, int y);
void searchSubImages(const char* subImageDirectory, const char* originalImageFile, const char* outputFile);