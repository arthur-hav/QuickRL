#include "main.h"
#include "globals.h"
#include "items.h"
#include "creatures.h"

SDL_Surface* screen = SDL_SetVideoMode(960, 600, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);


unsigned int dungeontime = 0;
int player_score = 0;
int player_floor = 1;
CPlayer* PC;

CGameEvent::CGameEvent(){};

CGameEvent::CGameEvent(unsigned int occur_time, CCreature * actor) : occur_time(occur_time), actor(actor)
{};

bool CGameEvent::operator < (const CGameEvent& event) const
{
    return occur_time > event.occur_time;
};


priority_queue<CGameEvent> event_queue;

CMap main_map(32, 22, FLOOR_CHAR);
CDisplay disp = CDisplay ();


bool los(int & x0, int & y0, int x1, int y1, float maxdist)
/*x0 and y0 are iterated until an obstructing wall is found or desination is reached*/
/* return true if there is a los between the 2 coordinates */
{
    int sx,sy, xnext, ynext, dx, dy;
    float dist;
    dx = x1-x0;
    dy = y1-y0;
    dist = sqrt(dx * dx + dy * dy);

    if (maxdist)
        if ( dist > maxdist )
            return false;

    if (x0 < x1)
        sx = 1;
    else
        sx = -1;
    if (y0 < y1)
        sy = 1;
    else
        sy = -1;
    xnext = x0;
    ynext = y0;


    while (xnext != x1 || ynext != y1)
    {
        if(abs(dy * (xnext - x0 + sx) - dx * (ynext - y0)) / dist < 0.5f)
            xnext += sx;

        else if(abs(dy * (xnext - x0) - dx * (ynext - y0 + sy)) / dist < 0.5f)
            ynext += sy;

        else
        {
            xnext += sx;
            ynext += sy;
        }

        if ( !main_map.is_in_map_bounds(xnext, ynext) ||
        main_map.get_grid_at(xnext, ynext) == WALL_CHAR ||
        main_map.get_grid_at(xnext, ynext) == DOOR_CHAR )
            break;
    }
    x0 = xnext;
    y0 = ynext;
    return x0 == x1 && y0 == y1;
};


CCreature * CPlayer::get_target()
{
    SDL_Event event;
    int i, j;
    disp.gameprintf("Target what ?");
    disp.draw();
    while (SDL_WaitEvent(&event))
    {
        switch ( event.type )
        {
            case SDL_MOUSEBUTTONDOWN:
            {

                if(event.button.x > HUD_W && event.button.y < 600 - CONS_H)
                {
                    i = event.button.x / 24 - 8;
                    j = event.button.y / 24;
                    return get_creature_at(i, j);
                }
                break;
            }

        }
    }
    return NULL;
}

unsigned int CPlayer::think()
{

    if(main_map.get_grid_at(x, y) == STAIRS_DOWN_CHAR)
    {
        main_map.generate();
        player_floor++;
    }


    disp.draw();


    SDL_Event event;
    int item_drag = -1;

    while (SDL_WaitEvent(&event))
    {
        switch ( event.type )
        {
            case SDL_KEYDOWN:
            {
                int dx = 0;
                int dy = 0;
                SDLKey mykey = event.key.keysym.sym;
                if (mykey == SDLK_LEFT || mykey == SDLK_KP4)
                    dx = -1;
                else if (mykey == SDLK_RIGHT || mykey == SDLK_KP6)
                    dx = 1;
                else if (mykey == SDLK_UP || mykey == SDLK_KP8)
                    dy = -1;
                else if (mykey == SDLK_DOWN || mykey == SDLK_KP2)
                    dy = 1;
                if(trymove(dx, dy))
                {
                    pick();
                    return get_move_time();
                }
                if(tryattack(dx, dy))
                    return get_attack_time();
                else if (mykey == SDLK_ESCAPE)
                {
                    printf("Exited cleanly\n");
                    exit(0);
                }
                else if(mykey == SDLK_PAGEUP)
                    disp.jump_relative(-4);
                else if(mykey == SDLK_PAGEDOWN)
                    disp.jump_relative(4);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                int item_nb = item_for_mousexy(event.button.x, event.button.y);
                if( item_nb != -1 && inventory[item_nb] )
                {
                    if ( event.button.button == SDL_BUTTON_LEFT )
                    {
                        SDLMod modifiers = SDL_GetModState();
                        if(modifiers & KMOD_CTRL)
                        {
                            //wip
                        }
                        item_drag = item_nb; //begin drag and drop
                        SDL_ShowCursor(SDL_DISABLE);
                    }
                    else if ( event.button.button == SDL_BUTTON_RIGHT )
                    {
                        return PC->item_use (item_nb);
                    }
                break;
                }
            }
            case SDL_MOUSEMOTION:
            {
                if(item_drag != -1)
                {

                    SDL_Rect rect;
                    rect.x = min(max(0,event.motion.x - 12),936);
                    rect.y = min(max(0,event.motion.y - 12),576);
                    rect.w = 24; rect.h = 24;
                    disp.pointer_fx (rect, inventory[item_drag]->get_model());
                }
                else
                    disp.look_update(event.button.x, event.button.y);
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if(item_drag != -1)
                {
                    int item_nb = item_for_mousexy(event.button.x, event.button.y);
                    if( item_nb != -1 )
                    {
                        CItem * save = inventory[item_nb]; //swap
                        inventory[item_nb] = inventory[item_drag];
                        inventory[item_drag] = save;
                    }
                    else if (event.button.x > HUD_W && event.button.y < 600 - CONS_H)
                        trydrop(item_drag);
                }
                SDL_ShowCursor(SDL_ENABLE);
                item_drag = -1;
                disp.draw();
                break;
            }
            case SDL_QUIT:
            {
                printf("Exited cleanly\n");
                exit(0);
            }
        }//switch
    }//while
    return 0;
}





int main ( int argc, char** argv )
{



    TTF_Init();
    console_font = TTF_OpenFont("VeraMoBd.ttf", 14);
    console_font_outlined = TTF_OpenFont("VeraMoBd.ttf", 14);
    count_font = TTF_OpenFont("VeraMoBd.ttf", 10);
    desc_font = TTF_OpenFont("VeraMoIt.ttf", 14);
    //TTF_SetFontOutline(console_font_outlined, 1);

    SDL_Init(SDL_INIT_EVERYTHING);
    init_models();

    PC = new CPlayer(0, 0);
    event_queue.push(CGameEvent( dungeontime, PC ));

    srand(time(NULL));
    main_map.generate();



    unsigned int act_time;

    CGameEvent event;

	while (1)
	{
	    event = event_queue.top();
        dungeontime = event.occur_time;
        act_time = event.actor->act();
        if (act_time > 0) {
            event.occur_time += act_time;
            event_queue.push(event);
        }
        event_queue.pop();
	}

};
