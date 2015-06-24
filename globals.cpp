#include "globals.h"


string NAMES[MAX_NAMED] =
{
    "koko",
    "gobelin",
    "shuriken",
    "javelin",
    "health potion",
    "emerald",
    "saphir",
    "rubis",
    "knife",
    "steal breastplate"
};



struct creature_constants CREATURES[MAX_CREATURES] =
{
    {
        "Koko",
        PLAYER,
        4.0, //Attack damage
        50, //Attack time
        0, //Armor penetration
        0.0, //Base armor
        100, //Move time
        100.0, //Max HP
        6.25 //fov radius
    },
    {
        "Gobelin",
        GOBELIN,
        5.0, //Attack damage
        100, //Attack time
        50, //Armor penetration
        0.0, //Base armor
        100, //Move time
        25.0, //Max HP
        5.25 //fov radius
    }
};

SDL_Surface * MODELS[MAX_TILESET];

void init_models()
{
    SDL_Surface * tileset = IMG_Load ("tileset.png");
    Uint32 rmask, gmask, bmask, amask;
    int i;
    SDL_Surface * surf;
    SDL_Rect rect;
    rect.w = 24;
    rect.h = 24;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    SDL_SetAlpha(tileset, 0, SDL_ALPHA_OPAQUE);
    for(i = 0; i < MAX_TILESET; i++)
    {

        rect.x = (i % 16) * 24;
        rect.y = (i / 16) * 24;
        surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 32, rmask, gmask, bmask, amask);


        SDL_BlitSurface(tileset, &rect, surf, NULL);
        MODELS[i] = surf;
    }
    SDL_FreeSurface(tileset);
}
