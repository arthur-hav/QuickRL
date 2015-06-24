#ifndef ENTITY_H
#define ENTITY_H

#include <SDL/SDL.h>
#include <string>

using namespace std;

class CEntity
{
    protected:
        int entity_number;

    public :
        std::string get_name ();
        SDL_Surface *get_model();
        int get_entity_nb ();

        void print(int x, int y);
        CEntity(int entity_number);
        virtual ~CEntity ();
};


#endif
