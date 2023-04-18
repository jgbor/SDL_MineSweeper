#include "megjelenit.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include"debugmalloc.h"

Uint32 gombszin=34<<24 | 155<<16 | 155<<8 | 255;
Uint32 hatter=125<<24 | 125<<16 | 125<<8 | 255;
Uint32 feher=255<<24 | 255<<16 | 255<<8 | 255;
Uint32 piros=255<<24 | 0<<16 | 0<<8 | 255;

//Alap programú infoc sdl-ből átvéve
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer){
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL){
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL){
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

/* Infocről kivéve, adatok beolvasásához, kicsit átalakítva, hogy egérmozgatásra lépjen ki      https://infoc.eet.bme.hu/sdl/
 * Beolvas egy szoveget a billentyuzetrol.
 * A rajzolashoz hasznalt font es a megjelenito az utolso parameterek.
 * Az elso a tomb, ahova a beolvasott szoveg kerul.
 * A masodik a maximális hossz, ami beolvasható.
 * A visszateresi erteke logikai igaz, ha sikerult a beolvasas. */
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer) {
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;

    dest[0] = '\0';

    bool irasvege = false;
    bool kilep = false;

    SDL_StartTextInput();
    while (!kilep && !irasvege) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(true);
                }
                /* kivett rész, hogy ne enterrel lépjen ki a bevitelből
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = true;
                }*/
                break;

            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < hossz) {
                    strcat(dest, event.text.text);
                }

                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;

            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;

            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                kilep = true;
                break;

            //Általam beleírt rész
            case SDL_MOUSEMOTION:
                irasvege=true;
                break;
        }
    }

    /* igaz jelzi a helyes beolvasást; = ha egérmozgás miatt állt meg a ciklus */
    SDL_StopTextInput();
    return irasvege;
}

//Megkapja a másodperceket és egy pp:ss formájú sztringre alakítja
char *idoformatum (int ido){
    char *ora= (char*)malloc(6*sizeof(char));
    sprintf(ora,"%.2d:%.2d",ido/60,ido%60);
    return ora;
}

