#include "main.h"
#include "globals.h"
#include "map.h"

#define UNSEEN 0
#define SEEN 1
#define SEEING 2
#define WAS_SEEING 3

SDL_Surface* model_shadow = IMG_Load ("shadow.png");

SDL_Surface* model_invent = IMG_Load ("invent.png");
SDL_Surface* model_equiped = IMG_Load ("equiped.png");
SDL_Surface* model_cooling = IMG_Load ("cooling.png");

SDL_Surface* icon_heart = IMG_Load ("heart-icon.png");
SDL_Surface* icon_sword = IMG_Load ("sword-icon.png");
SDL_Surface* icon_shield = IMG_Load ("shield-icon.png");
SDL_Surface* icon_ap = IMG_Load ("ap-icon.png");
SDL_Surface* icon_dollar = IMG_Load ("dollar-icon.png");
SDL_Surface* icon_stairs = IMG_Load ("stairs-icon.png");


TTF_Font * console_font;
TTF_Font * console_font_outlined;
TTF_Font * count_font;
TTF_Font * desc_font;

void CDisplay::pointer_fx (SDL_Rect rect, SDL_Surface *sprite){
    static int old_x = -1;
    static int old_y = -1;
    Uint32 rmask, gmask, bmask, amask;
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
    SDL_Surface *save_surface = SDL_CreateRGBSurface 
        (SDL_SWSURFACE, 24, 24, 32, rmask, gmask, bmask, amask);


    if (old_x > 0 && old_y > 0)
        SDL_UpdateRect(screen, old_x, old_y, rect.w, rect.h);

    SDL_BlitSurface(screen, &rect, save_surface, 0);
    SDL_BlitSurface(sprite, 0, screen, &rect);
    SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
    old_x = rect.x;
    old_y = rect.y;
    SDL_BlitSurface(save_surface, 0, screen, &rect);
    SDL_FreeSurface(save_surface);
}

void CDisplay::print_tile_at(int x, int y)
{
    if(vismap->get_grid_at(x,y) == UNSEEN)
        return;

    SDL_Rect rect;
    rect.x = 24 * x + 192;
    rect.y = 24 * y;
    SDL_Surface * tile_model = NULL;
    switch (main_map.get_grid_at(x,y))
    {
        case WALL_CHAR:
        {
            int i = 0;
            if( main_map.is_in_map_bounds(x-1, y) &&
                vismap->get_grid_at(x-1,y) != UNSEEN &&
                main_map.get_grid_at(x-1, y) == WALL_CHAR)
                i += 1;
            if( main_map.is_in_map_bounds(x+1, y) &&
                vismap->get_grid_at(x+1,y) != UNSEEN &&
                main_map.get_grid_at(x+1, y) == WALL_CHAR)
                i += 2;
            if( main_map.is_in_map_bounds(x, y+1) &&
                vismap->get_grid_at(x,y+1) != UNSEEN &&
                main_map.get_grid_at(x, y+1) == WALL_CHAR)
                i += 4;
            if( main_map.is_in_map_bounds(x, y-1) &&
                vismap->get_grid_at(x,y-1) != UNSEEN &&
                main_map.get_grid_at(x, y-1) == WALL_CHAR)
                i += 8;
            tile_model = MODELS[WALL0 + i];
            break;
        }
        case DOOR_CHAR:
        {
            tile_model = MODELS[DOOR];
            break;
        }
        case STAIRS_DOWN_CHAR:
        {
            tile_model = MODELS[STAIRS_DOWN];
            break;
        }
        default:
            tile_model = MODELS[FLOOR];
    }
    SDL_BlitSurface(tile_model, 0, screen, &rect);
    if(vismap->get_grid_at(x,y) == SEEN)
    {
        SDL_BlitSurface(model_shadow, 0, screen, &rect);
        return;
    }

    int i;
    for(i = 0; i < get_item_count_at(x, y); i++ )
        get_item_at(x, y, i)->print(x,y);

    CCreature * creat = get_creature_at(x, y);
    if (creat)
        creat->print(x, y);

};
void CDisplay::refresh_at(int x, int y)
{
    int resx = PC->x;
    int resy = PC->y;

    if(!vismap->is_in_map_bounds(x,y))
        return;
        
    bool lit = los(resx, resy, x,y, PC->get_fov_rad());
    if(lit)
    {
        vismap->set_grid_at(x,y, SEEING); //direct line A to B exist
        return;
    }
    if ((resx != PC->x || resy != PC->y) && vismap->get_grid_at(resx,resy) == UNSEEN)
        vismap->set_grid_at(resx,resy, SEEN); //obstructing wall that is unlit

    if(!vismap->is_in_map_bounds(x,y))
        return;

};

