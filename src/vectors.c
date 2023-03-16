#include <math.h>

#include "vectors.h"



void rotate_vector(Vector *vector, double angle)
{
    double prevX = vector->x;
    vector->x = vector->x * cos(angle) - vector->y * sin(angle);
    vector->y = prevX * sin(angle) + vector->y * cos(angle);
}