//Főmenü készítése
bool fomenu(game *jatek, listaelem *lista){
    SDL_Window *ablak;
    SDL_Renderer *renderer;
    sdl_init("Aknakereso - Menu",550,300,&ablak,&renderer);

    //2. ablak nyitása után a string nem jelent meg rendesen,3.-nál már egyáltalán nem, a megoldást itt találtam: https://discourse.libsdl.org/t/sdl2-gfx-not-working-with-multiple-windows/22713/3
    gfxPrimitivesSetFont(NULL, 0, 0);

    //Adatbeolvasáshoz szükséges dolgok
    SDL_Color FEHER = {255, 255, 255}, FEKETE = { 0, 0, 0 };
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("LiberationSerif-Regular.ttf", 32);
    if (font == NULL) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    //Háttér
    boxColor(renderer,0,0,550,300,hatter);

    //Feliratok
    stringColor(renderer,220,20,"A ket meret egy 3 es 60 kozotti szam,",feher);
    stringColor(renderer,220,40,"az aknak szama legalabb 3 es a cellak",feher);
    stringColor(renderer,220,60,"szamanal kisebb kell, hogy legyen.",feher);

    //Start gomb
    boxColor(renderer,400,200,500,250,gombszin);
    stringColor(renderer,430,222,"Start",feher);

    //Magasság bekéréséhez mező
    stringColor(renderer,20,10,"Magassag",feher);
    boxColor(renderer,20,20,100,60,feher);

    //Szélesség bekéréséhez mező
    stringColor(renderer,120,10,"Szelesseg",feher);
    boxColor(renderer,120,20,200,60,feher);

    //Aknák számának bekéréséhez mező
    stringColor(renderer,20,70,"Aknak szama",feher);
    boxColor(renderer,20,80,100,120,feher);

    //Játékos nevének bekéréséhez mező
    stringColor(renderer,120,70,"Jatekos neve",feher);
    boxColor(renderer,120,80,320,120,feher);

    //Ranglista
    stringColor(renderer,20,150,"Top 5",feher);
    if (lista==NULL){
        stringColor(renderer,50,200,"Nincs meg a listan eredmeny",feher);
    }else{
        int hely=170;
        listaelem *mozgo;
        for (mozgo=lista;mozgo!=NULL && hely<=250;mozgo=mozgo->kov){
            char *ido=idoformatum(mozgo->ido);
            stringColor(renderer,20,hely,ido,feher);
            free(ido);
            stringColor(renderer,70,hely,mozgo->egyeb,feher);
            hely+=20;
        }
    }

    SDL_RenderPresent(renderer);

    //A főmenü eseményei
    bool start=false;
    bool quit=false;
    SDL_Event event;
    while (!start && !quit){
        SDL_WaitEvent(&event);
        switch(event.type){
        case SDL_MOUSEBUTTONUP:
            if (event.button.button==SDL_BUTTON_LEFT){
                if ((event.button.x>=400 && event.button.x<=500)&&(event.button.y>=200 && event.button.y<=250)){
                    hibatipus hiba;
                    //start, ha minden adat jó, különben hibakód szerint figyelmeztetés
                    if (mehet(jatek,&hiba)){
                        start=true;
                    }else{
                        switch (hiba){
                        case meret: //rossz méret
                            boxColor(renderer,350,290,500,300,hatter);
                            stringColor(renderer,350,270,"Hibas meret!!!",piros);
                            break;
                        case aknaszam: //rossz aknaszám
                            boxColor(renderer,350,270,500,290,hatter);
                            stringColor(renderer,350,290,"Hibas aknaszam!!!",piros);
                            break;
                        case mindketto: //rossz mindkettő
                            stringColor(renderer,350,290,"Hibas aknaszam!!!",piros);
                            stringColor(renderer,350,270,"Hibas meret!!!",piros);
                            break;
                        }
                        SDL_RenderPresent(renderer);
                    }
                //Különböző adatok beolvasása input_texttel
                //magasság
                }else if((event.button.x>=20 && event.button.x<=100)&&(event.button.y>=20 && event.button.y<=60)){
                    SDL_Rect mag={20,20,80,40};
                    char magas[3]="";
                    input_text(magas,3,mag,FEHER,FEKETE,font,renderer);
                    jatek->magassag= atoi(magas);
                //szélesség
                }else if((event.button.x>=120 && event.button.x<=200)&&(event.button.y>=20 && event.button.y<=60)){
                    SDL_Rect szel={120,20,80,40};
                    char szeles[3]="";
                    input_text(szeles,3,szel,FEHER,FEKETE,font,renderer);
                    jatek->szelesseg= atoi(szeles);
                //aknaszám
                }else if((event.button.x>=20 && event.button.x<=100)&&(event.button.y>=80 && event.button.y<=120)){
                    SDL_Rect akn={20,80,80,40};
                    char akna[5]="";
                    input_text(akna,5,akn,FEHER,FEKETE,font,renderer);
                    jatek->aknadb= atoi(akna);
                //név
                }else if((event.button.x>=120 && event.button.x<=320)&&(event.button.y>=80 && event.button.y<=120)){
                    SDL_Rect nev={120,80,200,40};
                    input_text(jatek->nev,21,nev,FEHER,FEKETE,font,renderer);
                }

            }
            break;
        case SDL_QUIT:
            quit=true;
            break;
        }
    }

    SDL_Quit();
    return start; //ha játszani akarunk, igazzal tér vissza, különben hamissal
}