static void render_outlined_text(string str, SDL_Rect * rect)
{
    SDL_Surface * rendered_text;

    SDL_Color fg_color = {0xff, 0xff, 0xff};
    SDL_Color outline_color = {0x77, 0x33, 0x33};

    rendered_text = TTF_RenderText_Blended(console_font_outlined, str.c_str(), outline_color);
    SDL_BlitSurface(rendered_text, 0, screen, rect);
    SDL_FreeSurface(rendered_text);
    rendered_text = TTF_RenderText_Blended(console_font, str.c_str(), fg_color);
    SDL_BlitSurface(rendered_text, 0, screen, rect);
    SDL_FreeSurface(rendered_text);
}


static void render_desc_text(string str, SDL_Rect * rect)
{
    SDL_Surface * rendered_text;
    char copy[64];
    strcpy(copy, str.c_str());
    copy[0] = toupper(str[0]);

    SDL_Color fg_color = {0xbb, 0xbb, 0xbb};

    rendered_text = TTF_RenderText_Blended(desc_font, copy, fg_color);
    SDL_BlitSurface(rendered_text, 0, screen, rect);
    SDL_FreeSurface(rendered_text);
}



int item_for_mousexy(int mouse_x, int mouse_y)
{
    if(mouse_x >= BORDER && mouse_x < HUD_W - BORDER &&
        mouse_y >= INVENT_Y && mouse_y < INVENT_Y + 26 * INVENT_ROWS )
        return (mouse_x - BORDER) / 26 + ((mouse_y - INVENT_Y) / 26 ) * INVENT_COLUMNS;
    return -1;
}

void CDisplay::look_update(int mouse_x, int mouse_y)
{
    int item = item_for_mousexy(mouse_x, mouse_y);
    SDL_Rect rect;
    rect.x = BORDER;
    rect.y = DESC_Y;
    rect.w = HUD_W - 2 * BORDER;
    rect.h = 17;
    SDL_FillRect(screen, &rect, 0x00000000);
    if ( item != -1 && PC->inventory[item] )
        render_desc_text(PC->inventory[item]->get_name(), &rect);
    else {
        mouse_x -= HUD_W;
        mouse_x /= 24;
        mouse_y /= 24;
        if (!vismap->is_in_map_bounds(mouse_x, mouse_y)) 
            return;

        SDL_Rect mouse_rect;
        mouse_rect.x = mouse_x * 24 + HUD_W;
        mouse_rect.y = mouse_y * 24;
        mouse_rect.w = 24; 
        mouse_rect.h = 24;
        pointer_fx (mouse_rect, MODELS[MOUSE_OUTLINE]);

        CCreature * creat = get_creature_at(mouse_x, mouse_y);
        CItem * item = get_item_at(mouse_x, mouse_y, 0);
        if (vismap->get_grid_at (mouse_x, mouse_y) == SEEING){
            if (creat)
                render_desc_text(creat->get_name(), &rect);
            else if (item)
                render_desc_text(item->get_name(), &rect);
        }
    }
    SDL_UpdateRect(screen, rect.x, rect.y, HUD_W - 2 * BORDER, rect.h);

}

