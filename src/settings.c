#include "settings.h"

// ------------------------------- SETTINGS -------------------------------- //

Settings get_settings_default()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = BLACK,

        .world_axes_enabled = true,
        .local_axes_enabled = true,

        .hud_mode = HM_SIMPLE,

        .w_alpha_fade_enabled = true,
        .w_alpha_num_segments = 96,
        .w_alpha_dist_near = 0.1f,
        .w_alpha_dist_far = 1.8f,
        .w_alpha_near = 1.f,
        .w_alpha_far = 0.6f,

        .w_color_fade_enabled = true,
        .w_color_num_segments = 96,
        .w_color_dist_near = 0.01f,
        .w_color_dist_far = 0.5f,
        .w_color_near = (Color){255, 220, 80, 255},
        .w_color_far  = (Color){40, 60, 220, 255},

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 96,
        .w_thick_dist_near = 0.25f,
        .w_thick_dist_far = 0.75f,
        .w_thick_near = 0.05f,
        .w_thick_far = 0.025f,
    };
}

Settings get_settings_max_fade()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = BLACK,

        .world_axes_enabled = true,
        .local_axes_enabled = true,

        .hud_mode = HM_ALL,

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
        .w_color_near = (Color){234, 126, 11, 255},
        .w_color_far  = (Color){73, 70, 67, 255},

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 96,
        .w_thick_dist_near = 0.2f,
        .w_thick_dist_far = 1.2f,
        .w_thick_near = 0.1f,
        .w_thick_far = 0.01f,
    };
}

Settings get_settings_no_fade()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .background_color = RAYWHITE,

        .world_axes_enabled = false,
        .local_axes_enabled = false,

        .hud_mode = HM_NONE,

        .w_alpha_fade_enabled = false,
        .w_alpha_num_segments = 24,
        .w_alpha_dist_near = 0.3f,
        .w_alpha_dist_far = 1.0f,
        .w_alpha_near = 1.0f,
        .w_alpha_far = 1.0f,

        .w_color_fade_enabled = false,
        .w_color_num_segments = 24,
        .w_color_dist_near = 0.3f,
        .w_color_dist_far = 1.0f,
        .w_color_near = DARKBLUE,
        .w_color_far  = DARKBLUE,

        .w_thick_fade_enabled = false,
        .w_thick_num_segments = 24,
        .w_thick_dist_near = 0.3f,
        .w_thick_dist_far = 1.0f,
        .w_thick_near = 0.04f,
        .w_thick_far = 0.04f,
    };
}
