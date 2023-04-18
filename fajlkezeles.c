#include "fajlkezeles.h"
#include "debugmalloc.h"
#include <string.h>
#include <stdlib.h>

//Beolvassa a fájlból a ranglistát
listaelem *listabeolvas(){
    FILE* f;
    f=fopen("ranglista.txt","r");
    listaelem *lista=NULL;
    if (f==NULL){
        lista=NULL;
    }else{
        int ido;
        while (fscanf(f,"%d",&ido)==1){
            listaelem *ujelem= (listaelem*) malloc(sizeof(listaelem));
            ujelem->ido=ido;
            char szokoz;
            fscanf(f,"%c",&szokoz); //fgets ezt is beolvasta, így megnőtt a sor hossza minde kiírásnál
            fgets(ujelem->egyeb,38,f);
            ujelem->egyeb[strlen(ujelem->egyeb)-1]='\0';//kiírt egy plusz sor vége jelet, ezért ezt le kellett törölni
            ujelem->kov=NULL;
            if (lista==NULL){
                lista=ujelem;
            }else{
                listaelem *mozgo;
                for (mozgo=lista;mozgo->kov!=NULL;mozgo=mozgo->kov){
                }
                mozgo->kov=ujelem;
            }
        }
    }
    fclose(f);
    return lista;
}

//Felszabadítja a listát a memóriából
void listafelszabadit(listaelem *lista){
    listaelem *kovetkezo;
    while (lista!=NULL){
        kovetkezo=lista->kov;
        free(lista);
        lista=kovetkezo;
    }
}

//Kiírja a fájlba a ranglistából a top 5-öt (ha van annyi már)
void listakiir(listaelem *lista){
    FILE* f;
    f=fopen("ranglista.txt","w");
    if (f==NULL){
        printf("Nem lehet megnyitni a fajlt\n");
    }else{
        listaelem *mozgo=lista;
        int db=0;
        while (mozgo!=NULL && db<5){ //Mivel top 5-ös a ranglista, elég 5 elemet kiírni, ha az nagyobb lenne
            fprintf(f,"%d %s\n",mozgo->ido,mozgo->egyeb);
            mozgo=mozgo->kov;
            db++;
        }
    }
    fclose(f);
}

//Az adott játék adatait beteszi az idő szerint rendezett listába
void listabarak(listaelem **eleje, game *jatek){
    listaelem *ujelem= (listaelem*) malloc(sizeof(listaelem));
    ujelem->ido=jatek->ido;
    //A lista sztring adatának elkészítése
    char egyebbe[35];
    strcpy(egyebbe,jatek->nev);
    char tobbi[15];
    sprintf(tobbi," %dx%d %d db",jatek->magassag,jatek->szelesseg,jatek->aknadb);
    strcat(egyebbe,tobbi);
    strcpy(ujelem->egyeb,egyebbe);
    ujelem->kov=NULL;

    //Megfelelő helyre rakása az új elemnek
    if (*eleje==NULL){
        *eleje=ujelem;
    }else{
        listaelem *mozgo=*eleje;
        listaelem **elozo=eleje;
        while (mozgo!=NULL && ujelem->ido>mozgo->ido){
            elozo=&(mozgo->kov);
            mozgo=mozgo->kov;
        }
        *elozo=ujelem;
        ujelem->kov=mozgo;
    }
}
