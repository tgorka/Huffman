#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * Struktura do drzewa Huffmana.
 */
typedef struct TDrzewo TDrzewo;
struct TDrzewo {
    char slowo;
    int waga;
    //Do poruszania się kompresji/dekompresji
    TDrzewo *lewy;
    TDrzewo *prawy;
};

/**
 * Zebranie statystyk wystepowania znakow w posortowanej tablicy niepowtarzalnych znakow
 */
int* statystyki(FILE *statFile) {
  /* instrukcje */
  return NULL;
}

/**
 * Stworzenie drzewa wystapien uzywanego pozniej do kompresji/dekompresji na podstawie podanych statystyk
 */
TDrzewo* stworzTDrzewo(int* statystyki) {
  /* instrukcje */
  return NULL;
}

/**
 * Kompresuj ciag wejsciowy do binarnego wyjsciowego.
 */
void kompresuj(TDrzewo *tdrzewo, FILE *in, FILE *out) {
  /* instrukcje */
}

/**
 * Dekompresuj binarny ciag wejsciowy do ciagu wyjsciowego.
 */
void dekompresuj(TDrzewo *tdrzewo, FILE *in, FILE *out) {
  /* instrukcje */
}

/**
 * Glowne wywolanie aplikacji
 */
int main() {
    printf("Hoffman 1.0.0. Algorytm to kompresji/dekompresji.\n");


    printf("Zbieranie statystyk\n");
    FILE *statFile = fopen("test/podstawowy.txt", "r");
    if (!statFile) {
        printf("Problem z plikiem statystyk\n");
        return 1;
    }

    int *stat = statystyki(statFile);

    fclose(statFile);


    printf("Tworzenie drzewa\n");
    TDrzewo *tdrzewo = stworzTDrzewo(stat);


    printf("Rozpoczecie kompresji\n");
    FILE *inputCompressionFile = fopen("test/podstawowy.txt", "r");
    FILE *outputCompressionFile = fopen("podstawowy.bin", "wb");
    if (!inputCompressionFile || !outputCompressionFile) {
        printf("Problem z plikiem  in/out podczas kompresji\n");
        return 1;
    }

    kompresuj(tdrzewo, inputCompressionFile, outputCompressionFile);

    fclose(inputCompressionFile);
    fclose(outputCompressionFile);


    printf("Rozpoczecie dekompresji\n");
    FILE *inputDecompressionFile = fopen("podstawowy.bin", "rb");
    FILE *outputDecompressionFile = fopen("podstawowy_out.txt", "w");
    if (!inputDecompressionFile || !outputDecompressionFile) {
        printf("Problem z plikiem  in/out podczas dekompresji\n");
        return 1;
    }

    dekompresuj(tdrzewo, inputDecompressionFile, outputDecompressionFile);

    fclose(inputDecompressionFile);
    fclose(outputDecompressionFile);


    printf("Zakonczenie dzialania.\n");
    return 0;
}
