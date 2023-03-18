#ifndef MAP_H
#define MAP_H

typedef struct
{
    int size;
    int** map;
}
MapInfo;

/*
Generates a map of size*size
Returns NULL on failure
In the map: 0 represents an empty square, 1 represents a wall, 2 represents a wall with a different texture and without collision
destroy_map should be called once the map isn't needed anymore
*/
MapInfo* generate_map(int size);

/*
Frees the map
*/
void destroy_map(MapInfo* map);

#endif
