#include <time.h>
#include "lib/funcoes.h"
#include "lib/funcoes2.h"

/* 
    Aluno :  Toda a equipe
    Avaliacao 04 : Trabalho Final
    04.505.23−2023.1 − Prof.Daniel Ferreira
    Compilador : GCC versão 11.3.0
 */

#define TAMREC 100
#define QUANTREC 100
#define MAX_FILENAME_LENGTH 257
#define NUM_SUBIMAGES 100


int main(int argc, char *argv[]) {

    struct pgm img;

    if (argc != 3) {
        printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n", argv[0]);
        exit(1);
    }

    readPGMImage(&img, argv[1]);

    writePGMImage(&img, argv[2]);

    viewPGMImage(&img);

    cuttingPGMImage(&img, argv[1]);


    char subImageDirectory[MAX_FILENAME_LENGTH];
    char originalImageFile[MAX_FILENAME_LENGTH];
    char outputFile[MAX_FILENAME_LENGTH];
    
    // Solicitando ao usuário o diretório das sub-imagens, o caminho da imagem original e o nome do arquivo de saída
    printf("Digite o diretório das sub-imagens: ");
    scanf("%s", subImageDirectory);
    
    printf("Digite o caminho da imagem original: ");
    scanf("%s", originalImageFile);
    
    printf("Digite o nome do arquivo de saída: ");
    scanf("%s", outputFile);
    clock_t begin = clock();
    // Chamando a função para realizar a busca das sub-imagens e salvar as coordenadas no arquivo de saída
    searchSubImages(subImageDirectory, originalImageFile, outputFile);
    
    printf("Busca concluída. Coordenadas salvas no arquivo: %s\n", outputFile);
    clock_t end = clock();
    double time_total = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Tempo Total: %lf segundos\n", time_total);

   

    return 0;
}

