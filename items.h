#ifndef ITEMS_H
#define ITEMS_H

#include "entity.h"
#include "creatures.h"
class CCreature;

class CItem : public CEntity
{
    public:
        int count;
        void map(u_char x, u_char y);
        void unmap(u_char x, u_char y);
        virtual unsigned int use (CCreature *user);
        CItem(int item_id, int initial_count);
        virtual ~CItem ();
};

class CWeapon : public CItem
{
        float attack_dmg;
        unsigned int attack_time;
        int AP;

    public:
        float get_attack_dmg();
        unsigned int get_attack_time();
        int get_AP();
        unsigned int use (CCreature *user); //Override
        CWeapon(int item_id, int initial_count, float attack_dmg, unsigned int attack_time, int AP);
        virtual ~CWeapon ();
};

class CArmor : public CItem
{
        float armor;
        unsigned int move_time;

    public:
        float get_armor();
        unsigned int get_move_time();
        unsigned int use (CCreature *user); //Override
        CArmor(int item_id, int initial_count, float armor, unsigned int move_time);
        virtual ~CArmor();
};

CItem * get_item_at(u_char x, u_char y, int n);
int get_item_count_at(u_char x, u_char y);
void flush_items();

#endif
