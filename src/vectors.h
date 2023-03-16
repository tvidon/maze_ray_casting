#ifndef VECTORS_H
#define VECTORS_H

typedef struct
{
    double x;
    double y;
}
Vector;

/*
Rotates the vector
angle is in radians
*/
void rotate_vector(Vector* vector, double angle);

#endif
