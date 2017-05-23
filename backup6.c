#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct TDrzewo TDrzewo;         //Struktura do drzewa Huffmana.
struct TDrzewo {
    char slowo;
    int waga;
    //Do poruszania siê po prefixach.
    TDrzewo *prefix;
    TDrzewo *postfix;
    //Do poruszania siê od pnia.
    TDrzewo *lewy;
    TDrzewo *prawy;
    TDrzewo *korzen;
    //Do wzrostu drzewa.
    TDrzewo *nast;
    TDrzewo *pop;
};
typedef struct TCiag TCiag;         //Struktura do bitów z prefixów.
struct TCiag{
    char znak;
    TCiag *nast;
};

typedef struct TLista TLista;
struct TLista{
    char slowo;
    TCiag *skrot;
    TLista *nast;
};

// -------------  FUNKCJE DO KOMPRESJI --------------//

TDrzewo* KPobierzElementy(char *nazwa); //generuje pierwsze elementy drzewa przy kompresji.
int KSortuj(TDrzewo *Drzewo);           //sortuje elementy rosn¹co wzglêdem wagi po *nast-ach - szykowanie do generowania drzewa.
TDrzewo* KGalaz(TDrzewo *Drzewo);       //Generuje pijedyncza galaz drzewa.
TDrzewo* KDrzewGen(TDrzewo *Drzewo);    //generuje kompletne drzewo Huffmana.
TDrzewo* KPref(TDrzewo *Drzewo);        //Pointer do prefixów z drzewa Huffmana.
TDrzewo* KZnajdz(TDrzewo *Prefix, char slowo);  //Znajduje s³owo w drzewie i zwraca jego prefix.
TCiag* KZnajdzSkrot(TDrzewo *Prefix);   //Generuje skrót do danego prefixu z drzewa Huffmana;
char KDlogoscC(TCiag *Ciag);           //Generuje d³ugoœæ srótu w postaci znaku.

int KZapiszSlownik(TDrzewo *Prefix, char *nazwa);  //Zapisuje s³ownik do pliku.
TCiag* KompresujPlikDoCiaguB(TDrzewo *Prefix, char *nazwa);   //Generuje ci¹g bitowy zakompresowanych danych.
TCiag* KCiagDanych(TCiag *CiagB);           //Generucje ci¹g znaków do zapisu (KOMPRESJA).
int KZapiszPlk(TCiag *Ciag, char *nazwa);    //Zapisuje Ci¹g do pliku.

int Kompresuj (char *nazwaOdcz, char *NazwaZap);        //Kompresja metod¹ Huffmana.

//  -------------- FUNKCJE DO ODCZYTU -----------------//

TLista* OdczElemZPlk(FILE *plk);      //Odczytuje s³ownik z archiwum.

int PorownajCiagi(TCiag *ciag1, TCiag *ciag2);  //Poruwnuje 2 ci¹gi bitowe.
TCiag* WczytajCiag(FILE *plk);      //Wczytuje ci¹g bajtów.
TCiag* DCiagnaBit(TCiag *Ciag);     //Generuje ci¹g bitów.
char DZnajdzZnak(TLista *Lista, TCiag *CiagB);   //Znajduje znak w tablicy.
int DZapiszZnak(char znak, FILE *plk);          //Zapisuje znak do pliku.
int DZapDekom(char *nazwaZap, TCiag *Ciag, TLista *Lista);     //Zapisuje zdekompresowany plik.

int Dekompresuj(char *nazwaOdcz, char *nazwaZap);        //Dekompresuje Huffmanem.

void Pomoc();       //Katalog pomocy.

int main(int argc, char **argv)
{
    if ((argc==2)&&(strcmp(argv[1],"-h")==0)) Pomoc();
    else if ((argc==6)&&(strcmp(argv[1], "-f")==0)&&(strcmp(argv[3], "-o")==0)&&(strcmp(argv[5], "-c")==0)) {
        if (Kompresuj(argv[4], argv[2])) {
            printf("Blad. \n");
            Pomoc();
        }
    }
    else if ((argc==6)&&(strcmp(argv[1], "-f")==0)&&(strcmp(argv[3], "-o")==0)&&(strcmp(argv[5], "-d")==0)) {
        if (Dekompresuj(argv[4], argv[2])) {
            printf("Blad. \n");
            Pomoc();
        }
    }
    else {
        printf("Blad wpisywania danych wejsciowych, oto plik pomocy :\n");
        Pomoc();
    }


    return 0;
}

