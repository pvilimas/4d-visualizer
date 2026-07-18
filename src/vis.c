#include "vis.h"

// -------------------------------- 4D MESH -------------------------------- //

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                  const int (*edges)[2], int edge_count)
{
    Mesh4D mesh;
    mesh.vertex_count = vertex_count;
    mesh.edge_count = edge_count;

    mesh.vertices = malloc(sizeof(Vector4) * vertex_count);
    memcpy(mesh.vertices, vertices, sizeof(Vector4) * vertex_count);

    mesh.edges = malloc(sizeof(int[2]) * edge_count);
    memcpy(mesh.edges, edges, sizeof(int[2]) * edge_count);

    return mesh;
}

Mesh4D mesh4d_new_5cell(void)
{
    static const Vector4 VERTICES[5] = {
        {  1,  1,  1, -1.0f / 2.236068f },
        {  1, -1, -1, -1.0f / 2.236068f },
        { -1,  1, -1, -1.0f / 2.236068f },
        { -1, -1,  1, -1.0f / 2.236068f },
        {  0,  0,  0,  4.0f / 2.236068f },
    };
    static const int EDGES[10][2] = {
        {0,1}, {0,2}, {0,3}, {0,4},
        {1,2}, {1,3}, {1,4},
        {2,3}, {2,4},
        {3,4},
    };
    return mesh4d_new(VERTICES, 5, EDGES, 10);
}

Mesh4D mesh4d_new_8cell(void)
{
    static const Vector4 VERTICES[16] = {
        { -1, -1, -1, -1 }, {  1, -1, -1, -1 }, { -1,  1, -1, -1 }, {  1,  1, -1, -1 },
        { -1, -1,  1, -1 }, {  1, -1,  1, -1 }, { -1,  1,  1, -1 }, {  1,  1,  1, -1 },
        { -1, -1, -1,  1 }, {  1, -1, -1,  1 }, { -1,  1, -1,  1 }, {  1,  1, -1,  1 },
        { -1, -1,  1,  1 }, {  1, -1,  1,  1 }, { -1,  1,  1,  1 }, {  1,  1,  1,  1 },
    };

    static const int EDGES[32][2] = {
        {0,1},{0,2},{0,4},{0,8}, {1,3},{1,5},{1,9},
        {2,3},{2,6},{2,10}, {3,7},{3,11},
        {4,5},{4,6},{4,12}, {5,7},{5,13},
        {6,7},{6,14}, {7,15},
        {8,9},{8,10},{8,12}, {9,11},{9,13},
        {10,11},{10,14}, {11,15},
        {12,13},{12,14}, {13,15},
        {14,15},
    };

    return mesh4d_new(VERTICES, 16, EDGES, 32);
}

void mesh4d_free(Mesh4D* mesh)
{
    if (!mesh)
    {
        return;
    }

    if (mesh->vertices)
    {
        free(mesh->vertices);
    }
    if (mesh->edges)
    {
        free(mesh->edges);
    }
    mesh->vertices = NULL;
    mesh->edges = NULL;
    mesh->vertex_count = 0;
    mesh->edge_count = 0;
}

Settings get_default_settings()
{
    return (Settings) {
        .console_mode_gui = true,

        .w_alpha_fade_enabled = false,
        .w_alpha_num_segments = 16,
        .w_alpha_dist_near = 0,
        .w_alpha_dist_far = 2,
        .w_alpha_near = 1,
        .w_alpha_far = 0,

        .w_color_fade_enabled = true,
        .w_color_num_segments = 16,
        .w_color_dist_near = 0,
        .w_color_dist_far = 2,
        .w_color_near = SKYBLUE,
        .w_color_far = BLACK,

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 16,
        .w_thick_near = 0.04f,
        .w_thick_far = 0.01f,
        .w_thick_dist_near = 0,
        .w_thick_dist_far = 2,
    };
}

// ------------------------------- CONSOLE --------------------------------- //

void cmq_init()
{
    APP.cmq.size = 0;
    pthread_mutex_init(&APP.cmq.lock, NULL);
}

