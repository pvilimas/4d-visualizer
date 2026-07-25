#include "visualizer.h"

// -------------------------------- MODELS --------------------------------- //

// ------------------------------ APPLICATION ------------------------------ //

void app_init()
{
    // initialize raylib

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "4D Viewer");
    SetWindowMinSize(600, 400);
    SetExitKey(0);
    SetTargetFPS(60);
    MaximizeWindow();

    // initialize app data

    APP->ui_font = LoadFontEx("/home/paul/.local/share/fonts/Iosevka-Bold.ttf", 32, NULL, 0);
    console_gui_clear();
    console_gui_clear_autofill_entries();
    APP->console_gui_focused = false;

    APP->is_exiting = false;

    // init the console message handlers

    APP->cmh[CMT_QUIT]                       = cmh_quit;
    APP->cmh[CMT_PLAY]                       = cmh_play;
    APP->cmh[CMT_PAUSE]                      = cmh_pause;
    APP->cmh[CMT_RESET]                      = cmh_reset;

    APP->cmh[CMT_CHANGE_VIEW_2D]             = cmh_change_view_2d;
    APP->cmh[CMT_CHANGE_VIEW_3D]             = cmh_change_view_3d;

    APP->cmh[CMT_CHANGE_BGCOLOR]             = cmh_change_bgcolor;

    APP->cmh[CMT_TOGGLE_AXES]                = cmh_toggle_axes;
    APP->cmh[CMT_TOGGLE_HUD]                 = cmh_toggle_hud;

    APP->cmh[CMT_TOGGLE_ROTATION_XY]         = cmh_toggle_rotation_xy;
    APP->cmh[CMT_TOGGLE_ROTATION_XZ]         = cmh_toggle_rotation_xz;
    APP->cmh[CMT_TOGGLE_ROTATION_XW]         = cmh_toggle_rotation_xw;
    APP->cmh[CMT_TOGGLE_ROTATION_YZ]         = cmh_toggle_rotation_yz;
    APP->cmh[CMT_TOGGLE_ROTATION_YW]         = cmh_toggle_rotation_yw;
    APP->cmh[CMT_TOGGLE_ROTATION_ZW]         = cmh_toggle_rotation_zw;

    APP->cmh[CMT_SET_MODEL_TETRA]            = cmh_set_model_tetra;
    APP->cmh[CMT_SET_MODEL_CUBE]             = cmh_set_model_cube;
    APP->cmh[CMT_SET_MODEL_OCT]              = cmh_set_model_oct;
    APP->cmh[CMT_SET_MODEL_ICO]              = cmh_set_model_ico;

    APP->cmh[CMT_SET_MODEL_5CELL]            = cmh_set_model_5cell;
    APP->cmh[CMT_SET_MODEL_8CELL]            = cmh_set_model_8cell;
    APP->cmh[CMT_SET_MODEL_16CELL]           = cmh_set_model_16cell;
    APP->cmh[CMT_SET_MODEL_24CELL]           = cmh_set_model_24cell;

    APP->cmh[CMT_SET_WFADE_ALPHA_ENABLE]     = cmh_set_wfade_alpha_enable;
    APP->cmh[CMT_SET_WFADE_ALPHA_RANGE]      = cmh_set_wfade_alpha_range;
    APP->cmh[CMT_SET_WFADE_ALPHA_SEGMENTS]   = cmh_set_wfade_alpha_segments;
    APP->cmh[CMT_SET_WFADE_ALPHA_NEAR]       = cmh_set_wfade_alpha_near;
    APP->cmh[CMT_SET_WFADE_ALPHA_FAR]        = cmh_set_wfade_alpha_far;

    APP->cmh[CMT_SET_WFADE_COLOR_ENABLE]     = cmh_set_wfade_color_enable;
    APP->cmh[CMT_SET_WFADE_COLOR_RANGE]      = cmh_set_wfade_color_range;
    APP->cmh[CMT_SET_WFADE_COLOR_SEGMENTS]   = cmh_set_wfade_color_segments;
    APP->cmh[CMT_SET_WFADE_COLOR_NEAR]       = cmh_set_wfade_color_near;
    APP->cmh[CMT_SET_WFADE_COLOR_FAR]        = cmh_set_wfade_color_far;

    APP->cmh[CMT_SET_WFADE_THICK_ENABLE]     = cmh_set_wfade_thick_enable;
    APP->cmh[CMT_SET_WFADE_THICK_RANGE]      = cmh_set_wfade_thick_range;
    APP->cmh[CMT_SET_WFADE_THICK_SEGMENTS]   = cmh_set_wfade_thick_segments;
    APP->cmh[CMT_SET_WFADE_THICK_NEAR]       = cmh_set_wfade_thick_near;
    APP->cmh[CMT_SET_WFADE_THICK_FAR]        = cmh_set_wfade_thick_far;

    cmq_init();

    // init all the rest of the simulation data to the defaults
    app_reset();
}