void Pomoc(){
    printf("Katalog pomocy :\n");
    printf("Mozliwe polecenia :\n\n");
    printf("-h - Katalog pomocy.\n");
    printf("-f [plik zapisu] -o [plik odczytu] -c -kompresja pliku.\n");
    printf("-f [plik zapisu] -o [plik odczytu] -d - dekompresja pliku.\n");
}

TDrzewo* KPobierzElementy(char *nazwa){
    FILE *plk;
    TDrzewo *Drzewo;
    TDrzewo *BDrzewo;
    char znak;
    int i;

    if ((plk=fopen(nazwa, "rb"))==NULL) return NULL;
    if ((fread(&znak, sizeof(znak),1,plk))==0) return NULL;
    if ((Drzewo=malloc(sizeof(TDrzewo)))==NULL) return NULL;
    BDrzewo=Drzewo;
    //Okreœlam pierwszy element.
    Drzewo->slowo=znak;
    Drzewo->korzen=NULL;
    Drzewo->prefix=NULL;
    Drzewo->postfix=NULL;
    Drzewo->lewy=NULL;
    Drzewo->nast=NULL;
    Drzewo->pop=NULL;
    Drzewo->prawy=NULL;
    Drzewo->waga=1;
    //Wczytanie reszty elementów z pliku bitowego.
    while (fread(&znak,sizeof(znak),1,plk)!=0){
        i=0;
        while (i==0){
            //Jeœli slowo jest wœród elementów.
            while(Drzewo->nast!=NULL){
                if (Drzewo->slowo==znak){
                    Drzewo->waga++;
                    i++;
                }
                Drzewo=Drzewo->nast;
            }
            if (i==0) {
                //ustawianie cech nowych elementów z pliku.
                if ((Drzewo->nast=malloc(sizeof(TDrzewo)))==NULL) return NULL;
                Drzewo->nast->korzen=NULL;
                Drzewo->nast->lewy=NULL;
                Drzewo->nast->prawy=NULL;
                Drzewo->nast->slowo=znak;
                Drzewo->nast->waga=1;
                Drzewo->nast->nast=NULL;
                Drzewo->prefix=Drzewo->nast;
                Drzewo->nast->prefix=NULL;
                Drzewo->nast->postfix=Drzewo;
                Drzewo->nast->pop=Drzewo;
                Drzewo=Drzewo->nast;
                i++;
            }
        Drzewo=BDrzewo;
        }
    }
    return Drzewo;
}

int KSortuj(TDrzewo *Drzewo){
    TDrzewo *A, *B, *C, *D;
    int i;

    i=0;
    while (i==0){
        i++;
        while(Drzewo->nast!=NULL){
            if ((Drzewo->waga)>(Drzewo->nast->waga)){
                A=Drzewo->pop;
                B=Drzewo->nast;
                C=Drzewo;
                D=Drzewo->nast->nast;

                if (D!=NULL)D->pop=C;
                C->pop=B;
                B->pop=A;

                if (A!=NULL)A->nast=B;
                B->nast=C;
                C->nast=D;

                i=0;
            }
            if (Drzewo->nast!=NULL) Drzewo=Drzewo->nast;
        }
        while (Drzewo->pop!=NULL) Drzewo=Drzewo->pop;
    }
    return 0;
}

TDrzewo* KGalaz(TDrzewo *Drzewo){
    TDrzewo *temp;
    while (Drzewo->pop!=NULL) Drzewo=Drzewo->pop;
    if (Drzewo->nast==NULL) return NULL; //drzewko jest kompletne.
    if ((temp=malloc(sizeof(TDrzewo)))==NULL) return NULL;

    temp->prefix=NULL;
    temp->postfix=NULL;
    temp->korzen=NULL;
    temp->lewy=Drzewo;
    temp->nast=Drzewo->nast->nast;
    temp->pop=Drzewo->pop;
    temp->prawy=Drzewo->nast;
    temp->waga=Drzewo->waga+Drzewo->nast->waga;
    Drzewo->nast->nast=NULL;
    Drzewo->korzen=temp;
    Drzewo->nast->korzen=temp;
    Drzewo->pop=NULL;
    Drzewo->nast->pop=NULL;
    Drzewo->nast=NULL;

    return temp;
}