//A összes cella képének meghatározása
void cellatrajzol(game *jatek, SDL_Renderer *renderer, int cellameret){
    int kezdo_x=50+(600-cellameret*jatek->szelesseg)/2; //pálya bal felső sarka
    int kezdo_y=150;
    for (int i=1; i<=jatek->magassag; i++){
        for (int j=1; j<=jatek->szelesseg; j++){
            SDL_Texture *kep;
            char kepfajl[6];
            SDL_Rect celhely={(cellameret)*(j-1)+kezdo_x,(cellameret)*(i-1)+kezdo_y,cellameret,cellameret};
            //Különböző mezők képeinek betöltése
            switch (jatek->palya[i][j].allapot){
            case ures:
                kep=IMG_LoadTexture(renderer,"unexplored.png");
                break;
            case bomba:
                kep=IMG_LoadTexture(renderer,"unexplored.png");
                break;
            case jeloltures:
                kep=IMG_LoadTexture(renderer,"flag.png");
                break;
            case jeloltbomba:
                kep=IMG_LoadTexture(renderer,"flag.png");
                break;
            case felfedett:
                //Adott
                sprintf(kepfajl,"%d.png",jatek->palya[i][j].ertek);
                kep=IMG_LoadTexture(renderer,kepfajl);
                break;
            case robbant:
                kep=IMG_LoadTexture(renderer,"mine.png");
                break;
            }
            if (kep==NULL) {
                SDL_Log("Nem nyithato meg a kep: %s", IMG_GetError());
                exit(1);
            }
            SDL_RenderCopy(renderer,kep,NULL,&celhely);
            SDL_DestroyTexture(kep);
        }
    }
    SDL_RenderPresent(renderer);
}

//Kattintás esetén melyik cellára nyomtunk
pozicio melyikcella(SDL_Event *event,game *jatek, int cellameret){
    pozicio hely;
    int kezdo_x=50+(600-cellameret*jatek->szelesseg)/2;
    int kezdo_y=150;
    hely.sor=(event->button.y-kezdo_y)>=0 ? (event->button.y-kezdo_y)/cellameret+1 : -1; //A -1. cellát is 0.-nak vette
    hely.oszlop=(event->button.x-kezdo_x)>=0 ? (event->button.x-kezdo_x)/cellameret+1 : -1;
    return hely;
}

//Időzítőhöz szükséges fgv
Uint32 idozites(Uint32 ms, void *parameter){
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
    return ms;
}

