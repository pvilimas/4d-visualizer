#include "mesh.h"

// -------------------------------- 4D MESH -------------------------------- //

VertexList vl_new(uint32_t size)
{
    if (!size)
    {
        return (VertexList){0};
    }

    return (VertexList) {
        .data = calloc(size, sizeof(Vector4)),
        .size = size
    };
}

void vl_free(VertexList* vl)
{
    if (!vl || !vl->data)
    {
        return;
    }

    free(vl->data);
    vl->data = NULL;
    vl->size = 0;
}

RefList rl_new(uint32_t size)
{
    if (!size)
    {
        return (RefList){0};
    }

    return (RefList) {
        .data = calloc(size, sizeof(uint32_t)),
        .rle_size = size,
        // these get initialized later in pm_normalize()
        .metas = NULL,
        .refcount = 0
    };
}

void rl_free(RefList* rl)
{
    if (!rl || !rl->data)
    {
        return;
    }

    free(rl->data);
    rl->data = NULL;
    rl->rle_size = 0;
    rl->refcount = 0;

    if (rl->metas)
    {
        free(rl->metas);
        rl->metas = NULL;
    }
}

const uint32_t* rl_get_ref(const RefList* rl, uint32_t index)
{
    uint32_t pos = 0;
    uint32_t current = 0;

    while (current < index)
    {
        uint32_t count = rl->data[pos];
        pos += 1 + count;
        current += 1;
    }

    return &rl->data[pos];
}

PolytopeMesh pm_new(uint8_t dimension)
{
    return (PolytopeMesh) {
        .layers = calloc(dimension + 1, sizeof(MeshLayer)),
        .dimension = dimension};
}

void pm_free(PolytopeMesh* pm)
{
    if (!pm || !pm->layers)
    {
        return;
    }

    vl_free(&pm->layers[0].sub.verts);

    for (uint32_t i = 1; i <= pm->dimension; i++)
    {
        rl_free(&pm->layers[i].sub.refs);
    }

    free(pm->layers);
    pm->layers = NULL;
    pm->dimension = 0;
}

void pm_reserve(PolytopeMesh* pm, uint8_t dimension, uint32_t num_elements)
{
    if (!pm || dimension > pm->dimension || num_elements == 0)
    {
        return;
    }

    if (dimension == 0)
    {
        vl_free(&pm->layers[0].sub.verts);
        pm->layers[0].sub.verts = vl_new(num_elements);
    }
    else
    {
        rl_free(&pm->layers[dimension].sub.refs);
        pm->layers[dimension].sub.refs = rl_new(num_elements);
    }
}

void pm_set_vertex(PolytopeMesh* pm, uint32_t index, Vector4 vertex)
{
    pm->layers[0].sub.verts.data[index] = vertex;
}

void pm_set_ref(PolytopeMesh* pm, uint8_t dimension, uint32_t index, uint32_t ref)
{
    pm->layers[dimension].sub.refs.data[index] = ref;
}

void pm_normalize(PolytopeMesh* pm)
{
    Vector4* verts = pm->layers[0].sub.verts.data;
    uint32_t vert_count = pm->layers[0].sub.verts.size;

    if (vert_count == 0)
    {
        return;
    }

    Vector4 centroid = { 0, 0, 0, 0 };
    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        centroid.x += verts[i].x;
        centroid.y += verts[i].y;
        centroid.z += verts[i].z;
        centroid.w += verts[i].w;
    }
    centroid.x /= (float)vert_count;
    centroid.y /= (float)vert_count;
    centroid.z /= (float)vert_count;
    centroid.w /= (float)vert_count;

    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        verts[i].x -= centroid.x;
        verts[i].y -= centroid.y;
        verts[i].z -= centroid.z;
        verts[i].w -= centroid.w;
    }

    float max_len_sq = 0.0f;
    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        Vector4 v = verts[i];
        float len_sq = v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;

        if (len_sq > max_len_sq)
        {
            max_len_sq = len_sq;
        }
    }

    if (max_len_sq < 0.0001f)
    {
        return;
    }

    float circumradius = sqrtf(max_len_sq);
    float scale = 1.0f / circumradius;

    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        verts[i].x *= scale;
        verts[i].y *= scale;
        verts[i].z *= scale;
        verts[i].w *= scale;
    }

    // recalculate centroids (TODO volumes)

    // recalculate centroids per layer, dim 1 upward -- each layer's
    // members index LOCALLY into the previous layer's own metas array,
    // no cross-layer offset needed since metas lives per-RefList
    for (uint8_t dim = 1; dim <= pm->dimension; dim++)
    {
        RefList* refs = &pm->layers[dim].sub.refs;

        uint32_t refcount = 0;
        uint32_t* p = refs->data;
        uint32_t* end = refs->data + refs->rle_size;
        while (p < end)
        {
            refcount += 1;
            p += 1 + *p;
        }

        free(refs->metas);
        refs->metas = calloc(refcount, sizeof(RefMetadata));
        refs->refcount = refcount;

        RefList* lower_refs = (dim >= 2) ? &pm->layers[dim - 1].sub.refs : NULL;

        p = refs->data;
        uint32_t meta_index = 0;

        while (p < end)
        {
            uint32_t count = *p;
            uint32_t* members = p + 1;

            Vector4 sum = { 0, 0, 0, 0 };

            for (uint32_t i = 0; i < count; i++)
            {
                Vector4 v = (dim == 1)
                ? verts[members[i]]
                : lower_refs->metas[members[i]].centroid;

                sum.x += v.x;
                sum.y += v.y;
                sum.z += v.z;
                sum.w += v.w;
            }

            refs->metas[meta_index].centroid = (Vector4){
                sum.x / count, sum.y / count, sum.z / count, sum.w / count
            };

            meta_index += 1;
            p += 1 + count;
        }
    }
}