bool cmq_push(ConsoleMsg msg)
{
    pthread_mutex_lock(&APP.cmq.lock);

    bool ok = false;

    if (APP.cmq.size < CONSOLE_MSG_MAX_QUEUE_SIZE) {
        APP.cmq.messages[APP.cmq.size] = msg;
        APP.cmq.size += 1;
        ok = true;
    }

    pthread_mutex_unlock(&APP.cmq.lock);
    return ok;
}

bool cmq_pop(ConsoleMsg *msg_out)
{
    if (pthread_mutex_trylock(&APP.cmq.lock) != 0) {
        return false;
    }

    bool got = false;

    if (APP.cmq.size > 0) {
        *msg_out = APP.cmq.messages[0];

        // shift everything down by one so the front of the queue is always at [0]
        for (int i = 1; i < APP.cmq.size; i += 1) {
            APP.cmq.messages[i - 1] = APP.cmq.messages[i];
        }
        APP.cmq.size -= 1;
        got = true;
    }

    pthread_mutex_unlock(&APP.cmq.lock);
    return got;
}

// ----------------------- CONSOLE MESSAGE ARGUMENTS ----------------------- //

ConsoleMsgArgType cma_try_parse(const char* str, size_t len, bool* b_out, int64_t* i_out, float* f_out)
{
    if (cma_try_parse_int(str, len, i_out))
    {
        return CMAT_INT;
    }
    else if (cma_try_parse_float(str, len, f_out))
    {
        return CMAT_FLOAT;
    }
    else if (cma_try_parse_bool(str, len, b_out))
    {
        return CMAT_BOOL;
    }
    else
    {
        return CMAT_INVALID;
    }
}

