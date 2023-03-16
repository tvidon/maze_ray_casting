#ifndef MAP_H
#define MAP_H

/*
Generates a map of size*size
Returns NULL on failure
destroy_map should be called once the map isn't needed anymore
*/
int** generate_map(int size);

/*
Frees the map
*/
void destroy_map(int** map);

#endif