void app_reset()
{
    APP->is_paused = false;

    APP->view.axes.x = (Vector3){ 1, 0, 0 };
    APP->view.axes.y = (Vector3){ 0, 1, 0 };
    APP->view.axes.z = (Vector3){ 0, 0, 1 };
    APP->view.axes.w = Vector3Normalize((Vector3){1, 1, 1});

    APP->view.rotation.xy.enabled = false;
    APP->view.rotation.xy.speed = 0.33f;
    APP->view.rotation.xy.angle = 0.0f;
    APP->view.rotation.xz.enabled = false;
    APP->view.rotation.xz.speed = 0.33f;
    APP->view.rotation.xz.angle = 0.0f;
    APP->view.rotation.xw.enabled = false;
    APP->view.rotation.xw.speed = 0.18f;
    APP->view.rotation.xw.angle = 0.0f;
    APP->view.rotation.yz.enabled = false;
    APP->view.rotation.yz.speed = 0.33f;
    APP->view.rotation.yz.angle = 0.0f;
    APP->view.rotation.yw.enabled = false;
    APP->view.rotation.yw.speed = 0.18f;
    APP->view.rotation.yw.angle = 0.0f;
    APP->view.rotation.zw.enabled = false;
    APP->view.rotation.zw.speed = 0.18f;
    APP->view.rotation.zw.angle = 0.0f;

    APP->view.camera_control.mode = CM_3D_ORBITAL;
    APP->view.camera_control.orbital_yaw = -0.55f;
    APP->view.camera_control.orbital_pitch = 0.37f;
    APP->view.camera_control.orbital_radius = 9.0f;
    APP->view.camera_control.ortho_zoom = 4.0f;
    APP->view.camera_control.ortho_view_dir = (Vector3){0, 0, -1};

    APP->view.camera.fovy = 45.0f;
    APP->view.camera.projection = CAMERA_PERSPECTIVE;

    app_set_model(MT_CUBE);

    APP->settings = get_settings_max_fade();
}

