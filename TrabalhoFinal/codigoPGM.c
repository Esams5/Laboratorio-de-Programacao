#include <stdio.h>
#include <stdlib.h>

struct pgm {
    int tipo;
    int colunas;
    int linhas;
    int maxvalue;
    unsigned char **pData;
};

#define TAMREC 100
#define QUANTREC 100

void readPGMImage(struct pgm *, char *);
void viewPGMImage(struct pgm *);
void writePGMImage(struct pgm *, char *);
void cuttingPGMImage(struct pgm *, char *);
void applyFilter(struct pgm *);

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

void readPGMImage(struct pgm *pio, char *filename) {
    FILE *fp;
    char ch;

    if (!(fp = fopen(filename, "rb"))) {
        perror("Erro.");
        exit(1);
    }

    if ((ch = getc(fp)) != 'P') {
        puts("A imagem fornecida não está no formato pgm");
        exit(2);
    }

    pio->tipo = getc(fp) - 48;

    fseek(fp, 1, SEEK_CUR);

    while ((ch = getc(fp)) == '#') {
        while ((ch = getc(fp)) != '\n');
    }

    fseek(fp, -1, SEEK_CUR);

    fscanf(fp, "%d %d", &pio->colunas, &pio->linhas);
    if (ferror(fp)) {
        perror(NULL);
        exit(3);
    }
    fscanf(fp, "%d", &pio->maxvalue);
    fseek(fp, 1, SEEK_CUR);

    // Cria um ponteiro para ponteiro para o salvamento de dados da matriz

    pio->pData = (unsigned char **)malloc(pio->linhas * sizeof(unsigned char *));
    for (int i = 0; i < pio->linhas; i++) {
        pio->pData[i] = (unsigned char *)malloc(pio->colunas * sizeof(unsigned char));
    }

    switch (pio->tipo) {
        case 2:
            puts("Lendo imagem PGM (dados em texto)");
            for (int k = 0; k < pio->linhas; k++) {
                for (int i = 0; i < pio->colunas; i++) {
                    fscanf(fp, "%hhu", &(*(*(pio->pData + k) + i)));
                }
            }
            break;
        case 5:
            puts("Lendo imagem PGM (dados em binário)");
            for (int i = 0; i < pio->linhas; i++) {
                fread(pio->pData[i], sizeof(unsigned char), pio->colunas, fp);
            }
            break;
        default:
            puts("Não está implementado");
    }

    fclose(fp);
}

void writePGMImage(struct pgm *pio, char *filename) {
    FILE *fp;
    char ch;

    if (!(fp = fopen(filename, "wb"))) {
        perror("Erro.");
        exit(1);
    }

    fprintf(fp, "%s\n", "P5");
    fprintf(fp, "%d %d\n", pio->colunas, pio->linhas);
    fprintf(fp, "%d\n", 255);

    for (int i = 0; i < pio->linhas; i++) {
        fwrite(*(pio->pData + i), sizeof(unsigned char), pio->colunas, fp);
    }

    fclose(fp);
}

void viewPGMImage(struct pgm *pio) {
    printf("Tipo: %d\n", pio->tipo);
    printf("Dimensões: [%d %d]\n", pio->colunas, pio->linhas);
    printf("Max: %d\n", pio->maxvalue);

    for (int i = 0; i < pio->linhas; i++) {
        for (int j = 0; j < pio->colunas; j++) {
            printf("%2hhu ", *(*(pio->pData + i) + j));
        }
        printf("\n");
    }
    printf("\n");
}

void cuttingPGMImage(struct pgm *pio, char *filename) {

    // Cria 100 recortes com base na estrutura pgm
    struct pgm Recortes[QUANTREC];

    for (int i = 0; i < QUANTREC; i++) {
        Recortes[i].colunas = TAMREC;
        Recortes[i].linhas = TAMREC;
    }

    // Faz a alocação de cada um desses recortes na memória
    for (int i = 0; i < QUANTREC; i++) {
        Recortes[i].pData = (unsigned char **)malloc(Recortes[i].linhas * sizeof(unsigned char *));

        for (int j = 0; j < Recortes[i].linhas; j++) {
            Recortes[i].pData[j] = malloc(Recortes[i].colunas * sizeof(unsigned char));
        }
    }

    // Ele fará o recorte da segunte maneira, escolherá um ponto de partida inicial, indo de 0 até o tamanho da imagem - o tamanho do recorte, extendendo para
    // o x e o y para direita e para baixo, dessa maneira, ele nunca será afetado pelas bordas.
    int xPartida;
    int yPartida;
    for (int i = 0; i < QUANTREC; i++) {
        xPartida = rand() % (pio->colunas - TAMREC + 1);
        yPartida = rand() % (pio->linhas - TAMREC + 1);

        for (int j = 0; j < Recortes[i].linhas; j++) {
            for (int k = 0; k < Recortes[i].colunas; k++) {
                Recortes[i].pData[j][k] = (pio->pData[yPartida + j][xPartida + k]);
            }
        }
        applyFilter(&Recortes[i]);

        // Com essa linha de código, ele criará os 100 recortes na pasta ./images, escrevendo o que há no Recorte respectivo do laço em cada arquivo respectivo
        char outputFileName[100];
        snprintf(outputFileName, sizeof(outputFileName), "./images/recorte%d.pgm", i+1);
        writePGMImage(&Recortes[i], outputFileName);
    }
}

void applyFilter(struct pgm *pio) {

    // estrutura temporária para os calculos
    struct pgm temp;
    temp.colunas = pio->colunas;
    temp.linhas = pio->linhas;

    // alocação do dados na estrutura temporária
    temp.pData = (unsigned char **)malloc(temp.linhas * sizeof(unsigned char *));
    for (int i = 0; i < temp.linhas; i++) {
        temp.pData[i] = malloc(temp.colunas * sizeof(unsigned char));
    }

    for (int i = 0; i < pio->linhas; i++) {
        for (int j = 0; j < pio->colunas; j++) {
            int soma = 0;
            int count = 0;
            
            // calculo do filtro de media
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    int lin = i + m;
                    int col = j + n;

                    if (lin >= 0 && lin < pio->linhas && col >= 0 && col < pio->colunas) {
                        soma += pio->pData[lin][col];
                        count++;
                    }
                }
            }
        
            temp.pData[i][j] = (unsigned char)(soma / count);
        }
    }
    
    // inserindo o que foi escrito na estrutura temporaria na matriz original
    for (int i = 0; i < pio->linhas; i++) {
        for (int j = 0; j < pio->colunas; j++) {
            pio->pData[i][j] = temp.pData[i][j];
        }
    }

    // liberando a memória
    for (int i = 0; i < temp.linhas; i++) {
        free(temp.pData[i]);
    }
    free(temp.pData);
}
