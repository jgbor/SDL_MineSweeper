#ifndef VEZERLES_H_INCLUDED
#define VEZERLES_H_INCLUDED
#include <stdbool.h>

typedef enum all{ //állapot: az adott cellában mi van, megjelöltük-e, vagy már megnéztük-e
    bomba,
    ures,
    jeloltbomba,
    jeloltures,
    felfedett,
    robbant
}all;

typedef struct cella{
    all allapot;
    unsigned char ertek; //hány szomszédos cellán van bomba
}cella;

//Az aknamezőn lévő koordinátákhoz
typedef struct pozicio{
    int sor, oszlop;
}pozicio;

//Egy játék összes adata
typedef struct game{
    cella **palya;
    int magassag,szelesseg,aknadb,megnezve,ido;
    char nev[20+1];
}game;

typedef enum hibatipus{
    meret,aknaszam,mindketto
}hibatipus;

//Megnézi, hogy a szomszédos mezõkön hány akna van
unsigned char hanyakna(cella **palya, pozicio hely);

//Létrehozza a pályát és a rajta lévõ bombákat véletlenszerűen elhelyezi, ha sikeres a pálya létrehozása, igazzal tér vissza
bool palyainit(game *palya);

//Pályán van-e az adott mező
bool palyan(pozicio hely, int const magassag,int const szelesseg);

//Átállítja a cellát megtekintettre, ha kell a környékét is megnézi
void felfed (game *jatek, pozicio hely);

//Ez a fgv. segít a cellák megjelölésében, a jelölés visszavonásában
void megjelol(game *jatek, pozicio hely);

//Felszabadítja a lefoglalt memóriát
void felszabadit(game jatek);

//Megállapítja, hogy nyert-e a játékos
bool nyerte(game *jatek);

//Eldönti hogy a játék paraméterei alapján elindulhat-e, ha nem, akkor a hiba paraméterben egy hibakódot ad vissza
bool mehet(game *jatek, hibatipus *hiba);

#endif // VEZERLES_H_INCLUDED
