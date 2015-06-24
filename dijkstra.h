#include "main.h"

typedef list<int> path_t;

path_t compute_Dijktra(CMap * map, u_char begin_x, u_char begin_y, u_char end_x, u_char end_y);
void path_advance(path_t path, u_char * x, u_char * y);