bool cma_try_parse_bool(const char* str, size_t len, bool* out)
{
    if (len == 1 && str[0] == '1')
    {
        *out = true;
        return true;
    }
    else if (len == 1 && str[0] == '0')
    {
        *out = false;
        return true;
    }
    else if (len == 2 && strncasecmp(str, "on", 2) == 0)
    {
        *out = true;
        return true;
    }
    else if (len == 3 && strncasecmp(str, "off", 3) == 0)
    {
        *out = false;
        return true;
    }
    else if (len == 4 && strncasecmp(str, "true", 4) == 0)
    {
        *out = true;
        return true;
    }
    else if (len == 5 && strncasecmp(str, "false", 5) == 0)
    {
        *out = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool cma_try_parse_int(const char* str, size_t len, int64_t* out)
{
    if (len == 0)
    {
        return false;
    }

    char buf[32];

    if (len >= sizeof(buf))
    {
        return false;
    }

    memcpy(buf, str, len);
    buf[len] = '\0';

    char* endptr = NULL;
    errno = 0;
    long long value = strtoll(buf, &endptr, 10);

    if (endptr == buf)
    {
        return false;
    }
    else if (*endptr != '\0')
    {
        return false;
    }
    else if (errno == ERANGE || value > INT_MAX || value < INT_MIN)
    {
        return false;
    }
    else
    {
        *out = (int64_t)value;
        return true;
    }
}

bool cma_try_parse_float(const char* str, size_t len, float* out)
{
    if (len == 0)
    {
        return false;
    }

    char buf[32];

    if (len >= sizeof(buf))
    {
        return false;
    }

    memcpy(buf, str, len);
    buf[len] = '\0';

    char* endptr = NULL;
    errno = 0;
    float value = strtof(buf, &endptr);

    if (endptr == buf)
    {
        return false;
    }
    else if (*endptr != '\0')
    {
        return false;
    }
    else if (errno == ERANGE)
    {
        return false;
    }
    else
    {
        *out = value;
        return true;
    }
}

void cma_convert_int_to_float(int64_t in, float* out)
{
    *out = (float)in;
}

void cma_convert_int_to_bool(int64_t in, bool* out)
{
    *out = (bool)(!!in);
}

void cma_convert_float_to_int(float in, int64_t* out)
{
    *out = (int64_t)in;
}

void cma_convert_float_to_bool(float in, bool* out)
{
    *out = (bool)in;
}

void cma_convert_bool_to_int(bool in, int64_t* out)
{
    *out = (int64_t)in;
}

void cma_convert_bool_to_float(bool in, float* out)
{
    *out = (float)in;
}


// ----------------------- CONSOLE MESSAGE HANDLERS ------------------------ //

ConsoleMsgResult cmh_quit(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    atomic_store(&APP.is_exiting, 1);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_pause(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    printf("P\n");
    atomic_fetch_xor(&APP.is_paused, 1);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_enable(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // enabled:bool - whether or not to enable alpha

    if (msg->type != CMT_SET_WFADE_ALPHA_ENABLE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#233" };
    }

    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha <on|off>" };
    }

    APP.settings.w_alpha_fade_enabled = msg->args[0].get.bool_value;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_range(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_segments(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_near(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_far(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_enable(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_range(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_segments(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_near(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_far(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_enable(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_range(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_segments(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_near(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_far(const ConsoleMsg* msg)
{
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

// ------------------------------ APPLICATION ------------------------------ //

void app_init()
{
    // initialize raylib

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "4vis");
    SetExitKey(0);
    SetTargetFPS(60);
    MaximizeWindow();

    // initialize app data

    APP.ui_font = LoadFontEx("/home/paul/.local/share/fonts/Iosevka-Bold.ttf", 32, NULL, 0);
    console_gui_clear();
    APP.console_gui_focused = false;

    APP.view.axes.x = (Vector3){ 1, 0, 0 };
    APP.view.axes.y = (Vector3){ 0, 1, 0 };
    APP.view.axes.z = (Vector3){ 0, 0, 1 };
    const float w_axis_len = sqrtf(3.f);
    APP.view.axes.w = (Vector3){ w_axis_len, w_axis_len, w_axis_len };

    APP.view.rotation = (Rotation4D){ 0 };
    APP.view.rotation.xw.enabled = true;
    APP.view.rotation.xw.speed = 0.5f;
    APP.view.rotation.yz.enabled = true;
    APP.view.rotation.yz.speed = 0.5f;

    APP.view.camera.position = (Vector3){ 5.0f, 7.0f, 3.0f };
    APP.view.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    APP.view.camera.up = (Vector3){ 0.0f, 50.0f, 0.0f };
    APP.view.camera.fovy = 45.0f;
    APP.view.camera.projection = CAMERA_PERSPECTIVE;

    app_set_model_5cell();

    APP.settings = get_default_settings();

    // init the console message handlers
    APP.cmh[CMT_QUIT]                       = cmh_quit;
    APP.cmh[CMT_TOGGLE_PAUSE]               = cmh_toggle_pause;

    APP.cmh[CMT_SET_WFADE_ALPHA_ENABLE]    = cmh_set_wfade_alpha_enable;
    APP.cmh[CMT_SET_WFADE_ALPHA_RANGE]     = cmh_set_wfade_alpha_range;
    APP.cmh[CMT_SET_WFADE_ALPHA_SEGMENTS]  = cmh_set_wfade_alpha_segments;
    APP.cmh[CMT_SET_WFADE_ALPHA_NEAR]      = cmh_set_wfade_alpha_near;
    APP.cmh[CMT_SET_WFADE_ALPHA_FAR]       = cmh_set_wfade_alpha_far;

    APP.cmh[CMT_SET_WFADE_COLOR_ENABLE]    = cmh_set_wfade_color_enable;
    APP.cmh[CMT_SET_WFADE_COLOR_RANGE]     = cmh_set_wfade_color_range;
    APP.cmh[CMT_SET_WFADE_COLOR_SEGMENTS]  = cmh_set_wfade_color_segments;
    APP.cmh[CMT_SET_WFADE_COLOR_NEAR]      = cmh_set_wfade_color_near;
    APP.cmh[CMT_SET_WFADE_COLOR_FAR]       = cmh_set_wfade_color_far;

    APP.cmh[CMT_SET_WFADE_THICK_ENABLE]    = cmh_set_wfade_thick_enable;
    APP.cmh[CMT_SET_WFADE_THICK_RANGE]     = cmh_set_wfade_thick_range;
    APP.cmh[CMT_SET_WFADE_THICK_SEGMENTS]  = cmh_set_wfade_thick_segments;
    APP.cmh[CMT_SET_WFADE_THICK_NEAR]      = cmh_set_wfade_thick_near;
    APP.cmh[CMT_SET_WFADE_THICK_FAR]       = cmh_set_wfade_thick_far;

    atomic_store(&APP.is_exiting, 0);

    cmq_init();

    if (!APP.settings.console_mode_gui)
    {
        int result = pthread_create(&APP.console_thread, NULL, console_job_terminal, NULL);
        if (result != 0) {
            // fprintf(stderr, "failed to start console thread\n");
        }
    }
}

void app_run()
{
    while (!WindowShouldClose() && !atomic_load(&APP.is_exiting)) {

        // check keybinds

        // if (IsKeyPressed(KEY_Q))
        // {
        //     atomic_store(&APP.is_exiting, 1);
        // }
        //
        // if (IsKeyPressed(KEY_P))
        // {
        //     atomic_fetch_xor(&APP.is_paused, 1);
        // }

        // if using gui console, poll for keyinput
        if (APP.settings.console_mode_gui)
        {
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (APP.console_gui_focused)
                {
                    console_gui_backspace();
                }
            }
            else if (IsKeyPressed(KEY_ENTER))
            {
                if (!APP.console_gui_focused)
                {
                    APP.console_gui_focused = true;
                }
                else
                {
                    ConsoleMsg msg;
                    if (console_parse_line(APP.console_gui_input, &msg))
                    {
                        cmq_push(msg);
                    }
                    console_gui_clear();
                    APP.console_gui_focused = false;
                }
            }
            else if (APP.console_gui_focused)
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
        }

        // poll for console messages
        ConsoleMsg msg;
        while(cmq_pop(&msg))
        {
            // execute the message with the correct handler based on its type
            APP.cmh[msg.type](&msg);
        }

        if (atomic_load(&APP.is_exiting))
        {
            break;
        }

        if (!atomic_load(&APP.is_paused))
        {
            // apply rotations to all 6 planes of rotation
            if (APP.view.rotation.xw.enabled)
            {
                APP.view.rotation.xw.angle += APP.view.rotation.xw.speed * GetFrameTime();
            }
            if (APP.view.rotation.yz.enabled)
            {
                APP.view.rotation.yz.angle += APP.view.rotation.yz.speed * GetFrameTime();
            }
        }
        if (APP.settings.console_mode_gui && !APP.console_gui_focused)
        {
            UpdateCamera(&APP.view.camera, CAMERA_FREE);
        }
        else if (!APP.settings.console_mode_gui)
        {
            UpdateCamera(&APP.view.camera, CAMERA_FREE);
        }

        float xw_angle = APP.view.rotation.xw.angle;
        float yz_angle = APP.view.rotation.yz.angle;

        Vector4 rotated[APP.model.mesh.vertex_count];
        for (int i = 0; i < APP.model.mesh.vertex_count; i += 1)
        {
            Vector4 v = APP.model.mesh.vertices[i];
            v = rotate_vector_in_plane(v, APP.view.rotation.xw.angle, 0, 3);
            v = rotate_vector_in_plane(v, APP.view.rotation.yz.angle, 1, 2);
            rotated[i] = v;
        }

        Vector4 x_axis = (Vector4){1,0,0,0};
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.xw.angle, 0, 3);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.yz.angle, 1, 2);
        APP.model.axes.x = x_axis;

        Vector4 y_axis = (Vector4){0,1,0,0};
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.xw.angle, 0, 3);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.yz.angle, 1, 2);
        APP.model.axes.y = y_axis;

        Vector4 z_axis = (Vector4){0,0,1,0};
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.xw.angle, 0, 3);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.yz.angle, 1, 2);
        APP.model.axes.z = z_axis;

        Vector4 w_axis = (Vector4){0,0,0,1};
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.xw.angle, 0, 3);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.yz.angle, 1, 2);
        APP.model.axes.w = w_axis;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(APP.view.camera);

        // draw xyzw axes
        {
            Vector3 x_start = Vector3Scale(APP.view.axes.x, AXIS_MAGNITUDE);
            Vector3 x_end = Vector3Scale(APP.view.axes.x, -AXIS_MAGNITUDE);
            DrawCylinderEx(x_start, x_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_X_AXIS_COLOR);
            Vector3 y_start = Vector3Scale(APP.view.axes.y, AXIS_MAGNITUDE);
            Vector3 y_end = Vector3Scale(APP.view.axes.y, -AXIS_MAGNITUDE);
            DrawCylinderEx(y_start, y_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_Y_AXIS_COLOR);
            Vector3 z_start = Vector3Scale(APP.view.axes.z, AXIS_MAGNITUDE);
            Vector3 z_end = Vector3Scale(APP.view.axes.z, -AXIS_MAGNITUDE);
            DrawCylinderEx(z_start, z_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_Z_AXIS_COLOR);
            Vector3 w_start = Vector3Scale(APP.view.axes.w, AXIS_MAGNITUDE);
            Vector3 w_end = Vector3Scale(APP.view.axes.w, -AXIS_MAGNITUDE);
            DrawCylinderEx(w_start, w_end, AXIS_THICKNESS, AXIS_THICKNESS, AXIS_SEGMENTS, WORLD_W_AXIS_COLOR);

            // draw the origin
            DrawSphere((Vector3){0, 0, 0}, ORIGIN_SIZE, ORIGIN_COLOR);
        }

        // draw the model axes
        {
            Vector4 x_start_4d = { APP.model.axes.x.x, APP.model.axes.x.y, APP.model.axes.x.z, APP.model.axes.x.w };
            Vector4 x_end_4d   = { -x_start_4d.x, -x_start_4d.y, -x_start_4d.z, -x_start_4d.w };
            Vector3 x_start = projection4(x_start_4d);
            Vector3 x_end = projection4(x_end_4d);
            DrawCylinderEx(x_start, x_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_X_AXIS_COLOR);
            Vector4 y_start_4d = { APP.model.axes.y.x, APP.model.axes.y.y, APP.model.axes.y.z, APP.model.axes.y.w };
            Vector4 y_end_4d   = { -y_start_4d.x, -y_start_4d.y, -y_start_4d.z, -y_start_4d.w };
            Vector3 y_start = projection4(y_start_4d);
            Vector3 y_end = projection4(y_end_4d);
            DrawCylinderEx(y_start, y_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Y_AXIS_COLOR);
            Vector4 z_start_4d = { APP.model.axes.z.x, APP.model.axes.z.y, APP.model.axes.z.z, APP.model.axes.z.w };
            Vector4 z_end_4d   = { -z_start_4d.x, -z_start_4d.y, -z_start_4d.z, -z_start_4d.w };
            Vector3 z_start = projection4(z_start_4d);
            Vector3 z_end = projection4(z_end_4d);
            DrawCylinderEx(z_start, z_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Z_AXIS_COLOR);
            Vector4 w_start_4d = { APP.model.axes.w.x, APP.model.axes.w.y, APP.model.axes.w.z, APP.model.axes.w.w };
            Vector4 w_end_4d   = { -w_start_4d.x, -w_start_4d.y, -w_start_4d.z, -w_start_4d.w };
            Vector3 w_start = projection4(w_start_4d);
            Vector3 w_end = projection4(w_end_4d);
            DrawCylinderEx(w_start, w_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_W_AXIS_COLOR);

            // draw the endpoints
            DrawSphere(x_start, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(y_start, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(z_start, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(w_start, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(x_end, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(y_end, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(z_end, ORIGIN_SIZE/3, ORIGIN_COLOR);
            DrawSphere(w_end, ORIGIN_SIZE/3, ORIGIN_COLOR);
        }

        // draw the scene
        BeginBlendMode(BLEND_ALPHA);
        for (int e = 0; e < APP.model.mesh.edge_count; e += 1) {
            int i0 = APP.model.mesh.edges[e][0];
            int i1 = APP.model.mesh.edges[e][1];
            draw_line4(rotated[i0], rotated[i1]);
        }

        EndBlendMode();
        EndMode3D();

        // draw 2d overlays

        // draw XYZW axis highlights for rotation
        DrawRectangleRounded((Rectangle){10, 10, 130, 70}, 0.3f, 5, (Color){180,180,180,255});
        DrawRectangleRounded((Rectangle){15, 15, 120, 60}, 0.3f, 5, (Color){120,120,120,255});
        DrawTextEx(APP.ui_font, "X", (Vector2){20, 20}, 50, 1.0f,
            APP.view.rotation.xy.enabled || APP.view.rotation.xz.enabled || APP.view.rotation.xw.enabled
                ? WORLD_X_AXIS_COLOR : LOCAL_X_AXIS_COLOR);
        DrawTextEx(APP.ui_font, "Y", (Vector2){50, 20}, 50, 1.0f,
            APP.view.rotation.xy.enabled || APP.view.rotation.yz.enabled || APP.view.rotation.yw.enabled
                ? WORLD_Y_AXIS_COLOR : LOCAL_Y_AXIS_COLOR);
        DrawTextEx(APP.ui_font, "Z", (Vector2){80, 20}, 50, 1.0f,
            APP.view.rotation.xz.enabled || APP.view.rotation.yz.enabled || APP.view.rotation.zw.enabled
                ? WORLD_Z_AXIS_COLOR : LOCAL_Z_AXIS_COLOR);
        DrawTextEx(APP.ui_font, "W", (Vector2){110, 20}, 50, 1.0f,
            APP.view.rotation.xw.enabled || APP.view.rotation.yw.enabled || APP.view.rotation.zw.enabled
                ? WORLD_W_AXIS_COLOR : LOCAL_W_AXIS_COLOR);

        // draw the input bar if gui input is being used
        if (APP.settings.console_mode_gui)
        {
            Vector2 pos = {0, GetScreenHeight() - 90};
            float font_size = 50.0f;

            Vector2 prefix_size = MeasureTextEx(APP.ui_font, CONSOLE_MSG_PREFIX, font_size, 1.0f);
            Vector2 input_size = MeasureTextEx(APP.ui_font, APP.console_gui_input, font_size, 1.0f);

            float content_width = prefix_size.x + input_size.x;
            float box_width = content_width + 40;
            if (box_width < 130)
            {
                box_width = 130;
            }

            Color border_color = (APP.console_gui_focused)
                ? (Color){180,180,180,255}
                : (Color){90,90,90,255};
            Color fill_color = (APP.console_gui_focused)
                ? (Color){120,120,120,255}
                : (Color){60,60,60,255};

            DrawRectangleRounded((Rectangle){pos.x+10, pos.y+10, box_width, 70}, 0.3f, 5, border_color);
            DrawRectangleRounded((Rectangle){pos.x+15, pos.y+15, box_width - 10, 60}, 0.3f, 5, fill_color);
            DrawTextEx(APP.ui_font, CONSOLE_MSG_PREFIX, (Vector2){pos.x+20, pos.y+20}, font_size, 1.0f, BLACK);
            DrawTextEx(APP.ui_font, APP.console_gui_input, (Vector2){pos.x+20+prefix_size.x+5, pos.y+20}, font_size, 1.0f, BLACK);
        }

        EndDrawing();
    }
}

void app_exit()
{
    CloseWindow();
    fputs("exiting...\n", stdout);
    fflush(stdout);
    _exit(0);
}

// -------------------------------- MODELS --------------------------------- //

void app_set_model_5cell()
{
    mesh4d_free(&APP.model.mesh);
    APP.model.axes.x = (Vector4){ 1, 0, 0, 0 };
    APP.model.axes.y = (Vector4){ 0, 1, 0, 0 };
    APP.model.axes.z = (Vector4){ 0, 0, 1, 0 };
    APP.model.axes.w = (Vector4){ 0, 0, 0, 1 };
    APP.model.mesh = mesh4d_new_5cell();
}

void app_set_model_8cell()
{
    mesh4d_free(&APP.model.mesh);
    APP.model.axes.x = (Vector4){ 1, 0, 0, 0 };
    APP.model.axes.y = (Vector4){ 0, 1, 0, 0 };
    APP.model.axes.z = (Vector4){ 0, 0, 1, 0 };
    APP.model.axes.w = (Vector4){ 0, 0, 0, 1 };
    APP.model.mesh = mesh4d_new_8cell();
}

// --------------------------------- UTILS --------------------------------- //

void draw_line4(Vector4 p0, Vector4 p1)
{
    bool color_on = APP.settings.w_color_fade_enabled;
    bool alpha_on = APP.settings.w_alpha_fade_enabled;
    bool thick_on = APP.settings.w_thick_fade_enabled;
    Vector3 cam_pos = APP.view.camera.position;

    Color flat_color = APP.settings.w_color_near;

    if (!color_on && !alpha_on && !thick_on)
    {
        draw_line_segment4(p0, p1, APP.settings.w_thick_near, APP.settings.w_thick_near, cam_pos, flat_color);
        return;
    }

    int n = 1;
    if (color_on) n = APP.settings.w_color_num_segments;
    if (alpha_on && APP.settings.w_alpha_num_segments > n) n = APP.settings.w_alpha_num_segments;
    if (thick_on && APP.settings.w_thick_num_segments > n) n = APP.settings.w_thick_num_segments;

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

        float width0 = thick_on ? w_to_thickness(seg_start.w) : APP.settings.w_thick_near;
        float width1 = thick_on ? w_to_thickness(seg_end.w)   : APP.settings.w_thick_near;

        Color seg_color = color_on ? w_to_color(w_mid) : flat_color;
        if (alpha_on)
        {
            seg_color = Fade(seg_color, w_to_alpha(w_mid));
        }

        draw_line_segment4(seg_start, seg_end, width0, width1, cam_pos, seg_color);
    }
}

void draw_line_segment3(Vector3 p0, Vector3 p1,
                       float width0, float width1,
                       Vector3 cam_pos,
                       Color color)
{
    Vector3 dir = Vector3Normalize(Vector3Subtract(p1, p0));
    Vector3 mid = Vector3Lerp(p0, p1, 0.5f);
    Vector3 to_cam = Vector3Normalize(Vector3Subtract(cam_pos, mid));

    Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, to_cam));
    if (Vector3LengthSqr(right) < 0.0001f) {
        Vector3 fallback = (fabsf(dir.y) < 0.99f) ? (Vector3){0,1,0} : (Vector3){1,0,0};
        right = Vector3Normalize(Vector3CrossProduct(dir, fallback));
    }

    Vector3 offset0 = Vector3Scale(right, width0 * 0.5f);
    Vector3 offset1 = Vector3Scale(right, width1 * 0.5f);

    Vector3 a = Vector3Subtract(p0, offset0);
    Vector3 b = Vector3Add(p0, offset0);
    Vector3 c = Vector3Add(p1, offset1);
    Vector3 d = Vector3Subtract(p1, offset1);

    DrawTriangle3D(a, b, c, color);
    DrawTriangle3D(a, c, d, color);
}

void draw_line_segment4(Vector4 p0, Vector4 p1,
                       float width0, float width1,
                       Vector3 cam_pos,
                       Color color)
{
    Vector3 proj0 = projection4(p0);
    Vector3 proj1 = projection4(p1);

    Vector3 dir = Vector3Normalize(Vector3Subtract(proj1, proj0));
    Vector3 mid = Vector3Lerp(proj0, proj1, 0.5f);
    Vector3 to_cam = Vector3Normalize(Vector3Subtract(cam_pos, mid));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, to_cam));
    if (Vector3LengthSqr(right) < 0.0001f)
    {
        Vector3 fallback = (fabsf(dir.y) < 0.99f) ? (Vector3){0,1,0} : (Vector3){1,0,0};
        right = Vector3Normalize(Vector3CrossProduct(dir, fallback));
    }

    Vector3 offset0 = Vector3Scale(right, width0 * 0.5f);
    Vector3 offset1 = Vector3Scale(right, width1 * 0.5f);

    Vector3 a = Vector3Subtract(proj0, offset0);
    Vector3 b = Vector3Add(proj0, offset0);
    Vector3 c = Vector3Add(proj1, offset1);
    Vector3 d = Vector3Subtract(proj1, offset1);

    DrawTriangle3D(a, b, c, color);
    DrawTriangle3D(a, c, d, color);
}

Vector3 projection4(Vector4 v)
{
    Vector3 result = { 0, 0, 0 };
    result = Vector3Add(result, Vector3Scale(APP.view.axes.x, v.x));
    result = Vector3Add(result, Vector3Scale(APP.view.axes.y, v.y));
    result = Vector3Add(result, Vector3Scale(APP.view.axes.z, v.z));
    result = Vector3Add(result, Vector3Scale(APP.view.axes.w, v.w));
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
    return w_fade_factor(w, APP.settings.w_alpha_dist_near, APP.settings.w_alpha_dist_far);
}

float w_to_thickness(float w)
{
    float t = w_fade_factor(w, APP.settings.w_thick_dist_near, APP.settings.w_thick_dist_far);
    return APP.settings.w_thick_far + (APP.settings.w_thick_near - APP.settings.w_thick_far) * t;
}

Color w_to_color(float w)
{
    float t = w_fade_factor(w, APP.settings.w_color_dist_near, APP.settings.w_color_dist_far);
    return ColorLerp(APP.settings.w_color_far, APP.settings.w_color_near, t);
}

void* console_job_terminal(void* arg)
{
    char* line = NULL;
    size_t len = 0;

    while (!atomic_load(&APP.is_exiting))
    {

        fputs(CONSOLE_MSG_PREFIX, stdout);
        fflush(stdout);

        if (atomic_load(&APP.is_exiting))
        {
            exit(0);
        }

        ssize_t read = getline(&line, &len, stdin);

        if (atomic_load(&APP.is_exiting))
        {
            exit(0);
        }

        if (read < 0) {
            break; // eof
        }

        line[strcspn(line, "\n")] = '\0';

        ConsoleMsg msg;
        if (console_parse_line(line, &msg))
        {
            // try to push, if the queue is full, fuck it not my problem
            cmq_push(msg);
            // if (!cmq_push(msg)) {
                // fprintf(stderr, "console: message queue full\n");
            // }
        }
        else
        {
            // fprintf(stderr, "console: could not parse: %s\n", line);
        }
    }

    free(line);
    return NULL;
}

void* console_job_gui(void* arg)
{
    char line[CONSOLE_MSG_LEN];
    size_t len = 0;

    while (!atomic_load(&APP.is_exiting))
    {

    }
    return NULL;
}

bool console_parse_line(const char* line, ConsoleMsg* msg_out)
{
    if (!strncmp(line, "quit", 4))
    {
        msg_out->type = CMT_QUIT;
        msg_out->num_args_received = 0;
        return true;
    }
    else if (!strncmp(line, "exit", 4))
    {
        msg_out->type = CMT_QUIT;
        msg_out->num_args_received = 0;
        return true;
    }
    else if (!strncmp(line, "pause", 5))
    {
        msg_out->type = CMT_TOGGLE_PAUSE;
        msg_out->num_args_received = 0;
        return true;
    }
    else
    {
        char working[CONSOLE_MSG_LEN];
        strncpy(working, line, CONSOLE_MSG_LEN - 1);
        working[CONSOLE_MSG_LEN - 1] = '\0';

        char* tokens[3];
        int token_count = 0;

        char* tok = strtok(working, " ");
        while (tok != NULL && token_count < 3)
        {
            tokens[token_count] = tok;
            token_count += 1;
            tok = strtok(NULL, " ");
        }

        if (token_count == 3 &&
            strcmp(tokens[0], "wfade") == 0 &&
            strcmp(tokens[1], "alpha") == 0)
        {
            bool value;
            if (!cma_try_parse_bool(tokens[2], strlen(tokens[2]), &value))
            {
                return false;
            }
            msg_out->type = CMT_SET_WFADE_ALPHA_ENABLE;
            msg_out->num_args_received = 1;
            msg_out->args[0].type = CMAT_BOOL;
            msg_out->args[0].get.bool_value = value;
            return true;
        }

        return false;
    }
}

void console_gui_append_char(char c)
{
    if (APP.console_gui_input_len >= CONSOLE_MSG_LEN - 1)
    {
        APP.console_gui_input_len = CONSOLE_MSG_LEN - 1;
        return;
    }

    APP.console_gui_input[APP.console_gui_input_len] = c;
    APP.console_gui_input_len += 1;
    APP.console_gui_input[APP.console_gui_input_len] = '\0';
}

void console_gui_backspace()
{
    if (APP.console_gui_input_len <= 0)
    {
        return;
    }

    APP.console_gui_input_len -= 1;
    APP.console_gui_input[APP.console_gui_input_len] = '\0';
}

void console_gui_clear()
{
    APP.console_gui_input_len = 0;
    memset(APP.console_gui_input, 0, CONSOLE_MSG_LEN);
}
