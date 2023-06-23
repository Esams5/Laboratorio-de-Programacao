#include <stdio.h>
#include <stdlib.h>
#include "lib/funcoes.h"



#define TAMREC 100
#define QUANTREC 100


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

    return 0;
}

