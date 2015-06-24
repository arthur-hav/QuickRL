#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>
#include <string.h>
#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <unordered_map>
#include <queue>
#include <list>
#include "map.h"
#include "creatures.h"

using namespace std;

#define SLOWED 0
#define HASTED 1
#define SLUGGISH 2
#define SWIFTED 3

/* GUI constants */

#define BORDER 17

#define INVENT_Y 150
#define INVENT_ROWS 4
#define INVENT_COLUMNS 6
#define HUD_W 192
#define CONS_H 72
#define DESC_Y 275


inline float max(float A, float B)
{ return A > B ? A : B; };
inline float min(float A, float B)
{ return A < B ? A : B; };

extern bool los(int & x0, int & y0, int x1, int y1, float maxdist = 0);
extern int item_for_mousexy(int mouse_x, int mouse_y);


extern SDL_Surface* screen;
extern unsigned int dungeontime;
extern int player_score;
extern int player_floor;

extern TTF_Font * console_font;
extern TTF_Font * console_font_outlined;
extern TTF_Font * count_font;
extern TTF_Font * desc_font;

class CCreature;
class CPlayer;

class CGameEvent
{
    public:
        unsigned int occur_time;
        CCreature * actor;
        CGameEvent();
        CGameEvent(unsigned int occur_time, CCreature * actor);
        bool operator < (const CGameEvent& event) const;

};

extern priority_queue<CGameEvent> event_queue;

extern CPlayer* PC;

class CDisplay
{
        CMap * vismap;
        char message_buf[20][65]; // 20 lines, max 64 chars
        int last_message_nb;
        int offset;

        void print_tile_at(int x, int y);
        void refresh_at(int x, int y);
    public:
        void draw();
        void flush_vis();
        void look_update(int mouse_x, int mouse_y);
        void gameprintf(string message, ...);
        void jump_relative(int n);
        void pointer_fx (SDL_Rect rect, SDL_Surface *sprite);

        CDisplay();
        CDisplay(char * path);
        void save_vis(char * path) const;
        ~CDisplay();
};

extern CDisplay disp;

#endif //MAIN_H
