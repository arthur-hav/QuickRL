#ifndef CREATURES_H
#define CREATURES_H

#include "entity.h"
#include "items.h"

class CCreature : public CEntity
{

    protected:
        bool step_to(u_char aim_x, u_char aim_y);
        bool trymove( int relative_x, int relative_y );
        bool tryattack( int relative_x, int relative_y );
        void pick();
        bool trydrop(int item_nb);
        virtual unsigned int think() =0;

    public:
        CCreature(u_char x, u_char y, int creature_kind, float HP);

        ~CCreature();

        u_char x, y;
        float get_attack_dmg();
        unsigned int get_attack_time();
        int get_AP();
        float get_armor();
        unsigned int get_move_time();
        float get_maxHP();
        float get_fov_rad();
        virtual CCreature * get_target() =0;
        void map();
        void unmap();
        unsigned int item_use (int inventory_nb);

        float HP;
        CWeapon * weapon;
        CArmor * armor;
        CItem * inventory[24];
        unsigned int status[4];

        void takedamage(float dmg, int AP);
        unsigned int act();
};

class CPlayer : public CCreature
{
    protected:
        unsigned int think();
    public:
        CPlayer (u_char x, u_char y);
        CCreature * get_target();

};

class CMob : public CCreature
{
    protected:
        unsigned int think();
        u_char last_seen_x, last_seen_y;
        bool chasing;
    public:
        CMob (u_char x, u_char y);
        CCreature * get_target();
};


CCreature * get_creature_at(u_char x, u_char y);
void flush_creatures ();

#endif
