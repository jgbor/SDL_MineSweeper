#ifndef FAJLKEZELES_H_INCLUDED
#define FAJLKEZELES_H_INCLUDED

#include "vezerles.h"

typedef struct listaelem{
    char egyeb[20+3+3+5+3+1];//Az összehasonlítás szemponjából jelentéktelen infók(név + magasság + szélesség + aknák száma+ " db"), ezek a fájlban szóközzel vannak elválasztva
    int ido;
    struct listaelem *kov;
}listaelem;

//Beolvassa a fájlból a ranglistát
listaelem *listabeolvas();

//Felszabadítja a listát a memóriából
void listafelszabadit(listaelem *lista);

//Kiírja a fájlba a ranglistát
void listakiir(listaelem *lista);

//Az adott játék adatait beteszi az idő szerint rendezett listába
void listabarak(listaelem **eleje, game *jatek);

#endif // FAJLKEZELES_H_INCLUDED