TDrzewo* KDrzewGen(TDrzewo *Drzewo){
    TDrzewo *BDrzewo=Drzewo;
    while (BDrzewo->nast!=NULL){
        KSortuj(BDrzewo);
        BDrzewo=KGalaz(BDrzewo);
    }
    return BDrzewo;
}

TDrzewo* KPref(TDrzewo *Drzewo){
    TDrzewo *BDrzewo=Drzewo;

    while (BDrzewo->lewy!=NULL) BDrzewo=BDrzewo->lewy;
    while (BDrzewo->postfix!=NULL) BDrzewo=BDrzewo->postfix;
    return BDrzewo;
}

TDrzewo* KZnajdz(TDrzewo *Prefix, char slowo){
    TDrzewo *BPrefix=Prefix;

    while (BPrefix!=NULL){
        if (slowo==BPrefix->slowo) return BPrefix;
        BPrefix=BPrefix->prefix;
    }
    return NULL;
}

TCiag* KZnajdzSkrot(TDrzewo *Prefix){
    TDrzewo *BPrefix=Prefix;
    TCiag *Ciag=NULL;
    TCiag *BCiag=NULL;

    while (BPrefix->korzen!=NULL){
        if (Ciag==NULL){
            if ((Ciag=malloc(sizeof(TCiag)))==NULL) return NULL;
            BCiag=Ciag;
            Ciag->nast=NULL;
        }
        else {
            if ((Ciag->nast=malloc((sizeof(TCiag))))==NULL) return NULL;
            Ciag=Ciag->nast;
            Ciag->nast=NULL;
        }

        if (BPrefix->korzen->lewy==BPrefix) {
            Ciag->znak='1';
        }
        else Ciag->znak='0';
        BPrefix=BPrefix->korzen;
    }
    Ciag=BCiag;
    return Ciag;
}

char KDlogoscC(TCiag *Ciag){
    TCiag *BCiag=Ciag;
    char i=0;
    while (BCiag!=NULL){
        i++;
        BCiag=BCiag->nast;
    }
    return i;
}

int KZapiszSlownik(TDrzewo *Prefix, char *nazwa){
    FILE *plk=NULL;
    TDrzewo *BPrefix=Prefix;
    TCiag *Ciag=NULL;
    TCiag *BCiag=NULL;
    char znak;

    if (BPrefix==NULL) return 1;

    if ((plk=fopen(nazwa, "wb"))==NULL)return 1;
    fseek(plk, 0,0);

    while (BPrefix!=NULL){
        if ((Ciag=malloc(sizeof(TCiag)))==NULL) return 1;
        Ciag=KZnajdzSkrot(BPrefix);
        BCiag=Ciag;
        znak=KDlogoscC(Ciag);
        if ((fwrite(&znak,sizeof(znak),1,plk))==0) return 1;
        if ((fwrite(&BPrefix->slowo, sizeof(BPrefix->slowo),1,plk))==0) return 1;
        while (BCiag!=NULL){
            if ((fwrite(&BCiag->znak, sizeof(BCiag->znak),1,plk))==0) return 1;
            BCiag=BCiag->nast;
        }
        free(Ciag);
        BPrefix=BPrefix->prefix;
    }
    znak=0;
    if ((fwrite(&znak,sizeof(znak),1,plk))==0) return 1;
    fclose(plk);
    return 0;
}

TCiag* KompresujPlikDoCiaguB(TDrzewo *Prefix, char *nazwa){
    FILE *plk=NULL;
    char znak;
    TDrzewo *Skrot=NULL;
    TCiag *Ciag=NULL;
    TCiag *BCiag=NULL;
    TCiag *temp=NULL;
    TCiag *Btemp;

    if ((plk=fopen(nazwa, "rb"))==NULL) return NULL;
    fseek(plk, 0, 0);

    while ((fread(&znak, sizeof(znak),1, plk))!=0){
        Skrot=KZnajdz(Prefix, znak);

        temp=KZnajdzSkrot(Skrot);
        Btemp=temp;

        //Dopisywanie ci¹gu temp do Ci¹gu Ci¹g.
        while (temp!=NULL){
            if (Ciag==NULL) {
                if ((Ciag=malloc(sizeof(TCiag)))==NULL) return NULL;
                BCiag=Ciag;
                Ciag->nast=NULL;
                Ciag->znak=temp->znak;
                temp=temp->nast;
            }
            else {
                if ((Ciag->nast=malloc(sizeof(TCiag)))==NULL) return NULL;
                Ciag=Ciag->nast;
                Ciag->nast=NULL;
                Ciag->znak=temp->znak;
                temp=temp->nast;
            }
        }
        free(Btemp);
    }
    fclose(plk);
    return BCiag;
}

