#ifndef MESH_H
#define MESH_H

#include "common.h"

// -------------------------------- 4D MESH -------------------------------- //

typedef struct{
    Vector4* vertices;
    int vertex_count;
    int (*edges)[2];
    int edge_count;
    float radius;
} Mesh4D;

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                  const int (*edges)[2], int edge_count,
                  float radius);
void mesh4d_free(Mesh4D* mesh);

Mesh4D mesh4d_new_empty();

// 3d shapes - w=0
Mesh4D mesh4d_new_tetra();
Mesh4D mesh4d_new_cube();
Mesh4D mesh4d_new_oct();
Mesh4D mesh4d_new_ico();

// 4d shapes
Mesh4D mesh4d_new_5cell();
Mesh4D mesh4d_new_8cell();
Mesh4D mesh4d_new_16cell();
Mesh4D mesh4d_new_24cell();


#endif // MESH_H
