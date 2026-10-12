#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "console.h"

// --------------------------------- UTILS --------------------------------- //

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

// returns the normal vector for an axis billboard given its position
Vector3 axis_billboard_up(Vector3 billboard_pos, Camera camera);

#endif // UTILS_H