TCiag* KCiagDanych(TCiag *CiagB){
    TCiag *BCiagB=CiagB;
    TCiag *Ciag=NULL;
    TCiag *BCiag=NULL;
    int i;
    unsigned char slowo;

    while (BCiagB!=NULL){
        slowo=0;
        for (i=0;i<8;i++){
            if (BCiagB->znak=='1') {
                slowo+=pow(2, i);
            }
            BCiagB=BCiagB->nast;
            if (BCiagB==NULL) break;
        }
        if (BCiag==NULL){
            if ((BCiag=malloc(sizeof(TCiag)))==NULL) return NULL;
            Ciag=BCiag;
            BCiag->nast=NULL;
            BCiag->znak=(char)slowo;
        }
        else {
            if ((BCiag->nast=malloc(sizeof(TCiag)))==NULL) return NULL;
            BCiag=BCiag->nast;
            BCiag->nast=NULL;
            BCiag->znak=(char)slowo;
        }
    }
    return Ciag;
}

int KZapiszPlk(TCiag *Ciag, char *nazwa){
    FILE *plk;
    TCiag *BCiag=Ciag;

    if ((plk=fopen(nazwa, "ab"))==NULL) return 1;
    fseek(plk, 0, 2);

    while (BCiag!=NULL){
        if ((fwrite(&(BCiag->znak), sizeof(BCiag->znak), 1, plk))==0) return 1;
        BCiag=BCiag->nast;
    }
    fclose(plk);
    return 0;
}

int Kompresuj (char *nazwaOdcz, char *NazwaZap){
    TDrzewo *DrzewoHuff=NULL;
    TDrzewo *Prefix=NULL;
    TCiag   *CiagB=NULL;
    TCiag   *Ciag=NULL;

    if ((DrzewoHuff=KPobierzElementy(nazwaOdcz))==NULL) return 1;
    if ((DrzewoHuff=KDrzewGen(DrzewoHuff))==NULL) return 1;
    if ((Prefix=KPref(DrzewoHuff))==NULL) return 1;

    if ((KZapiszSlownik(Prefix, NazwaZap))==1) return 1;
    if ((CiagB=KompresujPlikDoCiaguB(Prefix, nazwaOdcz))==NULL) return 1;
    if ((Ciag=KCiagDanych(CiagB))==NULL) return 1;
    free(CiagB);
    if ((KZapiszPlk(Ciag, NazwaZap))==1) return 1;

    free(DrzewoHuff);
    free(Ciag);

    return 0;
}

// --------------------------------  //
TLista* OdczElemZPlk(FILE *plk){
    TLista *Lista=NULL;
    TLista *Blista=NULL;
    TCiag  *BCiag=NULL;
    char znak;
    int licznik=0;
    char Dlugosc=0;

    while ((fread(&znak, sizeof(znak), 1, plk))==0){
        if (licznik==0){
            if (znak==0) break;
            if (Lista==NULL){
                if ((Lista=malloc(sizeof(TLista)))==NULL) return NULL;
                Lista->nast=NULL;
                Lista->skrot=NULL;
                Blista=Lista;
            }
            else {
                if ((Lista->nast=malloc(sizeof(TLista)))==NULL) return NULL;
                Lista=Lista->nast;
                Lista->nast=NULL;
                Lista->skrot=NULL;
            }

            licznik++;
            Dlugosc=znak;
        }
        else if (licznik==1){
            Lista->slowo=znak;
            licznik++;
        }
        else if (licznik==2){
            while (Dlugosc!=0){
                if (Lista->skrot==NULL){
                    if ((Lista->skrot=malloc(sizeof(TCiag)))==NULL) return NULL;
                    Lista->skrot->nast=NULL;
                    BCiag=Lista->skrot;
                }
                else {
                    if ((Lista->skrot->nast=malloc(sizeof(TCiag)))==NULL) return NULL;
                    Lista->skrot=Lista->skrot->nast;
                    Lista->skrot->nast=NULL;
                }
            Lista->skrot->znak=znak;
            Dlugosc--;
            }
            Lista->skrot=BCiag;
            licznik=0;
        }
        Lista=Blista;
    }
    return Lista;
}

