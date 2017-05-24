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
    int wystapienia;
    //Do poruszania się kompresji/dekompresji
    TDrzewo *lewy;
    TDrzewo *prawy;
};

/**
 * Zebranie statystyk wystepowania znakow w posortowanej tablicy niepowtarzalnych znakow
 */
int* statystyki(FILE *statFile) {
    // przydziel pamiec do tablicy wszystkich mozliwych znakow z wartoscia domyslna 0
    int *stat = (int*) calloc(128, sizeof(int));
    int c;
    while ((c = getc(statFile)) != EOF) {
        if (c >= 0 && c < 128) { // wspieraj tylko normalne znaki z zakresu ASCI
            stat[c] += 1;
        }
    }
    return stat;
}

void zaalokujTDrzewo(int* stat, int i, TDrzewo *root) {
    if (i >= 128) {
        return;
    }

    TDrzewo *wezel = malloc(sizeof(TDrzewo));
    wezel->wystapienia = stat[i];
    wezel->slowo = i;
    wezel->prawy = NULL;
    wezel->lewy = NULL;

    TDrzewo *galaz = malloc(sizeof(TDrzewo));
    galaz->wystapienia = 0;
    galaz->slowo = -1;
    galaz->prawy = NULL;
    galaz->lewy = NULL;

    root->prawy = wezel;
    root->lewy = galaz;
    zaalokujTDrzewo(stat, i + 1, galaz);
}

void uwolnijTDrzewo(TDrzewo *root) {
    if (!root) {
        return;
    }
    uwolnijTDrzewo(root->lewy);
    uwolnijTDrzewo(root->prawy);
    free(root);
}

/**
 * Stworzenie drzewa wystapien uzywanego pozniej do kompresji/dekompresji na podstawie podanych statystyk
 */
TDrzewo* stworzTDrzewo(int* stat) {
    TDrzewo *root = malloc(sizeof(TDrzewo));
    root->wystapienia = 0;
    root->slowo = -1;
    root->prawy = NULL;
    root->lewy = NULL;

    zaalokujTDrzewo(stat, 0, root);
    return root;
}

void writeByte(TDrzewo *root, FILE *out, int c) {
    if (root->prawy && root->prawy->slowo == c) {
        // write 1
        fputc('1', out);
    } else if (root->lewy) {
        // write 0
        fputc('0', out);
        writeByte(root->lewy, out, c);
    }
}

/**
 * Kompresuj ciag wejsciowy do binarnego wyjsciowego.
 */
void kompresuj(TDrzewo *tdrzewo, FILE *in, FILE *out) {
    int c;
    while ((c = getc(in)) != EOF) {
        if (c >= 0 && c < 128) { // wspieraj tylko normalne znaki z zakresu ASCI
            writeByte(tdrzewo, out, c);
        }
    }
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
    printf("Zebrane statystyki:");
    for (int i = 0; i < 128; i++) {
        if (stat[i] > 0) {
            printf("[%c]x%d ", i, stat[i]);
        }
    }
    printf("\n");


    printf("Tworzenie drzewa\n");
    TDrzewo *tdrzewo = stworzTDrzewo(stat);
    free(stat);


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

    uwolnijTDrzewo(tdrzewo); // uwolnij pamiec
    printf("Zakonczenie dzialania.\n");
    return 0;
}