PolytopeMesh pm_new_2d_regular_polygon(uint8_t num_sides)
{
    if (num_sides < 3)
    {
        return pm_new_2d_regular_polygon(3);
    }

    PolytopeMesh pm = pm_new(2);

    pm_reserve(&pm, 0, num_sides);
    pm_reserve(&pm, 1, num_sides * 3);
    pm_reserve(&pm, 2, 1 + num_sides);

    for (uint32_t i = 0; i < num_sides; i++)
    {
        float angle = (2.0f * CONSTANT_PI * i) / num_sides;
        Vector4 vert = {
            .x = cosf(angle),
            .y = sinf(angle),
            .z = 0.0f,
            .w = 0.0f
        };
        pm_set_vertex(&pm, i, vert);
    }

    uint32_t edge_index = 0;
    for (uint32_t i = 0; i < num_sides; i++)
    {
        uint32_t v0 = i;
        uint32_t v1 = (i + 1) % num_sides;

        pm_set_ref(&pm, 1, edge_index++, 2);
        pm_set_ref(&pm, 1, edge_index++, v0);
        pm_set_ref(&pm, 1, edge_index++, v1);
    }

    uint32_t face_index = 0;
    pm_set_ref(&pm, 2, face_index++, num_sides);
    for (uint32_t i = 0; i < num_sides; i++)
    {
        pm_set_ref(&pm, 2, face_index++, i);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_3d_deltahedron(const Vector4* verts, uint32_t vert_count, float edge_len_sq)
{
    uint32_t* edge_index = malloc(sizeof(uint32_t) * vert_count * vert_count);
    for (uint32_t i = 0; i < vert_count * vert_count; i += 1)
    {
        edge_index[i] = UINT32_MAX;
    }

    uint32_t edge_count = 0;
    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        for (uint32_t j = i + 1; j < vert_count; j += 1)
        {
            float dx = verts[i].x - verts[j].x;
            float dy = verts[i].y - verts[j].y;
            float dz = verts[i].z - verts[j].z;
            float dw = verts[i].w - verts[j].w;
            float dist_sq = dx * dx + dy * dy + dz * dz + dw * dw;

            if (fabsf(dist_sq - edge_len_sq) < 0.001f)
            {
                edge_index[i * vert_count + j] = edge_count;
                edge_index[j * vert_count + i] = edge_count;
                edge_count += 1;
            }
        }
    }

    uint32_t face_count = 0;
    for (uint32_t a = 0; a < vert_count; a += 1)
    {
        for (uint32_t b = a + 1; b < vert_count; b += 1)
        {
            for (uint32_t c = b + 1; c < vert_count; c += 1)
            {
                if (edge_index[a * vert_count + b] != UINT32_MAX
                    && edge_index[b * vert_count + c] != UINT32_MAX
                    && edge_index[c * vert_count + a] != UINT32_MAX)
                {
                    face_count += 1;
                }
            }
        }
    }

    PolytopeMesh pm = pm_new(3);

    pm_reserve(&pm, 0, vert_count);
    pm_reserve(&pm, 1, edge_count * 3);
    pm_reserve(&pm, 2, face_count * 4);
    pm_reserve(&pm, 3, face_count + 1);

    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        pm_set_vertex(&pm, i, verts[i]);
    }

    uint32_t index = 0;
    for (uint32_t i = 0; i < vert_count; i += 1)
    {
        for (uint32_t j = i + 1; j < vert_count; j += 1)
        {
            if (edge_index[i * vert_count + j] != UINT32_MAX)
            {
                pm_set_ref(&pm, 1, index++, 2);
                pm_set_ref(&pm, 1, index++, i);
                pm_set_ref(&pm, 1, index++, j);
            }
        }
    }

    index = 0;
    for (uint32_t a = 0; a < vert_count; a += 1)
    {
        for (uint32_t b = a + 1; b < vert_count; b += 1)
        {
            for (uint32_t c = b + 1; c < vert_count; c += 1)
            {
                if (edge_index[a * vert_count + b] != UINT32_MAX
                    && edge_index[b * vert_count + c] != UINT32_MAX
                    && edge_index[c * vert_count + a] != UINT32_MAX)
                {
                    pm_set_ref(&pm, 2, index++, 3);
                    pm_set_ref(&pm, 2, index++, edge_index[a * vert_count + b]);
                    pm_set_ref(&pm, 2, index++, edge_index[b * vert_count + c]);
                    pm_set_ref(&pm, 2, index++, edge_index[c * vert_count + a]);
                }
            }
        }
    }

    index = 0;
    pm_set_ref(&pm, 3, index++, face_count);
    for (uint32_t f = 0; f < face_count; f += 1)
    {
        pm_set_ref(&pm, 3, index++, f);
    }

    free(edge_index);

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_3d_tetrahedron()
{
    PolytopeMesh pm = pm_new(3);

    pm_reserve(&pm, 0, 4);
    pm_reserve(&pm, 1, 6 * 2 + 6);
    pm_reserve(&pm, 2, 4 * 3 + 4);
    pm_reserve(&pm, 3, 1 * 4 + 1);

    pm_set_vertex(&pm, 0, (Vector4){ +1, +1, +1, 0 });
    pm_set_vertex(&pm, 1, (Vector4){ +1, -1, -1, 0 });
    pm_set_vertex(&pm, 2, (Vector4){ -1, +1, -1, 0 });
    pm_set_vertex(&pm, 3, (Vector4){ -1, -1, +1, 0 });

    uint32_t edge_verts[6][2] = {
        {0,1}, {0,2}, {0,3},
        {1,2}, {1,3}, {2,3},
    };

    uint32_t index = 0;
    for (uint32_t i = 0; i < 6; i += 1)
    {
        pm_set_ref(&pm, 1, index++, 2);
        pm_set_ref(&pm, 1, index++, edge_verts[i][0]);
        pm_set_ref(&pm, 1, index++, edge_verts[i][1]);
    }

    uint32_t face_edges[4][3] = {
        {0, 3, 1},
        {0, 4, 2},
        {1, 5, 2},
        {3, 5, 4},
    };

    index = 0;
    for (uint32_t f = 0; f < 4; f += 1)
    {
        pm_set_ref(&pm, 2, index++, 3);
        pm_set_ref(&pm, 2, index++, face_edges[f][0]);
        pm_set_ref(&pm, 2, index++, face_edges[f][1]);
        pm_set_ref(&pm, 2, index++, face_edges[f][2]);
    }

    index = 0;
    pm_set_ref(&pm, 3, index++, 4);
    for (uint32_t f = 0; f < 4; f += 1)
    {
        pm_set_ref(&pm, 3, index++, f);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_3d_cube()
{
    PolytopeMesh pm = pm_new(3);

    pm_reserve(&pm, 0, 8);
    pm_reserve(&pm, 1, 12 * 2 + 12);
    pm_reserve(&pm, 2, 6 * 4 + 6);
    pm_reserve(&pm, 3, 1 * 6 + 1);

    pm_set_vertex(&pm, 0, (Vector4){ +1, +1, +1, 0 });
    pm_set_vertex(&pm, 1, (Vector4){ +1, +1, -1, 0 });
    pm_set_vertex(&pm, 2, (Vector4){ +1, -1, +1, 0 });
    pm_set_vertex(&pm, 3, (Vector4){ +1, -1, -1, 0 });
    pm_set_vertex(&pm, 4, (Vector4){ -1, +1, +1, 0 });
    pm_set_vertex(&pm, 5, (Vector4){ -1, +1, -1, 0 });
    pm_set_vertex(&pm, 6, (Vector4){ -1, -1, +1, 0 });
    pm_set_vertex(&pm, 7, (Vector4){ -1, -1, -1, 0 });

    uint32_t edge_verts[12][2] = {
        {0,1}, {2,3}, {4,5}, {6,7},
        {0,2}, {1,3}, {4,6}, {5,7},
        {0,4}, {1,5}, {2,6}, {3,7},
    };

    uint32_t index = 0;
    for (uint32_t i = 0; i < 12; i += 1)
    {
        pm_set_ref(&pm, 1, index++, 2);
        pm_set_ref(&pm, 1, index++, edge_verts[i][0]);
        pm_set_ref(&pm, 1, index++, edge_verts[i][1]);
    }

    uint32_t face_edges[6][4] = {
        {0, 5, 1, 4},
        {2, 7, 3, 6},
        {8, 9, 5, 4},
        {10, 11, 7, 6},
        {0, 9, 2, 8},
        {1, 11, 3, 10},
    };

    index = 0;
    for (uint32_t f = 0; f < 6; f += 1)
    {
        pm_set_ref(&pm, 2, index++, 4);
        pm_set_ref(&pm, 2, index++, face_edges[f][0]);
        pm_set_ref(&pm, 2, index++, face_edges[f][1]);
        pm_set_ref(&pm, 2, index++, face_edges[f][2]);
        pm_set_ref(&pm, 2, index++, face_edges[f][3]);
    }

    index = 0;
    pm_set_ref(&pm, 3, index++, 6);
    for (uint32_t f = 0; f < 6; f += 1)
    {
        pm_set_ref(&pm, 3, index++, f);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_3d_octahedron()
{
    Vector4 verts[6] = {
        { +1, 0, 0, 0 }, { -1, 0, 0, 0 },
        { 0, +1, 0, 0 }, { 0, -1, 0, 0 },
        { 0, 0, +1, 0 }, { 0, 0, -1, 0 },
    };

    return pm_new_3d_deltahedron(verts, 6, 2.0f);
}

PolytopeMesh pm_new_3d_icosahedron()
{
    float phi = 1.61803398874989484820f;

    Vector4 verts[12] = {
        { 0, +1, +phi, 0 }, { 0, +1, -phi, 0 },
        { 0, -1, +phi, 0 }, { 0, -1, -phi, 0 },
        { +1, +phi, 0, 0 }, { +1, -phi, 0, 0 },
        { -1, +phi, 0, 0 }, { -1, -phi, 0, 0 },
        { +phi, 0, +1, 0 }, { +phi, 0, -1, 0 },
        { -phi, 0, +1, 0 }, { -phi, 0, -1, 0 },
    };

    return pm_new_3d_deltahedron(verts, 12, 4.0f);
}

PolytopeMesh pm_new_4d_5cell()
{
    PolytopeMesh pm = pm_new(4);

    float w1 = -0.44721359549995793928f;
    float w2 = 1.78885438199983175712f;

    pm_reserve(&pm, 0, 5);
    pm_reserve(&pm, 1, 10 * 3);
    pm_reserve(&pm, 2, 10 * 4);
    pm_reserve(&pm, 3, 5 * 5);
    pm_reserve(&pm, 4, 1 + 5);

    pm_set_vertex(&pm, 0, (Vector4){ +1, +1, +1, w1 });
    pm_set_vertex(&pm, 1, (Vector4){ +1, -1, -1, w1 });
    pm_set_vertex(&pm, 2, (Vector4){ -1, +1, -1, w1 });
    pm_set_vertex(&pm, 3, (Vector4){ -1, -1, +1, w1 });
    pm_set_vertex(&pm, 4, (Vector4){ 0, 0, 0, w2 });

    uint32_t edge_index[5][5];
    uint32_t index = 0;
    uint32_t edge_count = 0;
    for (uint32_t a = 0; a < 5; a += 1)
    {
        for (uint32_t b = a + 1; b < 5; b += 1)
        {
            edge_index[a][b] = edge_count;
            edge_index[b][a] = edge_count;
            edge_count += 1;

            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, a);
            pm_set_ref(&pm, 1, index++, b);
        }
    }

    uint32_t face_verts[10][3];
    uint32_t face_count = 0;
    index = 0;
    for (uint32_t a = 0; a < 5; a += 1)
    {
        for (uint32_t b = a + 1; b < 5; b += 1)
        {
            for (uint32_t c = b + 1; c < 5; c += 1)
            {
                face_verts[face_count][0] = a;
                face_verts[face_count][1] = b;
                face_verts[face_count][2] = c;
                face_count += 1;

                pm_set_ref(&pm, 2, index++, 3);
                pm_set_ref(&pm, 2, index++, edge_index[a][b]);
                pm_set_ref(&pm, 2, index++, edge_index[b][c]);
                pm_set_ref(&pm, 2, index++, edge_index[c][a]);
            }
        }
    }

    index = 0;
    for (uint32_t omitted = 0; omitted < 5; omitted += 1)
    {
        pm_set_ref(&pm, 3, index++, 4);
        for (uint32_t f = 0; f < 10; f += 1)
        {
            if (face_verts[f][0] != omitted && face_verts[f][1] != omitted && face_verts[f][2] != omitted)
            {
                pm_set_ref(&pm, 3, index++, f);
            }
        }
    }

    index = 0;
    pm_set_ref(&pm, 4, index++, 5);
    for (uint32_t c = 0; c < 5; c += 1)
    {
        pm_set_ref(&pm, 4, index++, c);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_4d_8cell()
{
    PolytopeMesh pm = pm_new(4);

    pm_reserve(&pm, 0, 16);
    pm_reserve(&pm, 1, 32 * 3);
    pm_reserve(&pm, 2, 24 * 5);
    pm_reserve(&pm, 3, 8 * 7);
    pm_reserve(&pm, 4, 1 + 8);

    for (uint32_t i = 0; i < 16; i += 1)
    {
        pm_set_vertex(&pm, i, (Vector4){
            (i & 1) ? +1.0f : -1.0f,
                      (i & 2) ? +1.0f : -1.0f,
                      (i & 4) ? +1.0f : -1.0f,
                      (i & 8) ? +1.0f : -1.0f
        });
    }

    uint32_t edge_index[16][4];
    uint32_t index = 0;
    uint32_t edge_count = 0;
    for (uint32_t v = 0; v < 16; v += 1)
    {
        for (uint32_t axis = 0; axis < 4; axis += 1)
        {
            if (!(v & (1u << axis)))
            {
                edge_index[v][axis] = edge_count;
                edge_count += 1;

                pm_set_ref(&pm, 1, index++, 2);
                pm_set_ref(&pm, 1, index++, v);
                pm_set_ref(&pm, 1, index++, v | (1u << axis));
            }
        }
    }

    uint32_t face_base[24];
    uint32_t face_axis_a[24];
    uint32_t face_axis_b[24];
    uint32_t face_count = 0;
    index = 0;
    for (uint32_t a = 0; a < 4; a += 1)
    {
        for (uint32_t b = a + 1; b < 4; b += 1)
        {
            for (uint32_t v = 0; v < 16; v += 1)
            {
                if (!(v & (1u << a)) && !(v & (1u << b)))
                {
                    uint32_t va = v | (1u << a);
                    uint32_t vb = v | (1u << b);

                    face_base[face_count] = v;
                    face_axis_a[face_count] = a;
                    face_axis_b[face_count] = b;
                    face_count += 1;

                    pm_set_ref(&pm, 2, index++, 4);
                    pm_set_ref(&pm, 2, index++, edge_index[v][a]);
                    pm_set_ref(&pm, 2, index++, edge_index[va][b]);
                    pm_set_ref(&pm, 2, index++, edge_index[vb][a]);
                    pm_set_ref(&pm, 2, index++, edge_index[v][b]);
                }
            }
        }
    }

    index = 0;
    for (uint32_t axis = 0; axis < 4; axis += 1)
    {
        for (uint32_t side = 0; side < 2; side += 1)
        {
            pm_set_ref(&pm, 3, index++, 6);
            for (uint32_t f = 0; f < 24; f += 1)
            {
                if (face_axis_a[f] != axis && face_axis_b[f] != axis && ((face_base[f] >> axis) & 1u) == side)
                {
                    pm_set_ref(&pm, 3, index++, f);
                }
            }
        }
    }

    index = 0;
    pm_set_ref(&pm, 4, index++, 8);
    for (uint32_t c = 0; c < 8; c += 1)
    {
        pm_set_ref(&pm, 4, index++, c);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_4d_16cell()
{
    PolytopeMesh pm = pm_new(4);

    pm_reserve(&pm, 0, 8);
    pm_reserve(&pm, 1, 24 * 3);
    pm_reserve(&pm, 2, 32 * 4);
    pm_reserve(&pm, 3, 16 * 5);
    pm_reserve(&pm, 4, 1 + 16);

    Vector4 verts[8] = {
        { +1, 0, 0, 0 }, { -1, 0, 0, 0 },
        { 0, +1, 0, 0 }, { 0, -1, 0, 0 },
        { 0, 0, +1, 0 }, { 0, 0, -1, 0 },
        { 0, 0, 0, +1 }, { 0, 0, 0, -1 }
    };
    for (uint32_t i = 0; i < 8; i++)
    {
        pm_set_vertex(&pm, i, verts[i]);
    }

    uint32_t edge_lookup[8][8];
    for (uint32_t i = 0; i < 8; i++)
        for (uint32_t j = 0; j < 8; j++)
            edge_lookup[i][j] = UINT32_MAX;

    uint32_t index = 0;
    uint32_t edge_count = 0;
    for (uint32_t a = 0; a < 8; a++)
    {
        for (uint32_t b = a + 1; b < 8; b++)
        {
            if ((a ^ b) != 1)
            {
                edge_lookup[a][b] = edge_count;
                edge_lookup[b][a] = edge_count;
                edge_count++;

                pm_set_ref(&pm, 1, index++, 2);
                pm_set_ref(&pm, 1, index++, a);
                pm_set_ref(&pm, 1, index++, b);
            }
        }
    }

    uint32_t face_verts[32][3];
    uint32_t face_count = 0;
    index = 0;
    for (uint32_t a = 0; a < 8; a++)
    {
        for (uint32_t b = a + 1; b < 8; b++)
        {
            if ((a ^ b) == 1) continue;
            for (uint32_t c = b + 1; c < 8; c++)
            {
                if ((a ^ c) == 1 || (b ^ c) == 1) continue;

                face_verts[face_count][0] = a;
                face_verts[face_count][1] = b;
                face_verts[face_count][2] = c;
                face_count++;

                pm_set_ref(&pm, 2, index++, 3);
                pm_set_ref(&pm, 2, index++, edge_lookup[a][b]);
                pm_set_ref(&pm, 2, index++, edge_lookup[b][c]);
                pm_set_ref(&pm, 2, index++, edge_lookup[c][a]);
            }
        }
    }

    index = 0;
    for (uint32_t c = 0; c < 16; c++)
    {
        uint32_t v[4] = {
            (c & 1) ? 0 : 1,
            (c & 2) ? 2 : 3,
            (c & 4) ? 4 : 5,
            (c & 8) ? 6 : 7
        };

        pm_set_ref(&pm, 3, index++, 4);
        for (uint32_t f = 0; f < 32; f++)
        {
            uint32_t matches = 0;
            for (uint32_t k = 0; k < 3; k++)
            {
                if (face_verts[f][k] == v[0] || face_verts[f][k] == v[1] ||
                    face_verts[f][k] == v[2] || face_verts[f][k] == v[3])
                {
                    matches++;
                }
            }
            if (matches == 3)
            {
                pm_set_ref(&pm, 3, index++, f);
            }
        }
    }

    index = 0;
    pm_set_ref(&pm, 4, index++, 16);
    for (uint32_t c = 0; c < 16; c++)
    {
        pm_set_ref(&pm, 4, index++, c);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_4d_24cell()
{
    Vector4 verts[24];
    uint32_t v_idx = 0;
    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t j = i + 1; j < 4; j++)
        {
            for (uint32_t s1 = 0; s1 < 2; s1++)
            {
                for (uint32_t s2 = 0; s2 < 2; s2++)
                {
                    Vector4 v = { 0, 0, 0, 0 };
                    float* comp = (float*)&v;
                    comp[i] = (s1 == 0) ? 1.0f : -1.0f;
                    comp[j] = (s2 == 0) ? 1.0f : -1.0f;
                    verts[v_idx++] = v;
                }
            }
        }
    }

    PolytopeMesh pm = pm_new_3d_deltahedron(verts, 24, 2.0f);
    pm.dimension = 4;

    uint32_t cell_count = 0;
    uint32_t cell_faces[24][8];

    for (uint32_t a = 0; a < 24; a++)
    {
        for (uint32_t b = a + 1; b < 24; b++)
        {
            for (uint32_t c = b + 1; c < 24; c++)
            {
                for (uint32_t d = c + 1; d < 24; d++)
                {
                    for (uint32_t e = d + 1; e < 24; e++)
                    {
                        for (uint32_t f = e + 1; f < 24; f++)
                        {
                            uint32_t oct[6] = { a, b, c, d, e, f };
                            float center_x = 0, center_y = 0, center_z = 0, center_w = 0;
                            for (int k = 0; k < 6; k++)
                            {
                                center_x += verts[oct[k]].x;
                                center_y += verts[oct[k]].y;
                                center_z += verts[oct[k]].z;
                                center_w += verts[oct[k]].w;
                            }

                            float center_sq = center_x*center_x + center_y*center_y + center_z*center_z + center_w*center_w;
                            if (fabsf(center_sq - 36.0f) < 0.01f)
                            {
                                uint32_t faces_found = 0;
                                cell_count++;
                            }
                        }
                    }
                }
            }
        }
    }

    pm_reserve(&pm, 3, 24 * 9);
    pm_reserve(&pm, 4, 1 + 24);

    uint32_t ref_index = 0;
    for (uint32_t cell = 0; cell < 24; cell++)
    {
        pm_set_ref(&pm, 3, ref_index++, 8);
        for (uint32_t f = 0; f < 8; f++)
        {
            pm_set_ref(&pm, 3, ref_index++, cell_faces[cell][f]);
        }
    }

    ref_index = 0;
    pm_set_ref(&pm, 4, ref_index++, 24);
    for (uint32_t c = 0; c < 24; c++)
    {
        pm_set_ref(&pm, 4, ref_index++, c);
    }

    pm_normalize(&pm);
    return pm;
}

PolytopeMesh pm_new_prism(const PolytopeMesh* base)
{
    uint32_t vert_count = base->layers[0].sub.verts.size;
    Vector4* base_verts = base->layers[0].sub.verts.data;

    if (base->dimension == 2)
    {
        RefList* base_edges = &base->layers[1].sub.refs;
        RefList* base_faces = &base->layers[2].sub.refs;
        uint32_t edge_count = base_edges->refcount;

        PolytopeMesh pm = pm_new(3);

        pm_reserve(&pm, 0, vert_count * 2);
        for (uint32_t i = 0; i < vert_count; i += 1)
        {
            Vector4 v = base_verts[i];
            pm_set_vertex(&pm, i, (Vector4){ v.x, v.y, -1, v.w });
            pm_set_vertex(&pm, vert_count + i, (Vector4){ v.x, v.y, 1, v.w });
        }

        pm_reserve(&pm, 1, (edge_count * 2 + vert_count) * 3);
        uint32_t index = 0;

        uint32_t* p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, v0);
            pm_set_ref(&pm, 1, index++, v1);
            p += 1 + count;
        }
        p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, vert_count + v0);
            pm_set_ref(&pm, 1, index++, vert_count + v1);
            p += 1 + count;
        }
        for (uint32_t i = 0; i < vert_count; i += 1)
        {
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, i);
            pm_set_ref(&pm, 1, index++, vert_count + i);
        }

        const uint32_t* base_face_ptr = rl_get_ref(base_faces, 0);
        uint32_t base_face_edge_count = base_face_ptr[0];

        uint32_t face_slots = (base_face_edge_count) + (base_face_edge_count) + (edge_count * 5);
        pm_reserve(&pm, 2, face_slots);
        index = 0;

        pm_set_ref(&pm, 2, index++, base_face_edge_count);
        for (uint32_t i = 0; i < base_face_edge_count; i += 1)
        {
            pm_set_ref(&pm, 2, index++, base_face_ptr[1 + i]);
        }

        pm_set_ref(&pm, 2, index++, base_face_edge_count);
        for (uint32_t i = 0; i < base_face_edge_count; i += 1)
        {
            pm_set_ref(&pm, 2, index++, edge_count + base_face_ptr[1 + i]);
        }

        p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];

            pm_set_ref(&pm, 2, index++, 4);
            pm_set_ref(&pm, 2, index++, e);
            pm_set_ref(&pm, 2, index++, 2 * edge_count + v1);
            pm_set_ref(&pm, 2, index++, edge_count + e);
            pm_set_ref(&pm, 2, index++, 2 * edge_count + v0);

            p += 1 + count;
        }

        uint32_t total_faces = 2 + edge_count;
        pm_reserve(&pm, 3, total_faces + 1);
        index = 0;
        pm_set_ref(&pm, 3, index++, total_faces);
        for (uint32_t f = 0; f < total_faces; f += 1)
        {
            pm_set_ref(&pm, 3, index++, f);
        }

        pm_normalize(&pm);
        return pm;
    }
    else if (base->dimension == 3)
    {
        RefList* base_edges = &base->layers[1].sub.refs;
        RefList* base_faces = &base->layers[2].sub.refs;
        uint32_t edge_count = base_edges->refcount;
        uint32_t face_count = base_faces->refcount;

        PolytopeMesh pm = pm_new(4);

        pm_reserve(&pm, 0, vert_count * 2);
        for (uint32_t i = 0; i < vert_count; i += 1)
        {
            Vector4 v = base_verts[i];
            pm_set_vertex(&pm, i, (Vector4){ v.x, v.y, v.z, -1 });
            pm_set_vertex(&pm, vert_count + i, (Vector4){ v.x, v.y, v.z, 1 });
        }

        pm_reserve(&pm, 1, (edge_count * 2 + vert_count) * 3);
        uint32_t index = 0;

        uint32_t* p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, v0);
            pm_set_ref(&pm, 1, index++, v1);
            p += 1 + count;
        }
        p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, vert_count + v0);
            pm_set_ref(&pm, 1, index++, vert_count + v1);
            p += 1 + count;
        }
        for (uint32_t i = 0; i < vert_count; i += 1)
        {
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, i);
            pm_set_ref(&pm, 1, index++, vert_count + i);
        }

        uint32_t face_slots = 0;
        p = base_faces->data;
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            uint32_t count = *p;
            face_slots += 1 + count;
            p += 1 + count;
        }
        face_slots = face_slots * 2 + edge_count * 5;

        pm_reserve(&pm, 2, face_slots);
        index = 0;

        p = base_faces->data;
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            uint32_t count = *p;
            uint32_t* members = p + 1;
            pm_set_ref(&pm, 2, index++, count);
            for (uint32_t i = 0; i < count; i += 1)
            {
                pm_set_ref(&pm, 2, index++, members[i]);
            }
            p += 1 + count;
        }
        p = base_faces->data;
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            uint32_t count = *p;
            uint32_t* members = p + 1;
            pm_set_ref(&pm, 2, index++, count);
            for (uint32_t i = 0; i < count; i += 1)
            {
                pm_set_ref(&pm, 2, index++, edge_count + members[i]);
            }
            p += 1 + count;
        }

        p = base_edges->data;
        for (uint32_t e = 0; e < edge_count; e += 1)
        {
            uint32_t count = *p;
            uint32_t v0 = p[1];
            uint32_t v1 = p[2];

            pm_set_ref(&pm, 2, index++, 4);
            pm_set_ref(&pm, 2, index++, e);
            pm_set_ref(&pm, 2, index++, 2 * edge_count + v1);
            pm_set_ref(&pm, 2, index++, edge_count + e);
            pm_set_ref(&pm, 2, index++, 2 * edge_count + v0);

            p += 1 + count;
        }

        uint32_t cell_slots = 2 * (1 + face_count);

        p = base_faces->data;
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            uint32_t count = *p;
            cell_slots += 3 + count;
            p += 1 + count;
        }

        pm_reserve(&pm, 3, cell_slots);
        index = 0;

        pm_set_ref(&pm, 3, index++, face_count);
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            pm_set_ref(&pm, 3, index++, f);
        }

        pm_set_ref(&pm, 3, index++, face_count);
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            pm_set_ref(&pm, 3, index++, face_count + f);
        }

        p = base_faces->data;
        for (uint32_t f = 0; f < face_count; f += 1)
        {
            uint32_t count = *p;
            uint32_t* members = p + 1;

            pm_set_ref(&pm, 3, index++, 2 + count);
            pm_set_ref(&pm, 3, index++, f);
            pm_set_ref(&pm, 3, index++, face_count + f);
            for (uint32_t i = 0; i < count; i += 1)
            {
                uint32_t base_edge = members[i];
                pm_set_ref(&pm, 3, index++, 2 * face_count + base_edge);
            }

            p += 1 + count;
        }

        uint32_t total_cells = 2 + face_count;
        pm_reserve(&pm, 4, total_cells + 1);
        index = 0;
        pm_set_ref(&pm, 4, index++, total_cells);
        for (uint32_t c = 0; c < total_cells; c += 1)
        {
            pm_set_ref(&pm, 4, index++, c);
        }

        pm_normalize(&pm);
        return pm;
    }

    // REALLY BAD, so for rn do not pass 1d or 4d shapes into here
    // or you risk having a double-free segfault.
    return *base;
}

