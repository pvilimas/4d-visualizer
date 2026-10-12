#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "axes.h"

// --------------------------------- MESH ---------------------------------- //

// polytope face lattice (combinatorial structure)

typedef Vector4 Vertex;

typedef struct {
    Vertex* data;
    uint32_t size;
} VertexList;

VertexList vl_new(uint32_t size);
void vl_free(VertexList* vl);

typedef struct {
    Vector4 centroid;
    float volume;
} RefMetadata;

typedef struct {
    uint32_t* data;         // must account for run-length encoding on allocation
    uint32_t rle_size;      // full size of the run-length encoding of the references
    RefMetadata* metas;     // metadata for each reference
    uint32_t refcount;      // total number of references = number of metas
} RefList;

RefList rl_new(uint32_t size);
void rl_free(RefList* rl);
const uint32_t* rl_get_ref(const RefList* rl, uint32_t index);

typedef struct MeshLayer {
    uint8_t dimension;
    union {
        VertexList verts;
        RefList refs;
    } sub;
} MeshLayer;

typedef struct {
    MeshLayer* layers;
    uint8_t dimension;
} PolytopeMesh;

PolytopeMesh pm_new(uint8_t dimension);
void pm_free(PolytopeMesh* pm);
void pm_reserve(PolytopeMesh* pm, uint8_t dimension, uint32_t num_elements);
void pm_set_vertex(PolytopeMesh* pm, uint32_t index, Vector4 vertex);
void pm_set_ref(PolytopeMesh* pm, uint8_t dimension, uint32_t index, uint32_t ref);
// divide all vertex magnitudes by the circumradius to make it exactly 1
// center it on the origin
// recalculate centroids and (TODO) volumes
void pm_normalize(PolytopeMesh* pm);

// preset polytopes

// 2d shapes - z=w=0
PolytopeMesh pm_new_2d_regular_polygon(uint8_t num_sides);
// 3d shapes - w=0
PolytopeMesh pm_new_3d_deltahedron(const Vector4* verts, uint32_t vert_count, float edge_len_sq);
PolytopeMesh pm_new_3d_tetrahedron();
PolytopeMesh pm_new_3d_cube();
PolytopeMesh pm_new_3d_octahedron();
PolytopeMesh pm_new_3d_icosahedron();
// 4d shapes
PolytopeMesh pm_new_4d_5cell();
PolytopeMesh pm_new_4d_8cell();
PolytopeMesh pm_new_4d_16cell();
PolytopeMesh pm_new_4d_24cell();

// general operations
PolytopeMesh pm_new_prism_from_2d(const PolytopeMesh* base);
PolytopeMesh pm_new_prism_from_3d(const PolytopeMesh* base);
PolytopeMesh pm_new_duoprism(uint32_t p, uint32_t q);

// -------------------------------- MODELS --------------------------------- //

typedef struct {
    ModelType type;
    Axes4D axes;
    PolytopeMesh mesh;
} Model4D;

#endif // MESH_H
