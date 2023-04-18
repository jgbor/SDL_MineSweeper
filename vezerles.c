#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "vezerles.h"
#include "debugmalloc.h"

//Megállapítja hány akna van a szomszédos mezőkön, visszatér ezzel a számmal
unsigned char hanyakna(cella **palya, pozicio hely){
    unsigned char db=0;
    for (int i=-1; i<=1; i++){
        for (int j=-1; j<=1; j++){
            if (palya[hely.sor+i][hely.oszlop+j].allapot==bomba)
                db++;
        }
    }
    //mivel az elõbb azt a mezõt is számoltuk amelyiken vagyunk, ha az bomba, annak az értékét ki kell vonni
    if (palya[hely.sor][hely.oszlop].allapot==bomba)
        db--;
    return db;
}

//Inicializálja a pályát
bool palyainit(game *jatek){
    srand(time(NULL));
    jatek->palya = (cella**) malloc ((jatek->magassag+2)*sizeof(cella*)); //+2 a keret miatt
    if (jatek->palya==NULL)
         return false;
    for (int i=0; i<jatek->magassag+2; i++){
        jatek->palya[i] = (cella*) malloc((jatek->szelesseg+2)*sizeof(cella)); //+2 itt is a keret miatt
        if (jatek->palya[i]==NULL)
            return false;
    }
    jatek->megnezve=0;
    jatek->ido=0;
    //minden cella alaphelyzetbe állítása
    for (int i=0; i<=jatek->magassag+1; i++){
        for (int j=0; j<=jatek->szelesseg+1; j++){
            jatek->palya[i][j].allapot=ures;
        }
    }
    for (int i=1; i<=jatek->aknadb; i++){
        int sor=rand()%jatek->magassag+1;
        int oszlop=rand()%jatek->szelesseg+1; //+1 miatt 1-től a max méretig lehet a bomba, a keretbe nem fog kerülni
        while (jatek->palya[sor][oszlop].allapot==bomba){ //Amíg olyan helyet sorsol, ahol van bomba addig újrarandomizálja a helyet
                sor=rand()%jatek->magassag+1;
                oszlop=rand()%jatek->szelesseg+1;
        }
        jatek->palya[sor][oszlop].allapot=bomba;
    }
    //a cellák körüli aknák számának meghatározása
    for (int i=1; i<=jatek->magassag; i++){
        for (int j=1; j<=jatek->szelesseg; j++){
            if (jatek->palya[i][j].allapot!=bomba){    //ha bomba van ott, fölösleges megszámolni
                pozicio hely={i,j};
                jatek->palya[i][j].ertek=hanyakna(jatek->palya,hely);
            }
        }
    }
    return true;
}

//Pályán van-e az adott mező
bool palyan(pozicio hely, int const magassag,int const szelesseg){
    return (hely.sor>0 && hely.sor<=magassag) && (hely.oszlop>0 && hely.oszlop<=szelesseg);
}

//Átállítja a cellát megtekintettre, ha kell a környékét is megnézi
void felfed (game *jatek, pozicio hely){
    bool mostvaltott=false;
    if (jatek->palya[hely.sor][hely.oszlop].allapot!=jeloltures && jatek->palya[hely.sor][hely.oszlop].allapot!=jeloltbomba){
        if (jatek->palya[hely.sor][hely.oszlop].allapot==ures){
            jatek->palya[hely.sor][hely.oszlop].allapot=felfedett;
            jatek->megnezve++;//győzelem megállapításához növeli a megnézett mezőket
            mostvaltott=true;
        }else if(jatek->palya[hely.sor][hely.oszlop].allapot==bomba){
            jatek->palya[hely.sor][hely.oszlop].allapot=robbant;
            jatek->megnezve=-1; //Ha robban, akkor ez jelzi, hogy vesztett a játékos
        }
    }
    //ha nulla akna van a cella körül, akkor felfedi a környéket, amíg el nem éri az összes olyat, aminek nem 0 az értéke
    if (jatek->palya[hely.sor][hely.oszlop].ertek==0 && mostvaltott){
        for (int i=-1; i<=1; i++){
            for (int j=-1; j<=1; j++){
                if (i!=0 || j!=0) {
                    pozicio kovihely = {hely.sor + i, hely.oszlop + j};
                    //Ha a pályán van a következõ hely és az még nem felfedett, akkor meghívjuk újra rá ezt a fgv.-t
                    if (palyan(kovihely,jatek->magassag,jatek->szelesseg) && (jatek->palya[kovihely.sor][kovihely.oszlop].ertek!=felfedett)){
                        felfed(jatek,kovihely);
                    }
                }
            }
        }
    }else
        return;
}

//Ez a fgv. segít a cellák megjelölésében, a jelölés visszavonásában
void megjelol(game *jatek, pozicio hely){
    switch (jatek->palya[hely.sor][hely.oszlop].allapot){
        case ures: jatek->palya[hely.sor][hely.oszlop].allapot=jeloltures;
                   break;
        case bomba: jatek->palya[hely.sor][hely.oszlop].allapot=jeloltbomba;
                   break;
        case jeloltures: jatek->palya[hely.sor][hely.oszlop].allapot=ures;
                   break;
        case jeloltbomba: jatek->palya[hely.sor][hely.oszlop].allapot=bomba;
                   break;
    }
}

//Felszabadítja a lefoglalt memóriát
void felszabadit(game jatek){
    for (int i=0;i<jatek.magassag+2;i++){
        free(jatek.palya[i]);
    }
    free(jatek.palya);
}

//Megállapítja, hogy nyert-e a játékos
bool nyerte(game *jatek){
    return jatek->megnezve==(jatek->magassag*jatek->szelesseg)-jatek->aknadb;
}

//Megállapítja, hogy mehet-e a játék
bool mehet(game *jatek, hibatipus *hiba){
    //Lehetséges méretekhez konstansok
    int const MINMERET=3;
    int const MAXMERET=60;
    int const MINBOMBA=3;
    bool jo_e=(jatek->aknadb>=MINBOMBA && jatek->aknadb<(jatek->magassag*jatek->szelesseg)) && (jatek->magassag>=MINMERET && jatek->magassag<=MAXMERET)
        && ((jatek->szelesseg>=MINMERET && jatek->szelesseg<=MAXMERET));
    if (!jo_e){
        if (!((jatek->magassag>=MINMERET && jatek->magassag<=MAXMERET) && ((jatek->szelesseg>=MINMERET && jatek->szelesseg<=MAXMERET))))
            if (!(jatek->aknadb>=MINBOMBA && jatek->aknadb<(jatek->magassag*jatek->szelesseg)))
                *hiba=mindketto; //mindkettő hibás
            else
                *hiba=meret; //csak méret rossz
        else
            *hiba=aknaszam; //csak aknaszám rossz
    }
    return jo_e;
}
