#ifndef MAP_H
#define MAP_H

/*
Generates a map of size*size
Returns NULL on failure
In the map: 0 represents an empty square, 1 represents a wall, 2 represents a wall with a different texture and without collision
destroy_map should be called once the map isn't needed anymore
*/
int** generate_map(int size);

/*
Frees the map
*/
void destroy_map(int** map);

#endif
