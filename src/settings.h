#ifndef SETTINGS_H
#define SETTINGS_H

#include "common.h"
#include "camera.h"

// ------------------------------- SETTINGS -------------------------------- //

typedef struct {

    // console settings

    // simulation settings

    // which camera mode to display the simulation in
    CameraMode4D camera_mode;

    Color background_color;     // background color to fill the world

    bool world_axes_enabled;    // enable/disable world axes
    bool local_axes_enabled;    // enable/disable local axes for the model

    bool hud_enabled;           // enable/disable the hud

    // fade alpha linearly along the w-axis (further = more transparent)
    bool w_alpha_fade_enabled;  // enable/disable alpha fade
    int w_alpha_num_segments;   // how many segments to split each line into
    float w_alpha_dist_near;    // w-distance where alpha = w_alpha_near
    float w_alpha_dist_far;     // w-distance where alpha = w_alpha_far
    float w_alpha_near;         // alpha value at w_alpha_dist_near
    float w_alpha_far;          // alpha value at w_alpha_dist_far

    // interpolate color linearly along the w-axis (further = towards color_far)
    bool w_color_fade_enabled;  // enable/disable color fade
    int w_color_num_segments;   // how many segments to split each line into
    float w_color_dist_near;    // w-distance where color = w_color_near
    float w_color_dist_far;     // w-distance where color = w_color_far
    Color w_color_near;         // color at w_color_dist_near
    Color w_color_far;          // color at w_color_dist_far

    // fade thickness linearly along the w-axis (further = thinner)
    bool w_thick_fade_enabled;  // enable/disable thickness fade
    int w_thick_num_segments;   // how many segments to split each line into
    float w_thick_dist_near;    // w-distance where thickness = w_thick_near
    float w_thick_dist_far;     // w-distance where thickness = w_thick_far
    float w_thick_near;         // thickness value at w_thick_dist_near
    float w_thick_far;          // thickness value at w_thick_dist_far

} Settings;

// get different presets of the settings
Settings get_settings_default();    // experimental defaults
Settings get_settings_no_fade();    // no color or alpha fading, no w-depth cues
Settings get_settings_max_fade();   // strong w-depth cues

#endif // SETTINGS_H
