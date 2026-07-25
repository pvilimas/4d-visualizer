#include "settings.h"

// ------------------------------- SETTINGS -------------------------------- //

Settings get_settings_default()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = BLACK,

        .world_axes_enabled = true,
        .local_axes_enabled = false,

        .hud_enabled = true,

        .w_alpha_fade_enabled = true,
        .w_alpha_num_segments = 16,
        .w_alpha_dist_near = 0.2,
        .w_alpha_dist_far = 1.8,
        .w_alpha_near = 1.0,
        .w_alpha_far = 0.1,

        .w_color_fade_enabled = true,
        .w_color_num_segments = 16,
        .w_color_dist_near = 0.2,
        .w_color_dist_far = 2.0,
        .w_color_near = SKYBLUE,
        .w_color_far = BLACK,

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 16,
        .w_thick_near = 0.32f,
        .w_thick_far = 0.01f,
        .w_thick_dist_near = 0.0,
        .w_thick_dist_far = 1.8,
    };
}

Settings get_settings_no_fade()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = BLACK,

        .world_axes_enabled = false,
        .local_axes_enabled = false,

        .hud_enabled = true,

        .w_alpha_fade_enabled = false,
        .w_color_fade_enabled = false,
        .w_thick_fade_enabled = false,
        .w_thick_near = 0.02f,
        .w_thick_far = 0.02f,
        .w_color_near = SKYBLUE,
        .w_color_far = SKYBLUE,
        .w_alpha_near = 1.f,
        .w_alpha_far = 1.f,
    };
}

Settings get_settings_max_fade()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = BLACK,

        .world_axes_enabled = true,
        .local_axes_enabled = true,

        .hud_enabled = true,

        .w_alpha_fade_enabled = true,
        .w_alpha_num_segments = 96,
        .w_alpha_dist_near = 0.2f,
        .w_alpha_dist_far = 2.0f,
        .w_alpha_near = 0.95f,
        .w_alpha_far = 0.4f,

        .w_color_fade_enabled = true,
        .w_color_num_segments = 96,
        .w_color_dist_near = 0.f,
        .w_color_dist_far = 0.025f,
        .w_color_near = (Color){255, 220, 80, 255},
        .w_color_far  = (Color){40, 60, 220, 255},

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 96,
        .w_thick_dist_near = 0.2f,
        .w_thick_dist_far = 1.2f,
        .w_thick_near = 0.1f,
        .w_thick_far = 0.01f,
    };
}
