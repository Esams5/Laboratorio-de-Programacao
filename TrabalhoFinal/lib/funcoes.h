#include <stdio.h>
#include <stdlib.h>

struct pgm {
    int tipo;
    int colunas;
    int linhas;
    int maxvalue;
    unsigned char **pData;
};

void readPGMImage(struct pgm *pio, char *filename);
void writePGMImage(struct pgm *pio, char *filename);
void viewPGMImage(struct pgm *pio);
void cuttingPGMImage(struct pgm *pio, char *filename);
void applyFilter(struct pgm *pio);