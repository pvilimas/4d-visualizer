#include "utils.h"
#include "visualizer.h"

// --------------------------------- UTILS --------------------------------- //

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

// projection

Vector3 projection4(Vector4 v)
{
    Vector3 result = { 0, 0, 0 };
    result = Vector3Add(result, Vector3Scale(APP->view.axes.x, v.x));
    result = Vector3Add(result, Vector3Scale(APP->view.axes.y, v.y));
    result = Vector3Add(result, Vector3Scale(APP->view.axes.z, v.z));
    result = Vector3Add(result, Vector3Scale(APP->view.axes.w, v.w));
    return result;
}

Vector4 rotate_vector_in_plane(Vector4 v, float angle, int a, int b) {
    float c = cosf(angle);
    float s = sinf(angle);
    float *coords = (float *)&v;
    float va = coords[a];
    float vb = coords[b];
    coords[a] = va * c - vb * s;
    coords[b] = va * s + vb * c;
    return v;
}

float w_fade_factor(float w, float near, float far)
{
    float dist = fabsf(w);
    float t = (far - dist) / (far - near);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t;
}

float w_to_alpha(float w)
{
    return w_fade_factor(w, APP->settings.w_alpha_dist_near, APP->settings.w_alpha_dist_far);
}

float w_to_thickness(float w)
{
    float t = w_fade_factor(w, APP->settings.w_thick_dist_near, APP->settings.w_thick_dist_far);
    return APP->settings.w_thick_far + (APP->settings.w_thick_near - APP->settings.w_thick_far) * t;
}

Color w_to_color(float w)
{
    float t = w_fade_factor(w, APP->settings.w_color_dist_near, APP->settings.w_color_dist_far);
    return ColorLerp(APP->settings.w_color_far, APP->settings.w_color_near, t);
}

void toggle_rotation_plane(bool* target, bool* conflict_a, bool* conflict_b, bool* conflict_c, bool* conflict_d)
{
    bool turning_on = !(*target);

    if (turning_on)
    {
        *conflict_a = false;
        *conflict_b = false;
        *conflict_c = false;
        *conflict_d = false;
    }

    *target = turning_on;
}

void toggle_rotation_xy()
{
    toggle_rotation_plane(&APP->view.rotation.xy.enabled,
                          &APP->view.rotation.xz.enabled, &APP->view.rotation.xw.enabled,
                          &APP->view.rotation.yz.enabled, &APP->view.rotation.yw.enabled);
}

void toggle_rotation_xz()
{
    toggle_rotation_plane(&APP->view.rotation.xz.enabled,
                          &APP->view.rotation.xy.enabled, &APP->view.rotation.xw.enabled,
                          &APP->view.rotation.yz.enabled, &APP->view.rotation.zw.enabled);
}

void toggle_rotation_xw()
{
    toggle_rotation_plane(&APP->view.rotation.xw.enabled,
                          &APP->view.rotation.xy.enabled, &APP->view.rotation.xz.enabled,
                          &APP->view.rotation.yw.enabled, &APP->view.rotation.zw.enabled);
}

void toggle_rotation_yz()
{
    toggle_rotation_plane(&APP->view.rotation.yz.enabled,
                          &APP->view.rotation.xy.enabled, &APP->view.rotation.xz.enabled,
                          &APP->view.rotation.yw.enabled, &APP->view.rotation.zw.enabled);
}

void toggle_rotation_yw()
{
    toggle_rotation_plane(&APP->view.rotation.yw.enabled,
                          &APP->view.rotation.xy.enabled, &APP->view.rotation.xw.enabled,
                          &APP->view.rotation.yz.enabled, &APP->view.rotation.zw.enabled);
}

void toggle_rotation_zw()
{
    toggle_rotation_plane(&APP->view.rotation.zw.enabled,
                          &APP->view.rotation.xz.enabled, &APP->view.rotation.xw.enabled,
                          &APP->view.rotation.yz.enabled, &APP->view.rotation.yw.enabled);
}

// cross-platform utils

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && *s2 != '\0')
    {
        unsigned char c1 = (unsigned char)tolower((unsigned char)*s1);
        unsigned char c2 = (unsigned char)tolower((unsigned char)*s2);

        if (c1 != c2)
        {
            return (int)c1 - (int)c2;
        }

        s1++;
        s2++;
    }

    return (unsigned char) tolower((unsigned char) *s1) -
    (unsigned char) tolower((unsigned char) *s2);
}

int strncasecmp(const char* s1, const char* s2, size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    while (n-- > 0)
    {
        unsigned char c1 = (unsigned char) tolower((unsigned char) *s1);
        unsigned char c2 = (unsigned char) tolower((unsigned char) *s2);

        if (c1 != c2 || *s1 == '\0' || *s2 == '\0')
        {
            return (int)c1 - (int)c2;
        }

        s1 += 1;
        s2 += 1;
    }

    return 0;
}

// other

void rgb_to_hex(uint8_t r, uint8_t g, uint8_t b, char* out) {
    if (!out)
    {
        return;
    }
    snprintf(out, 8, "#%02x%02x%02x", r, g, b);
}
