#ifndef GLOBALS_H
#define GLOBALS_H

#include "main.h"

enum
{
    PLAYER,
    GOBELIN,

    MAX_CREATURES
};

enum
{
    SHURIKEN = MAX_CREATURES,
    JAVELIN,
    HEALTH_POTION,
    EMERALD,
    SAPHIR,
    RUBIS,
    KNIFE,
    STEEL_BREASTPLATE,

    MAX_NAMED
};

enum
{
    FLOOR = MAX_NAMED,
    DOOR,
    STAIRS_DOWN,
    WALL0,
    WALL1,
    WALL2,
    WALL3,
    WALL4,
    WALL5,
    WALL6,
    WALL7,
    WALL8,
    WALL9,
    WALL10,
    WALL11,
    WALL12,
    WALL13,
    WALL14,
    WALL15,
    MOUSE_OUTLINE,

    MAX_TILESET
};

extern SDL_Surface * MODELS[MAX_TILESET];
extern string NAMES[MAX_NAMED];

void init_models();

struct creature_constants
{
    string name;
    int model_id;
    float attack_dmg;
    unsigned int attack_time;
    int AP;
    float base_armor;
    unsigned int move_time;
    float maxHP;
    float fov_rad;
};
extern struct creature_constants CREATURES[MAX_CREATURES];

#endif
