#include <stdio.h>
#include <stdlib.h>

#include "map.h"



// Durstenfeld version of the Fisher-Yates shuffle
static void shuffle(int list[], int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = (rand() % (i + 1)); // not perfectly uniform but quick
        int temp = list[j];
        list[j] = list[i];
        list[i] = temp;
    }
}


// Carve a maze using the recursive backtracking algorithm
static void carve(int x, int y, MapInfo* map)
{
    int directions[] = {1, 2, 3, 4}; // up, down, left, right
    shuffle(directions, 4);
    for (int i = 0; i < 4; i++)
    {
        if (directions[i] == 1)
        {
            int new_y = y + 2;
            if (new_y < map->size && map->map[x][new_y] == -1)
            {
                map->map[x][new_y - 1] = 0;
                map->map[x][new_y] = 0;
                carve(x, new_y, map);
            }
        }
        else if (directions[i] == 2)
        {
            int new_y = y - 2;
            if (new_y > 0 && map->map[x][new_y] == -1) // no need to check y 0
            {
                map->map[x][new_y + 1] = 0;
                map->map[x][new_y] = 0;
                carve(x, new_y, map);
            }
        }
        else if (directions[i] == 3)
        {
            int new_x = x - 2;
            if (new_x > 0 && map->map[new_x][y] == -1) // no need to check x 0
            {
                map->map[new_x + 1][y] = 0;
                map->map[new_x][y] = 0;
                carve(new_x, y, map);
            }
        }
        else
        {
            int new_x = x + 2;
            if (new_x < map->size && map->map[new_x][y] == -1)
            {
                map->map[new_x - 1][y] = 0;
                map->map[new_x][y] = 0;
                carve(new_x, y, map);
            }
        }
    }
}


MapInfo* generate_map(int size)
{
    // MAKE SURE MAP SIZE IS APPROPRIATE

    // 5 by 5 minimum
    if (size < 5) size = 5;

    // the recursive backtracking algorithm requires an odd map width and height
    if (!(size % 2)) size--;
    
    // INITIALIZE RETURN POINTER

    MapInfo* map= malloc(sizeof(MapInfo));
    if (!map)
    {
        printf("error attempting to allocate memory for the map info\n");
        return NULL;
    }
    map->map = malloc(size * sizeof(int*));
    if (!map->map)
    {
        printf("error attempting to allocate memory for the map\n");
        free(map);
        return NULL;
    }
    for (int x = 0; x < size; x++)
    {
        map->map[x] = malloc(size * sizeof(int));
        if (!map->map[x])
        {
            printf("error attempting to allocate memory for the map\n");
            for (int free_x = 0; free_x < x; free_x++)
                free(map->map[free_x]);
            free(map->map);
            free(map);
            return NULL;
        }
    }
    map->size = size;
    
    // GENERATE MAP
    
    // uses the recursive backtracking algorithm

    // generate all walls
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            if ((x % 2) == 0 || (y % 2) == 0) map->map[x][y] = 1;
            else map->map[x][y] = -1; // - 1 is an unexplored empty square
        }
    }

    // carve paths
    // starting from the exit to avoid having some branches that are only accessible if you walk by the exit
    map->map[size - 2][size - 2] = 0;
    carve(size - 2, size - 2, map);

    // remove (map->size / 6) random walls near the center to:
    // - make up for the low branching of the recursive backtracking algorithm
    // - add loops to the maze
    // this probably makes the maze easier but it also makes it more interesting
    int third = size / 3;
    for (int i = 0; i < map->size / 6; i++)
    {
        int rand_x = third + (rand() % third);
        int rand_y = third + (rand() % third);
        if ((rand_x % 2 && rand_y % 2) || !(rand_x % 2 || rand_y % 2)) rand_y++;
        if (map->map[rand_x][rand_y]) map->map[rand_x][rand_y] = 0;
        else i--;
    }

    // add exit
    map->map[size - 1][size - 2] = 2;

    return map;
}

void destroy_map(MapInfo* map)
{
    for (int x = 0; x < map->size; x++)
        free(map->map[x]);
    free(map->map);
    free(map);
}