PolytopeMesh pm_new_duoprism(uint32_t p, uint32_t q)
{
    PolytopeMesh pm = pm_new(4);

    uint32_t vert_count = p * q;
    uint32_t edge_count = 2 * p * q;
    uint32_t face_slots = p + q + 7 * p * q;
    uint32_t cell_slots = 3 * p + 3 * q + 2 * p * q;
    uint32_t hull_slots = 1 + (p + q);

    pm_reserve(&pm, 0, vert_count);
    pm_reserve(&pm, 1, edge_count * 3);
    pm_reserve(&pm, 2, face_slots);
    pm_reserve(&pm, 3, cell_slots);
    pm_reserve(&pm, 4, hull_slots);

    for (uint32_t i = 0; i < p; i += 1)
    {
        for (uint32_t j = 0; j < q; j += 1)
        {
            Vector4 v;
            v.x = cosf(2.0f * PI * (float)i / (float)p);
            v.y = sinf(2.0f * PI * (float)i / (float)p);
            v.z = cosf(2.0f * PI * (float)j / (float)q);
            v.w = sinf(2.0f * PI * (float)j / (float)q);
            pm_set_vertex(&pm, i * q + j, v);
        }
    }

    uint32_t index = 0;
    for (uint32_t i = 0; i < p; i += 1)
    {
        for (uint32_t j = 0; j < q; j += 1)
        {
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, i * q + j);
            pm_set_ref(&pm, 1, index++, ((i + 1) % p) * q + j);
        }
    }
    for (uint32_t i = 0; i < p; i += 1)
    {
        for (uint32_t j = 0; j < q; j += 1)
        {
            pm_set_ref(&pm, 1, index++, 2);
            pm_set_ref(&pm, 1, index++, i * q + j);
            pm_set_ref(&pm, 1, index++, i * q + (j + 1) % q);
        }
    }

    index = 0;
    for (uint32_t j = 0; j < q; j += 1)
    {
        pm_set_ref(&pm, 2, index++, p);
        for (uint32_t i = 0; i < p; i += 1)
        {
            pm_set_ref(&pm, 2, index++, i * q + j);
        }
    }
    for (uint32_t i = 0; i < p; i += 1)
    {
        pm_set_ref(&pm, 2, index++, q);
        for (uint32_t j = 0; j < q; j += 1)
        {
            pm_set_ref(&pm, 2, index++, p * q + i * q + j);
        }
    }
    for (uint32_t i = 0; i < p; i += 1)
    {
        for (uint32_t j = 0; j < q; j += 1)
        {
            pm_set_ref(&pm, 2, index++, 4);
            pm_set_ref(&pm, 2, index++, i * q + j);
            pm_set_ref(&pm, 2, index++, p * q + ((i + 1) % p) * q + j);
            pm_set_ref(&pm, 2, index++, i * q + (j + 1) % q);
            pm_set_ref(&pm, 2, index++, p * q + i * q + j);
        }
    }

    index = 0;
    for (uint32_t i = 0; i < p; i += 1)
    {
        pm_set_ref(&pm, 3, index++, 2 + q);
        pm_set_ref(&pm, 3, index++, q + i);
        pm_set_ref(&pm, 3, index++, q + (i + 1) % p);
        for (uint32_t j = 0; j < q; j += 1)
        {
            pm_set_ref(&pm, 3, index++, q + p + i * q + j);
        }
    }
    for (uint32_t j = 0; j < q; j += 1)
    {
        pm_set_ref(&pm, 3, index++, 2 + p);
        pm_set_ref(&pm, 3, index++, j);
        pm_set_ref(&pm, 3, index++, (j + 1) % q);
        for (uint32_t i = 0; i < p; i += 1)
        {
            pm_set_ref(&pm, 3, index++, q + p + i * q + j);
        }
    }

    index = 0;
    pm_set_ref(&pm, 4, index++, p + q);
    for (uint32_t c = 0; c < p + q; c += 1)
    {
        pm_set_ref(&pm, 4, index++, c);
    }

    pm_normalize(&pm);
    return pm;
}
