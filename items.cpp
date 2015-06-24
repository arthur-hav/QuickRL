#include "globals.h"
#include "items.h"
#include <cstdlib>
#include <unordered_map>

unordered_multimap<unsigned int,CItem *> items_map;

CItem::CItem(int item_id, int initial_count) :
CEntity(item_id), count(initial_count)
{}

CItem::~CItem()
{}

CArmor::CArmor(int item_id, int initial_count, float armor, unsigned int move_time) :
CItem (item_id, initial_count), armor(armor), move_time(move_time)
{}

CArmor::~CArmor()
{}

CWeapon::CWeapon(int item_id, int initial_count, float attack_dmg, unsigned int attack_time, int AP) :
CItem (item_id, initial_count), attack_dmg(attack_dmg), attack_time(attack_time), AP(AP)
{}

CWeapon::~CWeapon()
{}

CItem * get_item_at(u_char x, u_char y, int n)
{
    int i;
    unordered_multimap<unsigned int,CItem *>::iterator it;
    it=items_map.equal_range(x + (y << 8)).first;
    for(i=0; i<n ; ++it)
        ;
    if(it!=items_map.equal_range(x + (y << 8)).second)
        return it->second;
    return NULL;
}

int get_item_count_at(u_char x, u_char y)
{
    return items_map.count(x + (y << 8) );
}

void flush_items()
{
    unordered_multimap<unsigned int, CItem *>::iterator it;
    for (it = items_map.begin();
        it != items_map.end(); ++it)
        delete it->second;
    items_map.clear();
}

void CItem::map(u_char x, u_char y)
{items_map.insert( pair<unsigned int, CItem *>(x + (y << 8) , this) );};

void CItem::unmap(u_char x,u_char y)
{
    unordered_multimap<unsigned int,CItem *>::iterator it;

    for (it=items_map.equal_range(x + (y << 8) ).first;
         it!=items_map.equal_range(x + (y << 8) ).second; ++it)

        if (it->second == this)
        {
            items_map.erase(it);
            return;
        }
};

unsigned int CItem::use (CCreature* user)
{
    switch (entity_number) {
        case SHURIKEN : 
        {
            CCreature *target = user->get_target();
            target->takedamage(5, 1);
            count--;
            //last_used = dungeontime;
            return 50;
        }
        case JAVELIN :
        {
            CCreature *target = user->get_target();
            target->takedamage(12, 2);
            count--;
            //last_used = dungeontime;
            return 100;
        }
        case HEALTH_POTION :
        {
            user->HP += 0.5 * (user->get_maxHP() - user->HP);
            count--;
            return 100;
        }
        case EMERALD :
        {
            player_score += 100;
            return 0;
        }
        case SAPHIR :
        {
            player_score += 250;
            return 0;
        }
        case RUBIS :
        {
            player_score += 500;
            return 0;
        }
        default :
        {
            disp.gameprintf("Nothing happens.");
            return 0;
        }
    }
}
//CItem( KNIFE ), CWeapon( 5.0, 60, 0 )
//CItem( STEEL_BREASTPLATE ), CArmor( 2.0, 120 )


float CWeapon::get_attack_dmg()
{
    return attack_dmg;
}

int CWeapon::get_AP()
{
    return AP;
}
unsigned int CWeapon::use (CCreature* user)
{
    user->weapon = this;
    return 100;
}

unsigned int CArmor::use (CCreature* user)
{
    user->armor = this;
    return 100;
}

unsigned int CWeapon::get_attack_time()
{
    return attack_time;
}
float CArmor::get_armor()
{
    return armor;
}

unsigned int CArmor::get_move_time()
{
    return move_time;
}

