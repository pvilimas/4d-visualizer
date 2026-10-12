#include "utils.h"
#include "visualizer.h"

// --------------------------------- UTILS --------------------------------- //

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

Vector3 axis_billboard_up(Vector3 billboard_pos, Camera camera) {
    // 1. Calculate vector pointing from billboard to camera
    Vector3 forward = Vector3Subtract(camera.position, billboard_pos);
    float len = Vector3Length(forward);

    if (len < 0.0001f) return camera.up; // Camera is directly on top of billboard
    forward = Vector3Scale(forward, 1.0f / len);

    // 2. Default target up is world Y
    Vector3 target_up = (Vector3){ 0.0f, 1.0f, 0.0f };

    // 3. Check if Forward is parallel to World Y (Singularity check)
    // dot product close to 1.0 or -1.0 means parallel
    float dot = fabsf(Vector3DotProduct(forward, target_up));
    if (dot > 0.999f) {
        // Switch fallback up-vector to Z-axis so cross product never yields zero
        target_up = (Vector3){ 0.0f, 0.0f, 1.0f };
    }

    // 4. Compute robust orthogonal Right and Up vectors
    Vector3 right = Vector3Normalize(Vector3CrossProduct(target_up, forward));
    Vector3 true_up = Vector3CrossProduct(forward, right);

    return true_up;
}