void CDisplay::draw()
{
    SDL_FillRect(screen, NULL, 0x00000000);

    /* CONSOLE */

    char buf[24];
    int i,j;
    SDL_Rect rect;
    rect.x = HUD_W;
    rect.y = 600 - CONS_H;
    rect.h = CONS_H;
    rect.w = 960 - HUD_W;
    SDL_Surface * rendered_text;
    SDL_Color fg_color = {0xff, 0xff, 0xff};
    for(i = 0; i < 4 ; i++) //at most 4 messages
    {
        if (i + offset > last_message_nb)
            continue;
        if ( i + offset != 0 )
            fg_color = {0xa0, 0xa0, 0xa0}; //darken previous msg
        rendered_text = TTF_RenderText_Blended(console_font, message_buf[(last_message_nb - i + offset)%20], fg_color);
        rect.y = 584 - 17 * i;
        SDL_BlitSurface(rendered_text, 0, screen, &rect);
        SDL_FreeSurface(rendered_text);
    }

    /* HUD */

    rect.x = BORDER;
    rect.y = BORDER;
    rect.h = 600;
    rect.w = HUD_W - 2 * BORDER;

    SDL_FillRect(screen, &rect, 0x00000000);

    SDL_BlitSurface(icon_heart, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %.0f / %.0f", PC->HP, PC->get_maxHP());
    render_outlined_text(buf, &rect);
    rect.x -=16;

    rect.y += 17;
    SDL_BlitSurface(icon_sword, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %.0f / %d", PC->get_attack_dmg(), PC->get_attack_time());
    render_outlined_text(buf, &rect);
    rect.x -=16;

    rect.y += 17;
    SDL_BlitSurface(icon_shield, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %.0f", PC->get_armor());
    render_outlined_text(buf, &rect);
    rect.x -=16;

    rect.y += 17;
    SDL_BlitSurface(icon_ap, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %d", PC->get_AP());
    render_outlined_text(buf, &rect);
    rect.x -=16;

    rect.y += 17;
    SDL_BlitSurface(icon_dollar, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %d", player_score);
    render_outlined_text(buf, &rect);
    rect.x -=16;

    rect.y += 17;
    SDL_BlitSurface(icon_stairs, 0, screen, &rect);
    rect.x +=16;
    sprintf(buf, " %d", player_floor);
    render_outlined_text(buf, &rect);
    rect.x -=16;


    /* INVENTORY */
    i = 0;
    CItem * item;
    for(rect.y = INVENT_Y; rect.y < INVENT_Y + 26 * INVENT_ROWS; rect.y +=26 )
         for(rect.x = BORDER; rect.x < BORDER + 26 * INVENT_COLUMNS; rect.x +=26 )
        {
            SDL_BlitSurface(model_invent, 0, screen, &rect);
            item = PC->inventory[i];
            if (item)
            {

                rect.x++; rect.y++;

                if ( PC->armor == item || PC->weapon == item)
                    SDL_BlitSurface(model_equiped, 0, screen, &rect);

                SDL_BlitSurface(item->get_model(), 0, screen, &rect);
                if (item->count > 1)
                {
                    sprintf(buf, "%d", item->count);
                    fg_color = {0xff, 0xff, 0xff};
                    rendered_text = TTF_RenderText_Blended(count_font, buf, fg_color);
                    rect.y +=14;
                    SDL_BlitSurface(rendered_text, 0, screen, &rect);
                    rect.y -=14;
                    SDL_FreeSurface(rendered_text);
                }
                /*
                if (item->is_cooling())
                    SDL_BlitSurface(model_cooling, 0, screen, &rect); */
                rect.x--; rect.y-- ;
            }
            i++;
        }

    /* DUNGEON SCREEN */

    for(i = 0 ; i < vismap->get_width() ; i++)
        for(j = 0 ; j< vismap->get_height(); j++)
            if(vismap->get_grid_at(i,j) == SEEING)
                vismap->set_grid_at(i,j, SEEN);

    for(i = PC->x - (int)PC->get_fov_rad(); i < PC->x + (int)PC->get_fov_rad() + 1; i++)
        for(j = PC->y - (int)PC->get_fov_rad() ; j < PC->y + (int)PC->get_fov_rad() + 1; j++)
            refresh_at(i,j);

    for(i = 0 ; i < vismap->get_width() ; i++)
        for(j = 0 ; j< vismap->get_height(); j++)
            print_tile_at(i,j);

    SDL_Flip(screen);
};

void CDisplay::flush_vis()
{
    int i,j;
    for(i = 0 ; i < vismap->get_width() ; i++)
        for(j = 0 ; j< vismap->get_height(); j++)
            vismap->set_grid_at(i,j, UNSEEN);
}

CDisplay::CDisplay()
{
    vismap = new CMap(main_map.get_width(),main_map.get_height(), UNSEEN);
    last_message_nb = -1;
    offset = 0;
};
CDisplay::CDisplay(char * path)
{
    vismap = new CMap(path);
}
void CDisplay::save_vis(char * path) const
{
    vismap->save(path);
};
CDisplay::~CDisplay()
{
    delete vismap;
};

void CDisplay::gameprintf(string message, ...)
/* printf-like method with one line per call, auto-add line break*/
{
    last_message_nb ++;
    va_list ap;
    va_start (ap, message);
    vsprintf(message_buf[last_message_nb%20], message.c_str(), ap); //push formated string
    va_end (ap);
    message_buf[last_message_nb%20][0] = toupper(message_buf[last_message_nb%20][0]);
    offset = 0;
};
void CDisplay::jump_relative(int n)
{
    offset = max(max(offset + n, -15),-last_message_nb + 3) ;
    offset = min(offset, 0);
    draw();
}