int PorownajCiagi(TCiag *ciag1, TCiag *ciag2){
    TCiag *Bciag1=ciag1;
    TCiag *Bciag2=ciag2;
    int i=1;

    while (Bciag1!=NULL){
        i=1;
        if (Bciag1->znak==Bciag2->znak) {
            i=0;
            Bciag1=Bciag1->nast;
            Bciag2=Bciag2->nast;
        }
        else break;
    }
    return i;
}

TCiag* WczytajCiag(FILE *plk){
    char znak;
    TCiag *Ciag=NULL;
    TCiag *BCiag=NULL;

    while ((fread(&znak, sizeof(znak), 1, plk))!=0){
        if (Ciag==NULL){
            if ((Ciag=malloc(sizeof(TCiag)))==NULL) return NULL;
            Ciag->nast=NULL;
            BCiag=Ciag;
        }
        else {
            if ((Ciag->nast=malloc(sizeof(TCiag)))==NULL) return NULL;
            Ciag=Ciag->nast;
            Ciag->nast=NULL;
        }
        Ciag->znak=znak;
    }
    return BCiag;
}

TCiag* DCiagnaBit(TCiag *Ciag){
    TCiag *BCiag=Ciag;
    TCiag *Wynik;
    TCiag *BWynik;
    int i;

    while (BCiag!=NULL){
        for (i=7;i>-1;i--){
            if (Wynik==NULL){
                if ((Wynik=malloc(sizeof(TCiag)))==NULL) return NULL;
                BWynik=Wynik;
                Wynik->nast=NULL;
            }
            else {
                if ((Wynik->nast=malloc(sizeof(TCiag)))==NULL) return NULL;
                Wynik=Wynik->nast;
                Wynik->nast=NULL;
            }
            if (BCiag->znak>pow(2,i)){
                BCiag->znak-=pow(2,i);
                Wynik->znak='1';
            }
            else Wynik->znak='0';
        }
        BCiag=BCiag->nast;
    }
    return BWynik;
}

char DZnajdzZnak(TLista *Lista, TCiag *CiagB){
    TLista *BLista=Lista;
    TCiag *temp=NULL;
    TCiag *Btemp=NULL;

    while (CiagB!=NULL){
        if (temp==NULL){
                if ((temp=malloc(sizeof(TCiag)))==NULL) return (char)0;
                Btemp=temp;
                temp->nast=NULL;
            }
            else {
                if ((temp->nast=malloc(sizeof(TCiag)))==NULL) return (char)0;
                temp=temp->nast;
                temp->nast=NULL;
            }
        temp->znak=CiagB->znak;
        while (Lista!=NULL){
            if (PorownajCiagi(Btemp, Lista->skrot)==0) {
                    Lista=BLista;
                return Lista->slowo;
            }
        }
        Lista=BLista;
    }
    return (char)0;
}

int DZapiszZnak(char znak, FILE *plk){
    if ((fwrite(&znak, sizeof(znak),1,plk))==0) return 1;
    return 0;
}

int DZapDekom(char *nazwaZap, TCiag *Ciag, TLista *Lista){
    FILE *plk=NULL;
    TCiag *CiagB=NULL;
    char znak;

    if ((plk=fopen(nazwaZap, "wb"))==NULL) return 1;

    Ciag=WczytajCiag(plk);
    CiagB=DCiagnaBit(Ciag);
    while (CiagB!=NULL){
        znak=DZnajdzZnak(Lista, CiagB);
        if ((DZapiszZnak(znak, plk))==1) return 1;
    }
    free(CiagB);
    fclose(plk);
    return 0;
}

int Dekompresuj(char *nazwaOdcz, char *nazwaZap){
    FILE *plkO=NULL;
    TLista *Lista=NULL;
    TCiag *Ciag=NULL;

    if ((plkO=fopen(nazwaOdcz, "rb"))==NULL) return 1;
    Lista=OdczElemZPlk(plkO);
    Ciag=WczytajCiag(plkO);

    DZapDekom(nazwaZap, Ciag, Lista);

    free(Lista);
    free(Ciag);
    fclose(plkO);
    return 0;
}
