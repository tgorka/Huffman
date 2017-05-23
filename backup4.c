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


int main()
{
    TCiag *Ciag;
    TDrzewo *Drzewo=KPobierzElementy("1.txt");
    TDrzewo *Prefix, *kkk;
    Drzewo=KDrzewGen(Drzewo);
    Prefix=KPref(Drzewo);

    Kompresuj("1.txt", "2.txt");

    kkk=KZnajdz(Prefix, 'a');
    Ciag=KZnajdzSkrot(kkk);

    KZapiszSlownik(Prefix, "2.txt");

    printf(" - %d\n", KDlogoscC(Ciag));
    while (Ciag!=NULL){
        printf("%c", Ciag->znak);
        Ciag=Ciag->nast;
    }

    printf("\n");

    printf("%d\n", kkk->waga);

    while(Prefix->prefix!=NULL) {
        printf("%c - %d\n", Prefix->slowo, Prefix->waga);
        Prefix=Prefix->prefix;
    }


    free(Ciag);
    free(Drzewo);
    return 0;
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
