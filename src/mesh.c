#include "mesh.h"

// -------------------------------- 4D MESH -------------------------------- //

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                  const int (*edges)[2], int edge_count,
                  float radius)
{
    Mesh4D mesh = {0};
    mesh.vertex_count = vertex_count;
    mesh.edge_count = edge_count;

    if (!vertices && !vertex_count && !edges && !edge_count)
    {
        // create an empty mesh
        return (Mesh4D){0};
    }

    mesh.vertices = malloc(sizeof(Vector4) * vertex_count);
    memcpy(mesh.vertices, vertices, sizeof(Vector4) * vertex_count);

    mesh.edges = malloc(sizeof(int[2]) * edge_count);
    memcpy(mesh.edges, edges, sizeof(int[2]) * edge_count);

    mesh.radius = radius;

    return mesh;
}

void mesh4d_free(Mesh4D* mesh)
{
    if (!mesh)
    {
        return;
    }

    if (mesh->vertices)
    {
        free(mesh->vertices);
    }
    if (mesh->edges)
    {
        free(mesh->edges);
    }
    mesh->vertices = NULL;
    mesh->edges = NULL;
    mesh->vertex_count = 0;
    mesh->edge_count = 0;
    mesh->radius = 0;
}

Mesh4D mesh4d_new_empty()
{
    return mesh4d_new(NULL, 0, NULL, 0, 0);
}

