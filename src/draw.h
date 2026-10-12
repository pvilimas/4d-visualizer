#ifndef DRAW_H
#define DRAW_H

#include "common.h"
#include "utils.h"
#include "mesh.h"

// rendering

// draw an entire line
void draw_line4(Vector4 p0, Vector4 p1);

// draw an open cylinder for a segment, width0 at p0, width1 at p1
void draw_line_segment3(Vector3 p0, Vector3 p1,
                        float width0, float width1,
                        Vector3 cam_pos,
                        Color color);

// draw an open cylinder for a segment, width0 at p0, width1 at p1
void draw_line_segment4(Vector4 p0, Vector4 p1,
                        float width0, float width1,
                        Vector3 cam_pos,
                        Color color);

// draw a cylinder without circular endcaps
void draw_open_cylinder(Vector3 start, Vector3 end, float radius_start, float radius_end, int sides, Color color);

void draw_mesh(const PolytopeMesh* mesh, const Vector4* rotated);

// void draw_mesh_w_depth_cues(const PolytopeMesh* mesh, const Vector4* rotated);

#endif // DRAW_H
