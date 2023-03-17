#include <stdio.h>
#include <stdlib.h>

#include "map.h"



int** generate_map(int size)
{
    // INITIALIZE RETURN POINTER

    int** map = malloc(size * sizeof(int*));
    if (!map)
    {
        printf("error attempting to allocate memory for the map\n");
        return NULL;
    }
    for (int i = 0; i < 10; i++)
    {
        map[i] = malloc(size * sizeof(int));
        if (!map[i])
        {
            printf("error attempting to allocate memory for the map\n");
            return NULL;
        }
    }
    
    // GENERATE MAP

    int generated_map[10][10] =
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 1, 1, 1, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 2, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        };

    // fill up the output array
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            map[i][j] = generated_map[i][j];

    return map;
}

void destroy_map(int** map)
{
    for (int i = 0; i < 10; i++)
        free(map[i]);
    free(map);
}
