#include <stdbool.h>
#include <stdio.h>
#include "vezerles.h"
#include "megjelenit.h"
#include "fajlkezeles.h"
#include <string.h>


int main(int argc, char *argv[]){
    game jatek;
    bool menube=true;
    listaelem *ranglista=listabeolvas();
    while (menube){
        jatek.magassag=0;
        jatek.szelesseg=0;
        jatek.aknadb=0;
        strcpy(jatek.nev,"Jatekos");
        bool ujjatek=fomenu(&jatek,ranglista);
        menube=ujjatek;
        while (ujjatek){
            bool sikeres = palyainit(&jatek);
            if (sikeres){
                jatekablak(&jatek,&ujjatek,&menube,&ranglista);
            }
            else{
                printf("Hiba: tul keves a szabad memoria");
                ujjatek=false;
                menube=false;
            }
            felszabadit(jatek);
        }
    }
    listakiir(ranglista);
    listafelszabadit(ranglista);
    return 0;
}
