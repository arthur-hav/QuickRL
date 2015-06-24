#include "globals.h"
#include "creatures.h"
#include <unordered_map>

unordered_map<unsigned int, CCreature *> creatures_map;

void inline CCreature::map()
{creatures_map[x + (y << 8)] = this;};
void inline CCreature::unmap()
{creatures_map.erase(x + (y << 8));};

CCreature * get_creature_at(u_char x, u_char y)
{ return creatures_map[x + (y << 8)]; }

CCreature::CCreature(u_char x, u_char y, int creature_kind, float HP) :
CEntity( creature_kind ), x(x), y(y), HP(HP)
{
    memset(inventory, 0, 24 * sizeof(CItem *));
    memset(status, 0, 4 * sizeof(int));
    armor = NULL;
    weapon = NULL;
    map();
}

CPlayer::CPlayer(u_char x, u_char y) :
CCreature(x, y, PLAYER, 100.0)
{}

CCreature::~CCreature()
{
    int i;
    for (i = 0; i<24; i++)
        if (inventory[i]){
            inventory[i]->map (x, y);
            delete inventory[i];
        }
}

unsigned int CCreature::act()
{
    if(creatures_map[x + (y << 8)] != this)
    {
        delete this;
        return 0;
    }
    return think();
}

unsigned int CCreature::item_use (int inventory_nb)
{
    CItem *item = inventory[inventory_nb];
    unsigned int time = item->use(this);
    if (item->count <= 0){
        delete item;
        inventory[inventory_nb] = NULL;
    }
    return time;
}

float CCreature::get_attack_dmg()
{
    if(weapon)
        return weapon->get_attack_dmg();
    return CREATURES[entity_number].attack_dmg;
}

int CCreature::get_AP()
{
    if(weapon)
        return weapon->get_AP();
    return CREATURES[entity_number].AP;
}

unsigned int CCreature::get_attack_time()
{
    unsigned int time;
    if(weapon)
        time = weapon->get_attack_time();
    else
        time = CREATURES[entity_number].attack_time;
    if(status[HASTED] > dungeontime )
        time *= 0.5;
    if(status[SLOWED] > dungeontime)
        time *= 2;
    return time;
}

float CCreature::get_armor()
{
    if(armor)
        return CREATURES[entity_number].base_armor + armor->get_armor();
    return CREATURES[entity_number].base_armor;
}

unsigned int CCreature::get_move_time()
{
    unsigned int time;
    if(armor)
        time = armor->get_move_time();
    else
        time = CREATURES[entity_number].move_time;
    if(status[SWIFTED] > dungeontime )
        time *= 0.5;
    if(status[SLUGGISH] > dungeontime )
        time *= 2;
    return time;
}

float CCreature::get_maxHP()
{ return CREATURES[entity_number].maxHP; }

float CCreature::get_fov_rad()
{ return CREATURES[entity_number].fov_rad; }


bool CCreature::step_to(u_char aim_x, u_char aim_y)
{
    if (fabs(aim_y - y) <= aim_x - x && trymove(1,0))
        return true;
    if (fabs(aim_x - x) <= aim_y - y && trymove(0,1))
        return true;
    if (y > aim_y && trymove(0,-1))
        return true;
    if (x > aim_x && trymove(-1,0))
        return true;
    return false;
}
bool CCreature::trymove( int relative_x, int relative_y )
{
    if(main_map.get_grid_at(x + relative_x,y + relative_y) == WALL_CHAR)
        return false;
    if (get_creature_at(x + relative_x, y + relative_y))
        return false;
    unmap();
    x += relative_x; y += relative_y;
    map();
    return true;
}
bool CCreature::tryattack( int relative_x, int relative_y )
{
    if (! (relative_x != 0) ^ (relative_y != 0))
        return false;
    if(abs(relative_x) > 1 || abs(relative_y) > 1)
        return false;
    CCreature * creat = get_creature_at(x + relative_x, y + relative_y);
    if (!creat)
        return false;
    //disp.gameprintf("%s attacks %s.", get_name(), creat->get_name());
    creat->takedamage(get_attack_dmg(), get_AP());
    return true;
}
void CCreature::takedamage(float dmg, int AP)
{
    dmg -= get_armor() * (100 - AP) / 100.0;
    dmg = min(HP, dmg);
    HP -= dmg;
    disp.gameprintf(get_name () + " takes %.0f point(s) of damage.", dmg);
    if(HP <= 0)
    {
        unmap();
        disp.gameprintf(get_name () + " dies.");
        //Memory cleaned at his next predicted action
    }
}


void CCreature::pick()
{
    int i, j;
    CItem * item;
    bool picked;
    int nb;

    for (j = 0; j < get_item_count_at(x, y); j++)
    {
        picked = false;
        item = get_item_at(x, y, j);
        nb = item->get_entity_nb ();
        if (nb == EMERALD || nb == SAPHIR || nb == RUBIS)
        {
            item->use(this);
            item->unmap(x, y);
            delete item;
            continue;
        }
        else {
            for (i = 0; i < 24; i++)
                if (inventory[i] && inventory[i]->get_entity_nb () == nb) {
                    inventory[i]->count += item->count;
                    item->unmap(x, y);
                    delete item;
                    picked = true;
                    break;
                }
        }
        if (!picked)
            for (i = 0; i < 24; i++)
                if (!inventory[i]) {
                    inventory[i] = item;
                    item->unmap(x, y);
                    break;
                }
    }
}

bool CCreature::trydrop(int item_nb)
{
    CItem * item = inventory[item_nb];
    if (item)
    {
        item->map(x, y);
        disp.gameprintf("You drop " + item->get_name() + ".");
        inventory[item_nb] = NULL;
        return true;
    }
    return false;
}

CMob::CMob (u_char x, u_char y) :
CCreature(x, y, GOBELIN, 25.0f)
{
    chasing = false;
};
unsigned int CMob::think()
{
    int losx = x;
    int losy = y;
    if(los(losx, losy, PC->x, PC->y, get_fov_rad()))
    {
        last_seen_x = PC->x;
        last_seen_y = PC->y;
        chasing = true;
    }
    if (chasing)
    {
        if(tryattack(last_seen_x - x, last_seen_y - y))
            return get_attack_time();
        if(step_to(last_seen_x, last_seen_y))
            return get_move_time();
    }
    return get_move_time(); //idle
}

void flush_creatures (){
    creatures_map.clear();
}

CCreature * CMob::get_target()
{return NULL;} //TODO

