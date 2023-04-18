#ifndef MEGJELENIT_H_INCLUDED
#define MEGJELENIT_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "vezerles.h"
#include <stdlib.h>
#include "fajlkezeles.h"

//Ablak létrehozása, Infoc SDL alap programból
void sdl_init(char const *felirat,int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer);

//Infoc-ről átvéve, adatok beolvasásához, kicsit átalakítva a kilépési feltételnél  https://infoc.eet.bme.hu/sdl/
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);

//Megkapja a másodperceket és egy pp:ss formájú sztringre alakítja
char *idoformatum (int ido);

//Fõmenüt jeleníti meg, ahol a felhasználó megadhatja a pálya méreteit, az aknák számát és a nevét
bool fomenu(game *jatek, listaelem *ranglista);

//Az összes cella kirajzolával foglalkozó függvény
void cellatrajzol(game *jatek, SDL_Renderer *renderer, int cellameret);

//Megállapítja, hogy kattintás esetén melyik cellára nyomtunk
pozicio melyikcella(SDL_Event *event,game *jatek, int cellameret);

//Időzítőhöz szükséges fgv
Uint32 idozites(Uint32 ms, void *parameter);

//Az ablak, ahol maga a játék folyik
void jatekablak(game *jatek, bool *ujjatek, bool *menube, listaelem **ranglista);

#endif // MEGJELENIT_H_INCLUDED
