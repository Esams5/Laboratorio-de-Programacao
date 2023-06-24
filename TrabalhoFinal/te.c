#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

#define MAX_FILENAME_LENGTH 257
#define NUM_SUBIMAGES 100

typedef struct {
    int width;
    int height;
    unsigned char** data;
} Image;

void freeImage(Image* image) {
    for (int i = 0; i < image->height; i++) {
        free(image->data[i]);
    }
    free(image->data);
}

Image loadImage(const char* filename) {
    Image image;
    FILE* file = fopen(filename, "rb");
    
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }
    
    char magicNumber[3];
    fscanf(file, "%2s\n", magicNumber);
    
    if (strcmp(magicNumber, "P5") != 0) {
        printf("Formato de arquivo inválido: %s\n", filename);
        exit(1);
    }
    
    fscanf(file, "*[^\n]\n");  // Ignorar comentários
    fscanf(file, "%d %d\n", &image.width, &image.height);
    fscanf(file, "%*d\n");      // Ignorar valor máximo de pixel
    
    image.data = (unsigned char**)malloc(image.height * sizeof(unsigned char*));
    for (int i = 0; i < image.height; i++) {
        image.data[i] = (unsigned char*)malloc(image.width * sizeof(unsigned char));
        fread(image.data[i], sizeof(unsigned char), image.width, file);
    }
    
    fclose(file);
    return image;
}

void saveCoordinates(const char* filename, const char* subImageName, int x, int y) {
    FILE* file = fopen(filename, "a");
    
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }
    
    fprintf(file, "%s_t, %d, %d\n", subImageName, x, y);
    
    fclose(file);
}

double calculateMeanSquaredError(const Image* image, const Image* subImage, int x, int y) {
    double mse = 0.0;
    int subImageWidth = subImage->width;
    int subImageHeight = subImage->height;
    
    for (int i = 0; i < subImageHeight; i++) {
        for (int j = 0; j < subImageWidth; j++) {
            int pixelDiff = image->data[y + i][x + j] - subImage->data[i][j];
            mse += pixelDiff * pixelDiff;
        }
    }
    
    mse /= (subImageWidth * subImageHeight);
    return mse;
}

void searchSubImages(const char* subImageDirectory, const char* originalImageFile, const char* outputFile) {
    Image originalImage = loadImage(originalImageFile);
    
    FILE* file = fopen(outputFile, "w");
    
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", outputFile);
        exit(1);
    }
    
    fclose(file);
    
    DIR *dir;
    struct dirent *ent;
    char subImageFile[MAX_FILENAME_LENGTH];
    
    if ((dir = opendir(subImageDirectory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                sprintf(subImageFile, "%s/%s", subImageDirectory, ent->d_name);
                Image subImage = loadImage(subImageFile);
                
                int searchWidth = originalImage.width - subImage.width;
                int searchHeight = originalImage.height - subImage.height;
                
                double minMSE = -1.0;
                int minX = -1, minY = -1;
                
                for (int y = 0; y <= searchHeight; y++) {
                    for (int x = 0; x <= searchWidth; x++) {
                        double mse = calculateMeanSquaredError(&originalImage, &subImage, x, y);
                        
                        if (minMSE == -1.0 || mse < minMSE) {
                            minMSE = mse;
                            minX = x;
                            minY = y;
                        }
                    }
                }
                
                saveCoordinates(outputFile, ent->d_name, minX, minY);
                
                freeImage(&subImage);
            }
        }
        closedir(dir);
    } else {
        printf("Erro ao abrir o diretório: %s\n", subImageDirectory);
        exit(1);
    }
    
    freeImage(&originalImage);
}

int main() {
    char subImageDirectory[MAX_FILENAME_LENGTH];
    char originalImageFile[MAX_FILENAME_LENGTH];
    char outputFile[MAX_FILENAME_LENGTH];
    
    printf("Digite o diretório das sub-imagens: ");
    scanf("%s", subImageDirectory);
    
    printf("Digite o caminho da imagem original: ");
    scanf("%s", originalImageFile);
    
    printf("Digite o nome do arquivo de saída: ");
    scanf("%s", outputFile);

    clock_t begin = clock();
    
    searchSubImages(subImageDirectory, originalImageFile, outputFile);
    
    printf("Busca concluída. Coordenadas salvas no arquivo: %s\n", outputFile);

    clock_t end = clock();
    double time_total = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Tempo Total: %lf segundos\n", time_total);
    
    return 0;
}
