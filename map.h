#ifndef MAP_H
#define MAP_H

#define FLOOR_CHAR '.'
#define WALL_CHAR '#'
#define STAIRS_DOWN_CHAR '>'
#define DOOR_CHAR '+'


class CMap
{
        void init(char initial_value);
        void populate(int xmin, int ymin, int xmax, int ymax, int * perm, int * p_i);
    protected:
        u_char width, height;
        char ** grid;
    public:
        CMap(u_char w, u_char h, char initial_value = 0);
        CMap(const char * path);

        ~CMap();

        u_char get_width() const;
        u_char get_height() const;
        char get_grid_at(u_char x, u_char y) const;
        void set_grid_at(u_char x, u_char y, char value);
        void generate();
        void generate_bsp(int xmin, int ymin, int xmax, int ymax, int recursions, int * perm, int* p_i);


        bool is_in_map_bounds(int x, int y) const;
        void save(const char * path) const;
};

extern CMap main_map;

#endif