//A játékablak elkészítése
void jatekablak(game *jatek, bool *ujjatek, bool *menube, listaelem **lista){
    SDL_Window *ablak;
    SDL_Renderer *renderer;
    int cellameret= jatek->magassag>jatek->szelesseg ? 600/jatek->magassag : 600/jatek->szelesseg;
    int ablakmagassag=200+cellameret*jatek->magassag;
    sdl_init("Aknakereso",700,ablakmagassag,&ablak,&renderer);
    SDL_Event event;
    *ujjatek=false;
    *menube=false;
    bool quit=false;
    //Hangok
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
    Mix_Chunk *robbanas=Mix_LoadWAV("explosion.flac"); //https://opengameart.org/content/big-explosion
    Mix_Chunk *gyozelem=Mix_LoadWAV("win.wav"); //https://opengameart.org/content/win-sound-effect
    Mix_Chunk *idolejar=Mix_LoadWAV("clock.wav"); //https://opengameart.org/content/ticking-clock-0
    /*Mix_Music *zene=Mix_LoadMUS("music.wav"); //https://opengameart.org/content/loading-screen-loop
    Mix_PlayMusic(zene,-1); //Túl nagy méretű, ezért kiveszem*/

    //2. ablak nyitása után a string nem jelent meg rendesen,3.-nál már egyáltalán nem, a megoldást itt találtam: https://discourse.libsdl.org/t/sdl2-gfx-not-working-with-multiple-windows/22713/3
    gfxPrimitivesSetFont(NULL,0,0);

    //Háttér
    boxColor(renderer,0,0,700,ablakmagassag,hatter);

    //Új játék gomb
    boxColor(renderer,50,20,150,70,gombszin);
    stringColor(renderer,70,42,"Uj jatek",feher);

    //Főmenü gomb
    boxColor(renderer,550,20,650,70,gombszin);
    stringColor(renderer,580,42,"Fomenu",feher);

    //Cellák kirajzolása
    cellatrajzol(jatek,renderer,cellameret);

    //Idő kezdetben
    SDL_Texture *ora=IMG_LoadTexture(renderer,"clock.png");
    SDL_Rect celhely= {325,20,30,30};
    SDL_RenderCopy(renderer,ora,NULL,&celhely);
    char *ido=idoformatum(jatek->ido);
    stringColor(renderer,320,55,ido,feher);
    free(ido);

    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(ora);

    bool vege=false; //letiltja a játékfunkciókat, ha nyer, vagy veszít a játékos
    bool kirajzolva=false; //azért hogy az eredményt csak egyszer rajzolja ki(ha cikluson kívülre raknám, csak egy pillanatra látszódna
    clock_t kezd=clock();
    clock_t eltelt;
    SDL_TimerID timer=SDL_AddTimer(1000,idozites,NULL);
    //a játékablak eseményei
    while (!(*ujjatek) && !quit && !(*menube)){
        SDL_WaitEvent(&event);
        switch(event.type){
        case SDL_USEREVENT:
            //Óra kirajzolása másodpercenként
            eltelt= clock()-kezd;
            jatek->ido= eltelt/CLOCKS_PER_SEC;
            char *ido=idoformatum(jatek->ido);
            boxColor(renderer,320,55,370,100,hatter);
            stringColor(renderer,320,55,ido,feher);
            free(ido);
            SDL_RenderPresent(renderer);
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button==SDL_BUTTON_LEFT){
                if ((event.button.x>=50 && event.button.x<=150)&&(event.button.y>=20 && event.button.y<=70)){ //Új játék
                    *ujjatek=true;
                }else if ((event.button.x>=550 && event.button.x<=650)&&(event.button.y>=20 && event.button.y<=70)){ //Főmenü
                    *menube=true;
                }else{
                    //Felfedés
                    pozicio hely=melyikcella(&event,jatek,cellameret);
                    if (palyan(hely,jatek->magassag,jatek->szelesseg) && !vege){
                        felfed(jatek,hely);
                        cellatrajzol(jatek,renderer,cellameret);
                    }
                }
            }else if(event.button.button==SDL_BUTTON_RIGHT){
                //Megjelölés
                 pozicio hely=melyikcella(&event,jatek,cellameret);
                 if (palyan(hely,jatek->magassag,jatek->szelesseg) && !vege){
                    megjelol(jatek,hely);
                    cellatrajzol(jatek,renderer,cellameret);
                }
            }

            break;
        case SDL_QUIT:
            quit=true;
            break;
        }
        vege= nyerte(jatek) || (jatek->megnezve==-1) || (jatek->ido>3600);
        if (vege && !kirajzolva){
            SDL_RemoveTimer(timer);
            //Mix_PauseMusic();
            SDL_Texture *eredmeny;
            SDL_Rect celhely={25,ablakmagassag/2-50,650,150};
            boxColor(renderer,0,150,700,ablakmagassag,hatter-120);
            if (nyerte(jatek)){
                eredmeny=IMG_LoadTexture(renderer,"nyertel.png");
                Mix_PlayChannel(-1,gyozelem,0);
            }else{
                eredmeny=IMG_LoadTexture(renderer,"vesztettel.png");
                if (jatek->ido>3600){
                    char *ido=idoformatum(jatek->ido);
                    boxColor(renderer,320,55,370,100,hatter);
                    stringColor(renderer,320,55,ido,piros);
                    stringColor(renderer,290,65,"Lejart az idod :(",piros);
                    free(ido);
                    Mix_PlayChannel(-1,idolejar,0);
                }else{
                    Mix_PlayChannel(-1,robbanas,0);
                }
            }
            if (eredmeny==NULL) {
                SDL_Log("Nem nyithato meg a kep: %s", IMG_GetError());
                exit(1);
            }
            SDL_RenderCopy(renderer,eredmeny,NULL,&celhely);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(eredmeny);
            kirajzolva=true;
        }
    }
    //Ha nyert akkor berakja a listába
    if (nyerte(jatek)){
        listabarak(lista,jatek);
    }
    //Mix_FreeMusic(zene);
    Mix_FreeChunk(robbanas);
    Mix_FreeChunk(gyozelem);
    Mix_FreeChunk(idolejar);
    Mix_CloseAudio();
    SDL_Quit();
}
