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

int* statystyki() {
  /* instrukcje */
  return NULL;
}

TDrzewo* stworzTDrzewo() {
  /* instrukcje */
  return NULL;
}

void kompresuj(TDrzewo *tdrzewo) {
  /* instrukcje */
}

void dekompresuj(TDrzewo *tdrzewo) {
  /* instrukcje */
}

int main() {
    printf("Hoffman 1.0.0. Algorytm to kompresji/dekompresji.\n");

    printf("Zbieranie statystyk\n");
    int *stat = statystyki();

    printf("Tworzenie drzewa\n");
    TDrzewo *tdrzewo = stworzTDrzewo();

    printf("Rozpoczecie kompresji\n");
    kompresuj(tdrzewo);

    printf("Rozpoczecie dekompresji\n");
    dekompresuj(tdrzewo);

    printf("Zakonczenie dzialania.\n");

    return 0;
}
