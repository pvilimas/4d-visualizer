#include "draw.h"
#include "visualizer.h"

// rendering

void draw_line4(Vector4 p0, Vector4 p1)
{
    bool color_on = APP->settings.w_color_fade_enabled;
    bool alpha_on = APP->settings.w_alpha_fade_enabled;
    bool thick_on = APP->settings.w_thick_fade_enabled;
    Vector3 cam_pos = APP->view.camera.position;
    Color flat_color = APP->settings.w_color_near;

    // number of segments for the line
    int n = 1;
    if (color_on)
    {
        n = APP->settings.w_color_num_segments;
    }

    if (alpha_on && APP->settings.w_alpha_num_segments > n)
    {
        n = APP->settings.w_alpha_num_segments;
    }

    if (thick_on && APP->settings.w_thick_num_segments > n)
    {
        n = APP->settings.w_thick_num_segments;
    }

    float first_width = APP->settings.w_thick_near;
    Color first_color = flat_color;
    float last_width = APP->settings.w_thick_near;
    Color last_color = flat_color;

    if (!color_on && !alpha_on && !thick_on)
    {
        draw_line_segment4(p0, p1, APP->settings.w_thick_near, APP->settings.w_thick_near, cam_pos, flat_color);
    }
    else
    {
        for (int i = 0; i < n; i += 1)
        {
            float t0 = (float)i / (float)n;
            float t1 = (float)(i + 1) / (float)n;
            float tmid = (t0 + t1) * 0.5f;

            Vector4 seg_start = {
                p0.x + (p1.x - p0.x) * t0,
                p0.y + (p1.y - p0.y) * t0,
                p0.z + (p1.z - p0.z) * t0,
                p0.w + (p1.w - p0.w) * t0,
            };
            Vector4 seg_end = {
                p0.x + (p1.x - p0.x) * t1,
                p0.y + (p1.y - p0.y) * t1,
                p0.z + (p1.z - p0.z) * t1,
                p0.w + (p1.w - p0.w) * t1,
            };
            float w_mid = p0.w + (p1.w - p0.w) * tmid;

            float width0 = thick_on ? w_to_thickness(seg_start.w) : APP->settings.w_thick_near;
            float width1 = thick_on ? w_to_thickness(seg_end.w)   : APP->settings.w_thick_near;

            Color seg_color = color_on ? w_to_color(w_mid) : flat_color;
            if (alpha_on)
            {
                seg_color = Fade(seg_color, w_to_alpha(w_mid));
            }

            draw_line_segment4(seg_start, seg_end, width0, width1, cam_pos, seg_color);

            if (i == 0)
            {
                first_width = width0;
                first_color = seg_color;
            }
            if (i == n - 1)
            {
                last_width = width1;
                last_color = seg_color;
            }
        }
    }

    Vector3 proj0 = projection4(p0);
    Vector3 proj1 = projection4(p1);
    DrawSphere(proj0, first_width * 0.5f, first_color);
    DrawSphere(proj1, last_width * 0.5f, last_color);
}

void draw_line_segment3(Vector3 p0, Vector3 p1,
                        float width0, float width1,
                        Vector3 cam_pos,
                        Color color)
{
    draw_open_cylinder(p0, p1, width0 * 0.5f, width1 * 0.5f, MESH_CYLINDER_SEGMENTS, color);
}

void draw_line_segment4(Vector4 p0, Vector4 p1,
                        float width0, float width1,
                        Vector3 cam_pos,
                        Color color)
{
    Vector3 proj0 = projection4(p0);
    Vector3 proj1 = projection4(p1);
    draw_open_cylinder(proj0, proj1, width0 * 0.5f, width1 * 0.5f, MESH_CYLINDER_SEGMENTS, color);
}

void draw_open_cylinder(Vector3 start, Vector3 end, float radius_start, float radius_end, int sides, Color color)
{
    Vector3 dir = Vector3Normalize(Vector3Subtract(end, start));
    Vector3 up = (Vector3){0, 1, 0};
    if (fabsf(Vector3DotProduct(dir, up)) > 0.99f)
    {
        up = (Vector3){1, 0, 0};
    }
    Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, up));
    Vector3 forward = Vector3CrossProduct(right, dir);

    for (int i = 0; i < sides; i += 1)
    {
        float a0 = (2.0f * PI * i) / sides;
        float a1 = (2.0f * PI * (i + 1)) / sides;

        Vector3 offset0 = Vector3Add(Vector3Scale(right, cosf(a0)), Vector3Scale(forward, sinf(a0)));
        Vector3 offset1 = Vector3Add(Vector3Scale(right, cosf(a1)), Vector3Scale(forward, sinf(a1)));

        Vector3 p0_start = Vector3Add(start, Vector3Scale(offset0, radius_start));
        Vector3 p1_start = Vector3Add(start, Vector3Scale(offset1, radius_start));
        Vector3 p0_end = Vector3Add(end, Vector3Scale(offset0, radius_end));
        Vector3 p1_end = Vector3Add(end, Vector3Scale(offset1, radius_end));

        DrawTriangle3D(p0_start, p1_start, p1_end, color);
        DrawTriangle3D(p0_start, p1_end, p0_end, color);
    }
}

void draw_mesh(const PolytopeMesh* mesh, const Vector4* rotated)
{
    const RefList* edges = &mesh->layers[1].sub.refs;
    uint32_t pos = 0;

    while (pos < edges->rle_size)
    {
        uint32_t* entry = &edges->data[pos];

        uint32_t i0 = entry[1];
        uint32_t i1 = entry[2];
        draw_line4(rotated[i0], rotated[i1]);

        pos += 3; // [length=2][v_start][v_end]
    }
}

// Static index buffer so we allocate zero heap memory at runtime
// void draw_mesh_w_depth_cues(const Mesh4D* mesh, const Vector4* rotated)
// {
//     static int sorted_edges[720];
//     static float edge_depths[720];
//
//     if (!mesh || !rotated || mesh->edge_count <= 0) return;
//
//     int count = mesh->edge_count;
//
//     // 1. Compute mid-W for each edge
//     for (int e = 0; e < count; e++) {
//         sorted_edges[e] = e;
//         int i0 = mesh->edges[e][0];
//         int i1 = mesh->edges[e][1];
//         edge_depths[e] = (rotated[i0].w + rotated[i1].w) * 0.5f;
//     }
//
//     // 2. Simple Insertion Sort (fastest for small arrays / mostly-sorted frame data)
//     for (int i = 1; i < count; i++) {
//         int key_edge = sorted_edges[i];
//         float key_depth = edge_depths[key_edge];
//         int j = i - 1;
//
//         // Sort ascending: smallest W (furthest away) drawn FIRST
//         while (j >= 0 && edge_depths[sorted_edges[j]] > key_depth) {
//             sorted_edges[j + 1] = sorted_edges[j];
//             j--;
//         }
//         sorted_edges[j + 1] = key_edge;
//     }
//
//     // 3. Draw sorted edges back-to-front via your draw_line4
//     for (int e = 0; e < count; e++) {
//         int idx = sorted_edges[e];
//         int i0 = mesh->edges[idx][0];
//         int i1 = mesh->edges[idx][1];
//         draw_line4(rotated[i0], rotated[i1]);
//     }
// }
