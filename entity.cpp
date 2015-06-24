#include "entity.h"
#include "globals.h"
#include <SDL/SDL.h>

CEntity::CEntity(int entity_number) :
entity_number(entity_number)
{}

string CEntity::get_name()
{
    return NAMES[entity_number];
}
CEntity::~CEntity()
{}

SDL_Surface *CEntity::get_model()
{
    return MODELS[entity_number];
}

int CEntity::get_entity_nb (){
    return entity_number;
}

void CEntity::print(int x, int y)
{
    SDL_Rect rect;
    rect.x = 24 * x + HUD_W;
    rect.y = 24 * y;
    SDL_BlitSurface(get_model(), 0, screen, &rect);
}