Mesh4D mesh4d_new_tetra()
{
    static const Vector4 VERTICES[4] = {
        {  1,  1,  1, 0 },
        {  1, -1, -1, 0 },
        { -1,  1, -1, 0 },
        { -1, -1,  1, 0 },
    };
    static const int EDGES[6][2] = {
        {0,1}, {0,2}, {0,3},
        {1,2}, {1,3},
        {2,3},
    };
    return mesh4d_new(VERTICES, 4, EDGES, 6, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_cube()
{
    static const Vector4 VERTICES[8] = {
        { -1, -1, -1, 0 }, {  1, -1, -1, 0 }, { -1,  1, -1, 0 }, {  1,  1, -1, 0 },
        { -1, -1,  1, 0 }, {  1, -1,  1, 0 }, { -1,  1,  1, 0 }, {  1,  1,  1, 0 },
    };
    static const int EDGES[12][2] = {
        {0,1}, {0,2}, {0,4},
        {1,3}, {1,5},
        {2,3}, {2,6},
        {3,7},
        {4,5}, {4,6},
        {5,7},
        {6,7},
    };
    return mesh4d_new(VERTICES, 8, EDGES, 12, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_oct()
{
    static const Vector4 VERTICES[6] = {
        {  1,  0,  0, 0 },
        { -1,  0,  0, 0 },
        {  0,  1,  0, 0 },
        {  0, -1,  0, 0 },
        {  0,  0,  1, 0 },
        {  0,  0, -1, 0 },
    };

    static const int EDGES[12][2] = {
        {0, 2}, {2, 1}, {1, 3}, {3, 0},
        {4, 0}, {4, 1}, {4, 2}, {4, 3},
        {5, 0}, {5, 1}, {5, 2}, {5, 3},
    };

    return mesh4d_new(VERTICES, 6, EDGES, 12, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_ico()
{
    static const float phi = 1.618033988749895f;

    static const Vector4 VERTICES[12] = {
        { 0,  1,  phi, 0 },
        { 0,  1, -phi, 0 },
        { 0, -1,  phi, 0 },
        { 0, -1, -phi, 0 },
        {  1,  phi, 0, 0 },
        {  1, -phi, 0, 0 },
        { -1,  phi, 0, 0 },
        { -1, -phi, 0, 0 },
        {  phi, 0,  1, 0 },
        {  phi, 0, -1, 0 },
        { -phi, 0,  1, 0 },
        { -phi, 0, -1, 0 }
    };

    static const int EDGES[30][2] = {
        {0, 2}, {1, 3}, {4, 6}, {5, 7}, {8, 9}, {10, 11},
        {0, 4}, {0, 6}, {1, 4}, {1, 6}, {2, 5}, {2, 7}, {3, 5}, {3, 7},
        {0, 8}, {0, 10}, {2, 8}, {2, 10}, {1, 9}, {1, 11}, {3, 9}, {3, 11},
        {4, 8}, {4, 9}, {6, 10}, {6, 11}, {5, 8}, {5, 9}, {7, 10}, {7, 11}
    };

    return mesh4d_new(VERTICES, 12, EDGES, 30, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_5cell()
{
    static const Vector4 VERTICES[5] = {
        {  1,  1,  1, -1.0f / 2.236068f },
        {  1, -1, -1, -1.0f / 2.236068f },
        { -1,  1, -1, -1.0f / 2.236068f },
        { -1, -1,  1, -1.0f / 2.236068f },
        {  0,  0,  0,  4.0f / 2.236068f },
    };
    static const int EDGES[10][2] = {
        {0,1}, {0,2}, {0,3}, {0,4},
        {1,2}, {1,3}, {1,4},
        {2,3}, {2,4},
        {3,4},
    };
    return mesh4d_new(VERTICES, 5, EDGES, 10, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_8cell()
{
    static const Vector4 VERTICES[16] = {
        { -1, -1, -1, -1 }, {  1, -1, -1, -1 }, { -1,  1, -1, -1 }, {  1,  1, -1, -1 },
        { -1, -1,  1, -1 }, {  1, -1,  1, -1 }, { -1,  1,  1, -1 }, {  1,  1,  1, -1 },
        { -1, -1, -1,  1 }, {  1, -1, -1,  1 }, { -1,  1, -1,  1 }, {  1,  1, -1,  1 },
        { -1, -1,  1,  1 }, {  1, -1,  1,  1 }, { -1,  1,  1,  1 }, {  1,  1,  1,  1 },
    };

    static const int EDGES[32][2] = {
        {0,1},{0,2},{0,4},{0,8}, {1,3},{1,5},{1,9},
        {2,3},{2,6},{2,10}, {3,7},{3,11},
        {4,5},{4,6},{4,12}, {5,7},{5,13},
        {6,7},{6,14}, {7,15},
        {8,9},{8,10},{8,12}, {9,11},{9,13},
        {10,11},{10,14}, {11,15},
        {12,13},{12,14}, {13,15},
        {14,15},
    };

    return mesh4d_new(VERTICES, 16, EDGES, 32, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_16cell()
{
    static const Vector4 VERTICES[8] = {
        {  1,  0,  0,  0 }, { -1,  0,  0,  0 },
        {  0,  1,  0,  0 }, {  0, -1,  0,  0 },
        {  0,  0,  1,  0 }, {  0,  0, -1,  0 },
        {  0,  0,  0,  1 }, {  0,  0,  0, -1 },
    };

    static const int EDGES[24][2] = {
        {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7},
        {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7},
        {2,4}, {2,5}, {2,6}, {2,7},
        {3,4}, {3,5}, {3,6}, {3,7},
        {4,6}, {4,7},
        {5,6}, {5,7},
    };

    return mesh4d_new(VERTICES, 8, EDGES, 24, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_24cell()
{
    Vector4 verts[24];
    int count = 0;

    int axis_pairs[6][2] = {
        {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3},
    };

    for (int p = 0; p < 6; p += 1)
    {
        int a = axis_pairs[p][0];
        int b = axis_pairs[p][1];

        for (int sa = -1; sa <= 1; sa += 2)
        {
            for (int sb = -1; sb <= 1; sb += 2)
            {
                Vector4 v = { 0, 0, 0, 0 };
                float* coords = (float*)&v;
                coords[a] = (float)sa;
                coords[b] = (float)sb;
                verts[count] = v;
                count += 1;
            }
        }
    }

    int edges[96][2];
    int edge_count = 0;
    float edge_len_sq = 2.0f;

    for (int i = 0; i < 24; i += 1)
    {
        for (int j = i + 1; j < 24; j += 1)
        {
            float dx = verts[i].x - verts[j].x;
            float dy = verts[i].y - verts[j].y;
            float dz = verts[i].z - verts[j].z;
            float dw = verts[i].w - verts[j].w;
            float dist_sq = dx*dx + dy*dy + dz*dz + dw*dw;

            if (fabsf(dist_sq - edge_len_sq) < 0.001f)
            {
                edges[edge_count][0] = i;
                edges[edge_count][1] = j;
                edge_count += 1;
            }
        }
    }

    return mesh4d_new(verts, 24, (const int (*)[2])edges, edge_count, sqrtf(2.f));
}
