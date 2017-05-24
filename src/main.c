#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int DEBUG = 0;

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

int cmpfunc (const void *a, const void *b) {
   return ( ((TDrzewo*)b)->wystapienia - ((TDrzewo*)a)->wystapienia );
}

TDrzewo* posortowaneWezly(int *stat, int *size) {
    *size = 0;
    for(int i = 0; i < 128; i++) {
        if (stat[i] > 0) {
            *size = *size+1;
        }
    }
    TDrzewo *res = (TDrzewo*) calloc(*size, sizeof(TDrzewo));

    int j = 0;
    for(int i = 0; i < 128; i++) {
        if (stat[i] > 0) {
            res[j].wystapienia = stat[i];
            res[j].slowo = i;
            res[j].prawy = NULL;
            res[j].lewy = NULL;
            j++;
        }
    }
    qsort(res, *size, sizeof(TDrzewo), cmpfunc);
    return res;
}

void zaalokujTDrzewo(TDrzewo* stat, int statSize, int i, TDrzewo *root) {
    if (i >= statSize) {
        return;
    }
    if (DEBUG) {
        printf("allocating for word [%c]\n", i);
    }

    TDrzewo *galaz = malloc(sizeof(TDrzewo));
    galaz->wystapienia = 0;
    galaz->slowo = -1;
    galaz->prawy = NULL;
    galaz->lewy = NULL;

    root->prawy = &stat[i];
    root->lewy = galaz;
    zaalokujTDrzewo(stat, statSize, i + 1, galaz);
}

void uwolnijTDrzewo(TDrzewo *root) {
    if (!root) {
        return;
    }
    uwolnijTDrzewo(root->lewy);
    //uwolnijTDrzewo(root->prawy);
    free(root);
}

/**
 * Stworzenie drzewa wystapien uzywanego pozniej do kompresji/dekompresji na podstawie podanych statystyk
 */
TDrzewo* stworzTDrzewo(TDrzewo* stat, int statSize) {
    TDrzewo *root = malloc(sizeof(TDrzewo));
    root->wystapienia = 0;
    root->slowo = -1;
    root->prawy = NULL;
    root->lewy = NULL;

    zaalokujTDrzewo(stat, statSize, 0, root);
    return root;
}

int writeByte(TDrzewo *root, FILE *out, int c, char* buffer, int bufferIndex, long *compressedLen) {
    if (bufferIndex > 7) {
        char i = (char) strtol(buffer, (char **)NULL, 2);
        *compressedLen = *compressedLen+1;
        if (DEBUG) {
            printf("Compressing storing %ld char: [%d] byte: [%s]\n", *compressedLen, i, buffer);
        }
        fputc(i, out);
        bufferIndex = 0;
    }
    if (root->prawy && root->prawy->slowo == c) {
        // write 1
        if (DEBUG) {
            printf("Compressing coding character [%c]\n", c);
        }
        buffer[7-bufferIndex] = '1';
        bufferIndex++;
    } else if (root->lewy) {
        // write 0
        buffer[7-bufferIndex] = '0';
        bufferIndex++;
        bufferIndex = writeByte(root->lewy, out, c, buffer, bufferIndex, compressedLen);
    } else {
        printf("WARNING: not word found [%c] value [%d]\n", c, c);
    }
    return bufferIndex;
}

/**
 * Kompresuj ciag wejsciowy do binarnego wyjsciowego.
 */
float kompresuj(TDrzewo *tdrzewo, FILE *in, FILE *out) {
    int c;
    long originalLen = 0, compressedLen = 0;
    char* buffer = calloc(8, sizeof(char));
    int bufferIndex = 0;
    while ((c = getc(in)) != EOF) {
        originalLen++;
        if (c >= 0 && c < 128) { // wspieraj tylko normalne znaki z zakresu ASCI
            bufferIndex = writeByte(tdrzewo, out, c, buffer, bufferIndex, &compressedLen);
        }
    }
    for (int i = bufferIndex; i <= 7; i++) {
        buffer[7-i] = '0';
    }
    char i = (char) strtol(buffer, (char **)NULL, 2);
    compressedLen = compressedLen+1;
    if (DEBUG) {
        printf("Compressing storing %ld char: [%d] byte: [%s]\n", compressedLen, i, buffer);
        buffer[1] = '0'; // hotfix for waiting printf write buffer before free it from the memory
    }
    fputc(i, out);
    free(buffer);

    if (originalLen == 0 || compressedLen == 0) {
        return 0.0;
    } else {
        return 100.0 * compressedLen / originalLen;
    }
}

