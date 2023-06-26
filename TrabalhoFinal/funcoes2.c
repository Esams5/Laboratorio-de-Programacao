#include "lib/funcoes2.h"

/* 
    Aluno : Samuel Esdras da Silva Souza / Cêsar de Alcantra Neto
    Matricula : 20222045050263 / 20202045050110
    Avaliacao 04 : Trabalho Final
    04.505.23−2023.1 − Prof.Daniel Ferreira
    Compilador : GCC versão 11.3.0
 */

#define MAX_FILENAME_LENGTH 257
#define NUM_SUBIMAGES 100



void freeImage(Imagens* image) {
    for (int i = 0; i < image->height; i++) {
        free(image->data[i]);
    }
    free(image->data);
}

Imagens loadImage(const char* filename) {
    Imagens image;
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

double calculateMeanSquaredError(const Imagens* image, const Imagens* subImage, int x, int y) {
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
    Imagens originalImage = loadImage(originalImageFile);
    
    FILE* file = fopen(outputFile, "w");
    
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", outputFile);
        exit(1);
    }
    
    fclose(file);
    
    DIR *dir;
    struct dirent *ent;
    char subImageFile[MAX_FILENAME_LENGTH];
    
    // Abrindo o diretório de sub-imagens
    if ((dir = opendir(subImageDirectory)) != NULL) {
        // Iterando sobre cada arquivo no diretório
        while ((ent = readdir(dir)) != NULL) {
            // Verificando se é um arquivo regular
            if (ent->d_type == DT_REG) {
                // Construindo o caminho completo para a sub-imagem
                sprintf(subImageFile, "%s/%s", subImageDirectory, ent->d_name);
                // Carregando a sub-imagem
                Imagens subImage = loadImage(subImageFile);
                
                // Calculando as dimensões de busca na imagem original
                int searchWidth = originalImage.width - subImage.width;
                int searchHeight = originalImage.height - subImage.height;
                
                // Variáveis para armazenar as coordenadas da melhor correspondência
                double minMSE = -1.0;
                int minX = -1, minY = -1;
                
                // Iterando sobre as possíveis posições de busca na imagem original
                for (int y = 0; y <= searchHeight; y++) {
                    for (int x = 0; x <= searchWidth; x++) {
                        // Calculando o erro médio quadrático entre a sub-imagem e a região correspondente na imagem original
                        double mse = calculateMeanSquaredError(&originalImage, &subImage, x, y);
                        
                        // Atualizando as coordenadas da melhor correspondência
                        if (minMSE == -1.0 || mse < minMSE) {
                            minMSE = mse;
                            minX = x;
                            minY = y;
                        }
                    }
                }
                
                // Salvando as coordenadas da melhor correspondência no arquivo de saída
                saveCoordinates(outputFile, ent->d_name, minX, minY);
                
                // Liberando a memória alocada para a sub-imagem
                freeImage(&subImage);
            }
        }
        // Fechando o diretório
        closedir(dir);
    } else {
        printf("Erro ao abrir o diretório: %s\n", subImageDirectory);
        exit(1);
    }
    
    // Liberando a memória alocada para a imagem original
    freeImage(&originalImage);
}