void app_run()
{
    while (!WindowShouldClose() && !APP->is_exiting) {

        // update the camera
        if (APP->view.camera_control.mode == CM_2D_ORTHO && !APP->console_gui_focused)
        {
            update_camera_ortho();
        }
        else if (APP->view.camera_control.mode == CM_3D_ORBITAL && !APP->console_gui_focused)
        {
            update_camera_orbital();
        }

        // poll for keyinput
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (APP->console_gui_focused)
            {
                console_gui_backspace();
            }
        }
        else if (IsKeyPressed(KEY_ENTER))
        {
            if (!APP->console_gui_focused)
            {
                APP->console_gui_focused = true;
                console_gui_clear();
                console_gui_clear_autofill_entries();
                console_gui_refresh_autofill();
            }
            else
            {
                ConsoleMsg msg;
                if (console_parse_line(APP->console_gui_input, &msg))
                {
                    cmq_push(msg);
                }
                APP->console_gui_focused = false;
                console_gui_clear();
                console_gui_clear_autofill_entries();
            }
        }
        else if (APP->console_gui_focused)
        {
            int key = GetCharPressed();
            while (key > 0)
            {
                if (key >= 32 && key <= 125)
                {
                    console_gui_append_char((char)key);
                }
                key = GetCharPressed();
            }
        }

        // poll for console messages
        ConsoleMsg msg;
        while(cmq_pop(&msg))
        {
            // execute the message with the correct handler based on its type
            APP->cmh[msg.type](&msg);
        }

        if (APP->is_exiting)
        {
            break;
        }

        if (!APP->is_paused)
        {
            if (APP->view.rotation.xy.enabled) APP->view.rotation.xy.angle += APP->view.rotation.xy.speed * GetFrameTime();
            if (APP->view.rotation.xz.enabled) APP->view.rotation.xz.angle += APP->view.rotation.xz.speed * GetFrameTime();
            if (APP->view.rotation.xw.enabled) APP->view.rotation.xw.angle += APP->view.rotation.xw.speed * GetFrameTime();
            if (APP->view.rotation.yz.enabled) APP->view.rotation.yz.angle += APP->view.rotation.yz.speed * GetFrameTime();
            if (APP->view.rotation.yw.enabled) APP->view.rotation.yw.angle += APP->view.rotation.yw.speed * GetFrameTime();
            if (APP->view.rotation.zw.enabled) APP->view.rotation.zw.angle += APP->view.rotation.zw.speed * GetFrameTime();
        }

        Vector4 rotated[1024];
        for (int i = 0; i < APP->model.mesh.vertex_count; i += 1)
        {
            Vector4 v = APP->model.mesh.vertices[i];
            v = rotate_vector_in_plane(v, APP->view.rotation.xy.angle, 0, 1);
            v = rotate_vector_in_plane(v, APP->view.rotation.xz.angle, 0, 2);
            v = rotate_vector_in_plane(v, APP->view.rotation.xw.angle, 0, 3);
            v = rotate_vector_in_plane(v, APP->view.rotation.yz.angle, 1, 2);
            v = rotate_vector_in_plane(v, APP->view.rotation.yw.angle, 1, 3);
            v = rotate_vector_in_plane(v, APP->view.rotation.zw.angle, 2, 3);
            rotated[i] = v;
        }

        Vector4 x_axis = (Vector4){1,0,0,0};
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.xy.angle, 0, 1);
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.xz.angle, 0, 2);
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.xw.angle, 0, 3);
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.yz.angle, 1, 2);
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.yw.angle, 1, 3);
        x_axis = rotate_vector_in_plane(x_axis, APP->view.rotation.zw.angle, 2, 3);
        APP->model.axes.x = x_axis;

        Vector4 y_axis = (Vector4){0,1,0,0};
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.xy.angle, 0, 1);
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.xz.angle, 0, 2);
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.xw.angle, 0, 3);
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.yz.angle, 1, 2);
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.yw.angle, 1, 3);
        y_axis = rotate_vector_in_plane(y_axis, APP->view.rotation.zw.angle, 2, 3);
        APP->model.axes.y = y_axis;

        Vector4 z_axis = (Vector4){0,0,1,0};
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.xy.angle, 0, 1);
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.xz.angle, 0, 2);
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.xw.angle, 0, 3);
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.yz.angle, 1, 2);
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.yw.angle, 1, 3);
        z_axis = rotate_vector_in_plane(z_axis, APP->view.rotation.zw.angle, 2, 3);
        APP->model.axes.z = z_axis;

        Vector4 w_axis = (Vector4){0,0,0,1};
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.xy.angle, 0, 1);
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.xz.angle, 0, 2);
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.xw.angle, 0, 3);
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.yz.angle, 1, 2);
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.yw.angle, 1, 3);
        w_axis = rotate_vector_in_plane(w_axis, APP->view.rotation.zw.angle, 2, 3);
        APP->model.axes.w = w_axis;

        BeginDrawing();
        ClearBackground(APP->settings.background_color);
        BeginMode3D(APP->view.camera);

        // draw the model
        // rlDisableDepthMask();
        BeginBlendMode(BLEND_ALPHA);
        for (int e = 0; e < APP->model.mesh.edge_count; e += 1) {
            int i0 = APP->model.mesh.edges[e][0];
            int i1 = APP->model.mesh.edges[e][1];
            draw_line4(rotated[i0], rotated[i1]);
        }
        EndBlendMode();
        // rlEnableDepthMask();

        // draw xyzw axes
        if (APP->settings.world_axes_enabled)
        {
            Vector3 x_start = Vector3Scale(APP->view.axes.x, AXIS_MAGNITUDE);
            Vector3 x_end = Vector3Scale(APP->view.axes.x, -AXIS_MAGNITUDE);
            DrawCylinderEx(x_start, x_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_X_AXIS_COLOR);
            Vector3 y_start = Vector3Scale(APP->view.axes.y, AXIS_MAGNITUDE);
            Vector3 y_end = Vector3Scale(APP->view.axes.y, -AXIS_MAGNITUDE);
            DrawCylinderEx(y_start, y_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_Y_AXIS_COLOR);
            Vector3 z_start = Vector3Scale(APP->view.axes.z, AXIS_MAGNITUDE);
            Vector3 z_end = Vector3Scale(APP->view.axes.z, -AXIS_MAGNITUDE);
            DrawCylinderEx(z_start, z_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_Z_AXIS_COLOR);
            Vector3 w_start = Vector3Scale(APP->view.axes.w, AXIS_MAGNITUDE);
            Vector3 w_end = Vector3Scale(APP->view.axes.w, -AXIS_MAGNITUDE);
            DrawCylinderEx(w_start, w_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_W_AXIS_COLOR);

            // draw the origin
            DrawSphere((Vector3){0, 0, 0}, ORIGIN_SIZE, ORIGIN_COLOR);
        }

        // draw the model axes
        if (APP->settings.local_axes_enabled)
        {
            float scale_factor = 0.25f;

            // if (APP->view.rotation.xy.enabled || APP->view.rotation.xz.enabled || APP->view.rotation.xw.enabled)
            {
                Vector4 x_start_4d = { APP->model.axes.x.x, APP->model.axes.x.y, APP->model.axes.x.z, APP->model.axes.x.w };
                Vector4 x_end_4d   = { -x_start_4d.x, -x_start_4d.y, -x_start_4d.z, -x_start_4d.w };
                Vector3 x_start = Vector3Scale(projection4(x_start_4d), scale_factor);
                Vector3 x_end = Vector3Scale(projection4(x_end_4d), scale_factor);
                DrawCylinderEx(x_start, x_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_X_AXIS_COLOR);
                DrawSphere(x_start, ORIGIN_SIZE/3, LOCAL_X_AXIS_COLOR);
                DrawSphere(x_end, ORIGIN_SIZE/3, LOCAL_X_AXIS_COLOR);
            }

            // if (APP->view.rotation.xy.enabled || APP->view.rotation.yz.enabled || APP->view.rotation.yw.enabled)
            {
                Vector4 y_start_4d = { APP->model.axes.y.x, APP->model.axes.y.y, APP->model.axes.y.z, APP->model.axes.y.w };
                Vector4 y_end_4d   = { -y_start_4d.x, -y_start_4d.y, -y_start_4d.z, -y_start_4d.w };
                Vector3 y_start = Vector3Scale(projection4(y_start_4d), scale_factor);
                Vector3 y_end = Vector3Scale(projection4(y_end_4d), scale_factor);
                DrawCylinderEx(y_start, y_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Y_AXIS_COLOR);
                DrawSphere(y_start, ORIGIN_SIZE/3, LOCAL_Y_AXIS_COLOR);
                DrawSphere(y_end, ORIGIN_SIZE/3, LOCAL_Y_AXIS_COLOR);
            }

            // if (APP->view.rotation.xz.enabled || APP->view.rotation.yz.enabled || APP->view.rotation.zw.enabled)
            {
                Vector4 z_start_4d = { APP->model.axes.z.x, APP->model.axes.z.y, APP->model.axes.z.z, APP->model.axes.z.w };
                Vector4 z_end_4d   = { -z_start_4d.x, -z_start_4d.y, -z_start_4d.z, -z_start_4d.w };
                Vector3 z_start = Vector3Scale(projection4(z_start_4d), scale_factor);
                Vector3 z_end = Vector3Scale(projection4(z_end_4d), scale_factor);
                DrawCylinderEx(z_start, z_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Z_AXIS_COLOR);
                DrawSphere(z_start, ORIGIN_SIZE/3, LOCAL_Z_AXIS_COLOR);
                DrawSphere(z_end, ORIGIN_SIZE/3, LOCAL_Z_AXIS_COLOR);
            }

            // if (APP->view.rotation.xw.enabled || APP->view.rotation.yw.enabled || APP->view.rotation.zw.enabled)
            {
                Vector4 w_start_4d = { APP->model.axes.w.x, APP->model.axes.w.y, APP->model.axes.w.z, APP->model.axes.w.w };
                Vector4 w_end_4d   = { -w_start_4d.x, -w_start_4d.y, -w_start_4d.z, -w_start_4d.w };
                Vector3 w_start = Vector3Scale(projection4(w_start_4d), scale_factor);
                Vector3 w_end = Vector3Scale(projection4(w_end_4d), scale_factor);
                DrawCylinderEx(w_start, w_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_W_AXIS_COLOR);
                DrawSphere(w_start, ORIGIN_SIZE/3, LOCAL_W_AXIS_COLOR);
                DrawSphere(w_end, ORIGIN_SIZE/3, LOCAL_W_AXIS_COLOR);
            }

            DrawSphere((Vector3){0, 0, 0}, ORIGIN_SIZE/2, ORIGIN_COLOR);
        }

        EndMode3D();

        // draw 2d overlays

        // draw XYZW axis highlights for rotation
        if (APP->settings.hud_enabled)
        {
            DrawTextEx(APP->ui_font, "X", (Vector2){20, 20}, 50, 1.0f,
                APP->view.rotation.xy.enabled || APP->view.rotation.xz.enabled || APP->view.rotation.xw.enabled
                    ? WORLD_X_AXIS_COLOR
                    : LOCAL_X_AXIS_COLOR);
            DrawTextEx(APP->ui_font, "Y", (Vector2){50, 20}, 50, 1.0f,
                APP->view.rotation.xy.enabled || APP->view.rotation.yz.enabled || APP->view.rotation.yw.enabled
                    ? WORLD_Y_AXIS_COLOR
                    : LOCAL_Y_AXIS_COLOR);
            DrawTextEx(APP->ui_font, "Z", (Vector2){80, 20}, 50, 1.0f,
                APP->view.rotation.xz.enabled || APP->view.rotation.yz.enabled || APP->view.rotation.zw.enabled
                    ? WORLD_Z_AXIS_COLOR
                    : LOCAL_Z_AXIS_COLOR);
            DrawTextEx(APP->ui_font, "W", (Vector2){110, 20}, 50, 1.0f,
                APP->view.rotation.xw.enabled || APP->view.rotation.yw.enabled || APP->view.rotation.zw.enabled
                    ? WORLD_W_AXIS_COLOR
                    : LOCAL_W_AXIS_COLOR);
        }

        // draw the input window
        if (APP->settings.hud_enabled || APP->console_gui_focused) {
            Vector2 pos = {0, GetScreenHeight() - 90};
            float font_size = 50.0f;
            float row_height = 60.0f;
            float row_spacing = 10.0f;
            Vector2 prefix_size = MeasureTextEx(APP->ui_font, CONSOLE_MSG_PREFIX, font_size, 1.0f);
            Vector2 input_size = MeasureTextEx(APP->ui_font, APP->console_gui_input, font_size, 1.0f);
            float content_width = prefix_size.x + input_size.x;
            for (size_t i = 0; i < APP->console_gui_autofill_entries_len; i += 1)
            {
                Vector2 entry_size = MeasureTextEx(APP->ui_font, APP->console_gui_autofill_entries[i], font_size, 1.0f);
                float entry_width = prefix_size.x + entry_size.x;
                if (entry_width > content_width)
                {
                    content_width = entry_width;
                }
            }
            float box_width = content_width + 40;
            if (box_width < 130)
            {
                box_width = 130;
            }
            Color border_color = (APP->console_gui_focused)
                ? GUI_COLOR_BORDER_LIGHT
                : GUI_COLOR_BORDER_DARK;
            Color fill_color = (APP->console_gui_focused)
                ? GUI_COLOR_FILL_LIGHT
                : GUI_COLOR_FILL_DARK;
            float extra_height = (row_height + row_spacing) * APP->console_gui_autofill_entries_len;
            float height = 60 + extra_height;
            pos.y -= extra_height;

            // calculate rectangle rounded corners
            Rectangle border_rec = (Rectangle){pos.x+10, pos.y+10, box_width, height+10};
            Rectangle fill_rec   = (Rectangle){pos.x+15, pos.y+15, box_width - 10, height};

            float target_radius = 12.0f; // The exact pixel radius we want for corners

            float smallest_side_border = (border_rec.width < border_rec.height) ? border_rec.width : border_rec.height;
            float border_roundness = (target_radius * 2.0f) / smallest_side_border;
            if (border_roundness > 1.0f) border_roundness = 1.0f;

            float smallest_side_fill = (fill_rec.width < fill_rec.height) ? fill_rec.width : fill_rec.height;
            float fill_roundness = (target_radius * 2.0f) / smallest_side_fill;
            if (fill_roundness > 1.0f) fill_roundness = 1.0f;

            // dynamically scale segments so larger menus don't look blocky
            int segments = (int)(target_radius * 0.75f);
            if (segments < 5)  segments = 5;
            if (segments > 24) segments = 24;

            // draw background menus
            DrawRectangleRounded(border_rec, border_roundness, segments, border_color);
            DrawRectangleRounded(fill_rec, fill_roundness, segments, fill_color);

            for (size_t i = 0; i < APP->console_gui_autofill_entries_len; i += 1)
            {
                size_t entry_index = APP->console_gui_autofill_entries_len - 1 - i;
                float row_y = pos.y + 20 + (float)i * (row_height + row_spacing);
                DrawTextEx(APP->ui_font, APP->console_gui_autofill_entries[entry_index], (Vector2){pos.x+20+prefix_size.x+5, row_y}, font_size, 1.0f, DARKGRAY);
            }
            pos.y += extra_height;
            DrawTextEx(APP->ui_font, CONSOLE_MSG_PREFIX, (Vector2){pos.x+20, pos.y+20}, font_size, 1.0f, BLACK);
            DrawTextEx(APP->ui_font, APP->console_gui_input, (Vector2){pos.x+20+prefix_size.x+5, pos.y+20}, font_size, 1.0f, BLACK);
        }

        // draw the w-fade display : ALPHA
        if (APP->settings.hud_enabled) {
            Vector2 offset = {0, 10};
            int inner_width = 320;
            int inner_height = 90;
            int border_thick = 12;
            int font_size = 30;
            Vector2 tl = {
                .x = GetScreenWidth() - (inner_width + (border_thick * 2)) + offset.x,
                .y = border_thick + offset.y};

            Rectangle border_rec = { tl.x, tl.y,
                inner_width + border_thick, inner_height + border_thick};
            Rectangle fill_rec = { tl.x+(border_thick/2), tl.y+(border_thick/2), inner_width, inner_height };

            Color border_color = GUI_COLOR_BORDER_DARK;
            Color fill_color = GUI_COLOR_FILL_DARK;

            DrawRectangleRounded(border_rec, 0.3, 10, border_color);
            DrawRectangleRounded(fill_rec, 0.3, 10, fill_color);

            float total_content_height = (font_size * 2) + 12;
            float top_text_y = fill_rec.y + ((fill_rec.height - total_content_height) / 2.0f);
            float line_y = top_text_y + font_size + 6;
            float bottom_text_y = line_y + 6;

            Vector2 line_start = {tl.x + 35, line_y};
            Vector2 line_end = {tl.x + inner_width - 35, line_y};

            {
                // draw the alpha fade line
                int h = 8;
                float r = 4.0f;
                Color start = Fade(LIGHTGRAY, APP->settings.w_alpha_near);
                Color end = Fade(LIGHTGRAY, APP->settings.w_alpha_far);

                if (!APP->settings.w_alpha_fade_enabled)
                {
                    start = LIGHTGRAY;
                    end = start;

                    DrawRectangle((int)line_start.x, (int)(line_start.y - r), (int)(line_end.x - line_start.x), h, start);
                    DrawCircleSector((Vector2){ line_start.x, line_start.y }, r, 90.0f, 270.0f, 16, start);
                    DrawCircleSector((Vector2){ line_end.x, line_end.y }, r, 270.0f, 450.0f, 16, end);
                }
                else
                {
                    BeginBlendMode(BLEND_ALPHA);
                    DrawRectangleGradientH((int)line_start.x, (int)(line_start.y - r), (int)(line_end.x - line_start.x), h, start, end);
                    DrawCircleSector((Vector2){ line_start.x, line_start.y }, r, 90.0f, 270.0f, 16, start);
                    DrawCircleSector((Vector2){ line_end.x, line_end.y }, r, 270.0f, 450.0f, 16, end);
                    EndBlendMode();
                }
            }

            Vector2 near_text_pos = {
                line_start.x - 13,
                top_text_y
            };

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_near),
                        near_text_pos, font_size, 1.0f, WORLD_W_AXIS_COLOR);

            near_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_dist_near),
                        near_text_pos, font_size, 1.0f, (Color){30,30,30,255});

            Vector2 far_text_pos = {
                line_end.x - MeasureTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_far), font_size, 1.0f).x + 13,
                top_text_y
            };

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_far),
                        far_text_pos, font_size, 1.0f, LOCAL_W_AXIS_COLOR);

            far_text_pos.x = line_end.x - MeasureTextEx(APP->ui_font,
                TextFormat("%g", APP->settings.w_alpha_dist_far),
                    font_size, 1.0f).x + 13;
            far_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_dist_far),
                        far_text_pos, font_size, 1.0f, (Color){30,30,30,255});
        }
        // draw the w-fade display : COLOR
        if (APP->settings.hud_enabled) {
            Vector2 offset = {0, 120};
            int inner_width = 320;
            int inner_height = 90;
            int border_thick = 12;
            int font_size = 30;
            Vector2 tl = {
                .x = GetScreenWidth() - (inner_width + (border_thick * 2)) + offset.x,
                .y = border_thick + offset.y};

            Rectangle border_rec = { tl.x, tl.y,
                inner_width + border_thick, inner_height + border_thick};
            Rectangle fill_rec = { tl.x+(border_thick/2), tl.y+(border_thick/2), inner_width, inner_height };

            Color border_color = GUI_COLOR_BORDER_DARK;
            Color fill_color = GUI_COLOR_FILL_DARK;

            DrawRectangleRounded(border_rec, 0.3, 10, border_color);
            DrawRectangleRounded(fill_rec, 0.3, 10, fill_color);

            float total_content_height = (font_size * 2) + 12;
            float top_text_y = fill_rec.y + ((fill_rec.height - total_content_height) / 2.0f);
            float line_y = top_text_y + font_size + 6;
            float bottom_text_y = line_y + 6;

            Vector2 line_start = {tl.x + 35, line_y};
            Vector2 line_end = {tl.x + inner_width - 35, line_y};

            {
                // draw the color fade line
                int h = 8;
                float r = 4.0f;
                Color start = APP->settings.w_color_near;
                Color end = APP->settings.w_color_far;

                if (!APP->settings.w_color_fade_enabled)
                {
                    end = start;
                }

                BeginBlendMode(BLEND_ALPHA);
                DrawRectangleGradientH((int)line_start.x, (int)(line_start.y - r),
                    (int)(line_end.x - line_start.x), h, start, end);
                DrawCircleSector((Vector2){ line_start.x, line_start.y }, r, 90.0f, 270.0f, 16, start);
                DrawCircleSector((Vector2){ line_end.x, line_end.y }, r, 270.0f, 450.0f, 16, end);
                EndBlendMode();
            }

            Vector2 near_text_pos = {
                line_start.x - 13,
                top_text_y
            };

            char color_str[8];
            rgb_to_hex(APP->settings.w_color_near.r,
                       APP->settings.w_color_near.g,
                       APP->settings.w_color_near.b,
                       color_str);
            DrawTextEx(APP->ui_font, TextFormat("%.*s", 8, color_str),
                        near_text_pos, font_size, 1.0f, WORLD_W_AXIS_COLOR);

            near_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_color_dist_near),
                        near_text_pos, font_size, 1.0f, (Color){30,30,30,255});

            Vector2 text_size = MeasureTextEx(APP->ui_font,
                TextFormat("%g", APP->settings.w_color_far),
                font_size, 1.0f);
            Vector2 far_text_pos = {
                line_end.x - (text_size.x/2) + 13,
                top_text_y
            };

            rgb_to_hex(APP->settings.w_color_far.r,
                APP->settings.w_color_far.g,
                APP->settings.w_color_far.b,
                color_str);
            DrawTextEx(APP->ui_font, TextFormat("%.*s", 8, color_str),
                far_text_pos, font_size, 1.0f, LOCAL_W_AXIS_COLOR);

            text_size = MeasureTextEx(APP->ui_font,
                TextFormat("%g", APP->settings.w_color_dist_far),
                font_size, 1.0f);

            far_text_pos.x = line_end.x - text_size.x + 13;
            far_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_color_dist_far),
                        far_text_pos, font_size, 1.0f, (Color){30,30,30,255});
        }
        // draw the w-fade display : THICK
        if (APP->settings.hud_enabled) {
            Vector2 offset = {0, 230};
            int inner_width = 320;
            int inner_height = 90;
            int border_thick = 12;
            int font_size = 30;
            Vector2 tl = {
                .x = GetScreenWidth() - (inner_width + (border_thick * 2)) + offset.x,
                .y = border_thick + offset.y};

            Rectangle border_rec = { tl.x, tl.y,
                inner_width + border_thick, inner_height + border_thick};
            Rectangle fill_rec = { tl.x+(border_thick/2), tl.y+(border_thick/2), inner_width, inner_height };

            Color border_color = GUI_COLOR_BORDER_DARK;
            Color fill_color = GUI_COLOR_FILL_DARK;

            DrawRectangleRounded(border_rec, 0.3, 10, border_color);
            DrawRectangleRounded(fill_rec, 0.3, 10, fill_color);

            float total_content_height = (font_size * 2) + 12;
            float top_text_y = fill_rec.y + ((fill_rec.height - total_content_height) / 2.0f);
            float line_y = top_text_y + font_size + 6;
            float bottom_text_y = line_y + 6;

            Vector2 line_start = {tl.x + 35, line_y};
            Vector2 line_end = {tl.x + inner_width - 35, line_y};

            {
                // draw the thick fade line with constant 6:1 thickness ratio
                float r_start = 18.0f;
                float r_end   = 3.0f;

                if (!APP->settings.w_thick_fade_enabled)
                {
                    r_start = 4.0f;
                    r_end = 4.0f;
                }

                Color start = GUI_COLOR_BORDER_DARK;
                Color end   = GUI_COLOR_BORDER_DARK;

                BeginBlendMode(BLEND_ALPHA);

                Vector2 p1 = { line_start.x, line_start.y - r_start };
                Vector2 p2 = { line_start.x, line_start.y + r_start };
                Vector2 p3 = { line_end.x,   line_end.y + r_end };
                Vector2 p4 = { line_end.x,   line_end.y - r_end };

                DrawTriangle(p1, p2, p3, start);
                DrawTriangle(p1, p3, p4, start);

                DrawCircleSector((Vector2){ line_start.x, line_start.y }, r_start, 90.0f, 270.0f, 16, start);
                DrawCircleSector((Vector2){ line_end.x, line_end.y }, r_end, 270.0f, 450.0f, 16, end);

                EndBlendMode();
            }

            Vector2 near_text_pos = {
                line_start.x - 13,
                top_text_y
            };

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_thick_near),
                        near_text_pos, font_size, 1.0f, WORLD_W_AXIS_COLOR);

            near_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_alpha_dist_near),
                        near_text_pos, font_size, 1.0f, (Color){30,30,30,255});

            Vector2 far_text_pos = {
                line_end.x - MeasureTextEx(APP->ui_font,
                    TextFormat("%g", APP->settings.w_thick_far),
                        font_size, 1.0f).x + 13,
                top_text_y
            };

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_thick_far),
                        far_text_pos, font_size, 1.0f, LOCAL_W_AXIS_COLOR);

            far_text_pos.x = line_end.x - MeasureTextEx(APP->ui_font,
                TextFormat("%g", APP->settings.w_thick_dist_far), font_size, 1.0f).x + 13;
            far_text_pos.y = bottom_text_y;

            DrawTextEx(APP->ui_font, TextFormat("%g", APP->settings.w_thick_dist_far),
                        far_text_pos, font_size, 1.0f, (Color){30,30,30,255});
        }

        // draw debug information

