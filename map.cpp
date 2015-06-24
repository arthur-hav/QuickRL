#include "globals.h"
#include "main.h"
#include "map.h"
#include "items.h"
#include "creatures.h"

void CMap::init(char initial_value)
{
    int i;
    grid = new char*[height];
    for(i = 0; i < height; i++)
    {
        grid[i] = new char[width];
        memset(grid[i],initial_value,width);
    }
}

CMap::CMap(u_char w, u_char h, char initial_value)
{
    width = w;
    height = h;
    init(initial_value);
};
CMap::CMap(const char * path) //load from file
{
    FILE*FileIN=fopen(path,"r");
    char buf[8];

    fread(buf, 4, 1, FileIN);
    width = atoi(buf);

    fread(buf, 4, 1, FileIN);
    height = atoi(buf);

    init(0);

    fseek(FileIN, 1, SEEK_CUR);//skip '\n'
    int i;

    for(i=0 ; i < height ; i++)
        fscanf(FileIN, "%s\n", grid[i]);

    fclose(FileIN);
};
CMap::~CMap()
{
    int i;
    for(i=0 ; i < height ; i++)
        delete [] grid[i];
    delete[] grid;
};

u_char CMap::get_width() const
{
    return width;
};
u_char CMap::get_height() const
{
    return height;
};
char CMap::get_grid_at(u_char x, u_char y) const
{
    assert(is_in_map_bounds(x,y));
    return grid[y][x];
};
void CMap::set_grid_at(u_char x, u_char y, char value)
{
    assert(is_in_map_bounds(x,y));
    grid[y][x] = value;
};

void CMap::save(const char * path) const
{
    int i;
    FILE * FileOUT = fopen(path,"w");
    fprintf(FileOUT, "%04d%04d\n", width, height);//dungeon dimensions
    for ( i = 0; i < height; i++)
    {
        fwrite(grid[i], 1, width, FileOUT);
        fwrite("\n", 1, 1, FileOUT);
    }
    fclose(FileOUT);
};
bool CMap::is_in_map_bounds(int x, int y) const
{
    return ( x >= 0 && x < width && y >= 0 && y < height );
};




void CMap::generate_bsp(int xmin, int ymin, int xmax, int ymax, int recursions, int * perm, int * p_i)
{
    double splitfrac;
    int xsplit, ysplit;

    if (recursions <= 0)
    {
        populate(xmin, ymin, xmax, ymax, perm, p_i);
        return;
    }

    splitfrac = 0.35 + rand()%300 / 1000.0;
    xsplit = xmin + (int)((splitfrac * (xmax - xmin)) + 0.5);
    ysplit = ymin + (int)((splitfrac * (ymax - ymin)) + 0.5);

    if(recursions % 2 == 0)
    //vertical split
    {
        int tries, door;

        generate_bsp(xmin, ymin, xsplit, ymax, recursions-1, perm, p_i);
        generate_bsp(xsplit, ymin, xmax, ymax, recursions-1, perm, p_i);

        for(tries = 0; tries < 100; tries++)
        {
            door = ymin + (rand() % (ymax-ymin-1));
            if( grid[door][xsplit-2] == FLOOR_CHAR && grid[door][xsplit] == FLOOR_CHAR )
            {
                grid[door][xsplit-1] = DOOR_CHAR;
                break;
            }
        }
    }

    else
    //horizontal split
    {
        int tries, door;

        generate_bsp(xmin, ymin, xmax, ysplit, recursions-1, perm, p_i);
        generate_bsp(xmin, ysplit, xmax, ymax, recursions-1, perm, p_i);

        for(tries = 0; tries < 100; tries++)
        {
            door = xmin + (rand() % (xmax-xmin-1));
            if( grid[ysplit-2][door] == FLOOR_CHAR && grid[ysplit][door] == FLOOR_CHAR )
            {
                grid[ysplit-1][door] = DOOR_CHAR;
                break;
            }
        }
    }


}

void random_perm(int * perm, int size)
//Knuth shuffle
{
    int i, ri, tmp;
    for (i=0; i < size; i++)
        perm[i] = i;
    for (i=0; i < size - 1; i++)
    {
        ri = rand()%size;
        tmp = perm[i];
        perm[i] = perm[ri];
        perm[ri] = tmp;
    }

}

void CMap::generate()
{
    int xi, yi, p_i;
    int perm[16];

    random_perm(perm, 16);
    p_i = 0;

    for(yi=0; yi<height; yi++)
        for(xi=0; xi<width; xi++)
            grid[yi][xi] = WALL_CHAR;

    disp.flush_vis();
    flush_creatures ();
    flush_items();

    generate_bsp(1,1, width,height, 4, perm, &p_i);

}


void CMap::populate(int xmin, int ymin, int xmax, int ymax, int * perm, int * p_i)
{
    int xi, yi, xmid, ymid;
    for(yi=ymin; yi<ymax-1; yi++)
        for(xi=xmin; xi<xmax-1; xi++)
            grid[yi][xi] = FLOOR_CHAR;
    xmid = xmin + rand()%(xmax-xmin-1);
    ymid = ymin + rand()%(ymax-ymin-1);
    if (perm[(*p_i)] == 0)
    {
        PC->unmap();
        PC->x = xmid;
        PC->y = ymid;
        PC->map();
    }
    else if (perm[(*p_i)] == 1)
        grid[ymid][xmid] = STAIRS_DOWN_CHAR;

    else
    {
        int gemprob = 2000 / ( player_floor + 5 );
        int itemprob = 3000 / ( player_floor + 10 );
        int randint = rand()%1000;
        if(randint < gemprob)
        {
            CItem * item;
            randint = rand()%10;
            if ( randint + player_floor < 10 )
                item = new CItem(EMERALD, 1);
            else if ( randint + player_floor < 13 )
                item = new CItem(SAPHIR, 1);
            else
                item = new CItem(RUBIS, 1);
            item->map(xmid,ymid);
        }
        else if (randint < (gemprob + itemprob))
        {
            randint = rand()%5;
            CItem * item;
            if (randint == 0)
                item = new CItem(SHURIKEN, 5+ rand()%6);
            else if(randint == 1)
                item = new CItem(JAVELIN, 2+ rand()%3);
            else if(randint == 2)
                item = new CItem(HEALTH_POTION, 1);
            else if(randint == 3)
                item = new CArmor(STEEL_BREASTPLATE, 1, 4, 100);
            else 
                item = new CWeapon(KNIFE, 1, 10, 80, 10);
            item->map(xmid, ymid);
        }
        else
        {
            CMob* mob = new CMob(xmid, ymid);
            event_queue.push(CGameEvent( dungeontime, mob ));
        }
    }
    (*p_i)++;
}


