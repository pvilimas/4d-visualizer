#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "console.h"

// --------------------------------- UTILS --------------------------------- //

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

// projection

// project a vector4 down to 3 dimensions which raylib can project to a 2d screen
Vector3 projection4(Vector4 v);

// rotate in one of the 6 planes: a,b are indices 0=x,1=y,2=z,3=w
Vector4 rotate_vector_in_plane(Vector4 v, float angle, int a, int b);

// convert w-distance (4th dimension) to different things for visualization
float w_fade_factor(float w, float near, float far);
float w_to_alpha(float w);
float w_to_thickness(float w);
Color w_to_color(float w);

// toggle all 4 planes of rotation and turn off other ones to prevent a conflict
void toggle_rotation_plane(bool* target,
                           bool* conflict_a,
                           bool* conflict_b,
                           bool* conflict_c,
                           bool* conflict_d);
void toggle_rotation_xy();
void toggle_rotation_xz();
void toggle_rotation_xw();
void toggle_rotation_yz();
void toggle_rotation_yw();
void toggle_rotation_zw();

// cross-platform utils

int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t n);

// other

// takes a char[8], writes a string like #ff00ff
void rgb_to_hex(uint8_t r, uint8_t g, uint8_t b, char* out);

#endif // UTILS_H