#if DEBUG_CAMERA_COORDS

        if (APP->view.camera_control.mode == CM_3D_ORBITAL)
        {
            const char* camera_coord_text = TextFormat(
                "Camera:\n"
                "yaw: %f\n"
                "pitch: %f\n"
                "radius: %f\n",
                APP->view.camera_control.orbital_yaw,
                APP->view.camera_control.orbital_pitch,
                APP->view.camera_control.orbital_radius);
            DrawText(camera_coord_text, 50, 300, 20, RED);
        }

#endif
        EndDrawing();
    }
}

void app_exit()
{
    CloseWindow();
    fflush(stdout);
    exit(0);
}

void app_set_model(ModelType type)
{
    mesh4d_free(&APP->model.mesh);
    APP->model.type = type;

    APP->model.axes.x = (Vector4){ 1, 0, 0, 0 };
    APP->model.axes.y = (Vector4){ 0, 1, 0, 0 };
    APP->model.axes.z = (Vector4){ 0, 0, 1, 0 };
    APP->model.axes.w = (Vector4){ 0, 0, 0, 1 };

    // dont set rotation state in here, preserve it for comparing models

    switch(type)
    {
        case MT_TETRA:
            APP->model.mesh = mesh4d_new_tetra();
            break;
        case MT_CUBE:
            APP->model.mesh = mesh4d_new_cube();
            break;
        case MT_OCT:
            APP->model.mesh = mesh4d_new_oct();
            break;
        case MT_ICO:
            APP->model.mesh = mesh4d_new_ico();
            break;
        case MT_5CELL:
            APP->model.mesh = mesh4d_new_5cell();
            break;
        case MT_8CELL:
            APP->model.mesh = mesh4d_new_8cell();
            break;
        case MT_16CELL:
            APP->model.mesh = mesh4d_new_16cell();
            break;
        case MT_24CELL:
            APP->model.mesh = mesh4d_new_24cell();
            break;
        default:
            break;
    }
}