TDrzewo* readByte(TDrzewo *root, TDrzewo *aktualnyWezel, FILE *out, int c, long *originalLen) {
    if (!aktualnyWezel) {
        return root;
    }
    if ('1' == c) {
        // write slowo
        *originalLen = *originalLen + 1;
        fputc(aktualnyWezel->prawy->slowo, out);
        return root;
    } else if ('0' == c && aktualnyWezel->lewy) {
        // write next
        return aktualnyWezel->lewy;
    }
    // at the end when byte was filled by 0 only
    return root;
}

char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

char *stringPadRight(char *buffer, char *tmp) {
    int len = (int) strlen(tmp);
    for (int i = 0; i < 8; i++) {
        if (i < len) {
            buffer[7 - i] = tmp[len - 1 - i];
        } else {
            buffer[7 - i] = '0';
        }
    }
    return buffer;
}

/**
 * Dekompresuj binarny ciag wejsciowy do ciagu wyjsciowego.
 */
void dekompresuj(TDrzewo *tdrzewo, FILE *in, FILE *out) {
    int c;
    char* tmp = calloc(8, sizeof(char));
    char* buffer = calloc(8, sizeof(char));
    long originalLen = 0, compressedLen = 0;
    int bufferIndex = 0;
    TDrzewo *aktualnyWezel = tdrzewo;
    while ((c = getc(in)) != EOF) {
        compressedLen++;
        itoa(c, tmp, 2);
        stringPadRight(buffer, tmp);
        if (DEBUG) {
            printf("Decompressing: restoring %ld char:[%d] bytes:[%s]\n", compressedLen, c, buffer);
        }
        for (int i = 7; i >= 0; i--) {
            aktualnyWezel = readByte(tdrzewo, aktualnyWezel, out, buffer[i], &originalLen);
        }
    }
    free(tmp);
    free(buffer);
}

/**
 * Glowne wywolanie aplikacji
 */
int main() {
    printf("Hoffman 1.0.0. Algorytm to kompresji/dekompresji.\n");


    printf("Zbieranie statystyk\n");
    FILE *statFile = fopen("test/podstawowy.txt", "r");
    if (!statFile) {
        printf("WARNING: Problem z plikiem statystyk\n");
        return 1;
    }

    int *stat = statystyki(statFile);
    int statSize = 0;
    TDrzewo *sortedStat = posortowaneWezly(stat, &statSize);

    fclose(statFile);
    printf("Zebrane statystyki %d znakow:", statSize);
    for (int i = 0; i < statSize; i++) {
        printf("[%c][%d]x%d ", sortedStat[i].slowo, sortedStat[i].slowo, sortedStat[i].wystapienia);
    }
    printf("\n");


    printf("Tworzenie drzewa\n");
    TDrzewo *tdrzewo = stworzTDrzewo(sortedStat, statSize);
    free(stat);


    printf("Rozpoczecie kompresji\n");
    FILE *inputCompressionFile = fopen("test/podstawowy.txt", "r");
    FILE *outputCompressionFile = fopen("podstawowy.bin", "wb");
    if (!inputCompressionFile || !outputCompressionFile) {
        printf("Problem z plikiem  in/out podczas kompresji\n");
        return 1;
    }

    float ratio = kompresuj(tdrzewo, inputCompressionFile, outputCompressionFile);

    fclose(inputCompressionFile);
    fclose(outputCompressionFile);
    printf("Zakonczenie kompresji wspolczynnik: %.2f %%\n", ratio);


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

    free(sortedStat);
    uwolnijTDrzewo(tdrzewo); // uwolnij pamiec
    printf("Zakonczenie dzialania.\n");
    return 0;
}
