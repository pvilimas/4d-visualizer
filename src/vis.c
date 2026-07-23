#include "vis.h"

// -------------------------------- 4D MESH -------------------------------- //

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                  const int (*edges)[2], int edge_count,
                  float radius)
{
    Mesh4D mesh = {0};
    mesh.vertex_count = vertex_count;
    mesh.edge_count = edge_count;

    if (!vertices && !vertex_count && !edges && !edge_count)
    {
        // create an empty mesh
        return (Mesh4D){0};
    }

    mesh.vertices = malloc(sizeof(Vector4) * vertex_count);
    memcpy(mesh.vertices, vertices, sizeof(Vector4) * vertex_count);

    mesh.edges = malloc(sizeof(int[2]) * edge_count);
    memcpy(mesh.edges, edges, sizeof(int[2]) * edge_count);

    mesh.radius = radius;

    return mesh;
}

Mesh4D mesh4d_new_empty()
{
    return mesh4d_new(NULL, 0, NULL, 0, 0);
}

Mesh4D mesh4d_new_tetra()
{
    static const Vector4 VERTICES[4] = {
        {  1,  1,  1, 0 },
        {  1, -1, -1, 0 },
        { -1,  1, -1, 0 },
        { -1, -1,  1, 0 },
    };
    static const int EDGES[6][2] = {
        {0,1}, {0,2}, {0,3},
        {1,2}, {1,3},
        {2,3},
    };
    return mesh4d_new(VERTICES, 4, EDGES, 6, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_cube()
{
    static const Vector4 VERTICES[8] = {
        { -1, -1, -1, 0 }, {  1, -1, -1, 0 }, { -1,  1, -1, 0 }, {  1,  1, -1, 0 },
        { -1, -1,  1, 0 }, {  1, -1,  1, 0 }, { -1,  1,  1, 0 }, {  1,  1,  1, 0 },
    };
    static const int EDGES[12][2] = {
        {0,1}, {0,2}, {0,4},
        {1,3}, {1,5},
        {2,3}, {2,6},
        {3,7},
        {4,5}, {4,6},
        {5,7},
        {6,7},
    };
    return mesh4d_new(VERTICES, 8, EDGES, 12, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_oct()
{
    static const Vector4 VERTICES[6] = {
        {  1,  0,  0, 0 },
        { -1,  0,  0, 0 },
        {  0,  1,  0, 0 },
        {  0, -1,  0, 0 },
        {  0,  0,  1, 0 },
        {  0,  0, -1, 0 },
    };

    static const int EDGES[12][2] = {
        {0, 2}, {2, 1}, {1, 3}, {3, 0},
        {4, 0}, {4, 1}, {4, 2}, {4, 3},
        {5, 0}, {5, 1}, {5, 2}, {5, 3},
    };

    return mesh4d_new(VERTICES, 6, EDGES, 12, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_ico()
{
    static const int phi = 1.618033988749895f;

    static const Vector4 VERTICES[12] = {
        { 0,  1,  phi, 0 },
        { 0,  1, -phi, 0 },
        { 0, -1,  phi, 0 },
        { 0, -1, -phi, 0 },
        {  1,  phi, 0, 0 },
        {  1, -phi, 0, 0 },
        { -1,  phi, 0, 0 },
        { -1, -phi, 0, 0 },
        {  phi, 0,  1, 0 },
        {  phi, 0, -1, 0 },
        { -phi, 0,  1, 0 },
        { -phi, 0, -1, 0 }
    };

    static const int EDGES[30][2] = {
        {0, 2}, {0, 4}, {0, 6}, {0, 8}, {0, 10},
        {1, 3}, {1, 4}, {1, 6}, {1, 9}, {1, 11},
        {2, 5}, {2, 7}, {2, 8}, {2, 10},
        {3, 5}, {3, 7}, {3, 9}, {3, 11},
        {4, 6}, {4, 8}, {4, 9},
        {5, 7}, {5, 8}, {5, 9},
        {6, 10}, {6, 11},
        {7, 10}, {7, 11},
        {8, 9}, {10, 11}
    };

    return mesh4d_new(VERTICES, 12, EDGES, 30, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_5cell()
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
    return mesh4d_new(VERTICES, 5, EDGES, 10, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_8cell()
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

    return mesh4d_new(VERTICES, 16, EDGES, 32, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_16cell()
{
    static const Vector4 VERTICES[8] = {
        {  1,  0,  0,  0 }, { -1,  0,  0,  0 },
        {  0,  1,  0,  0 }, {  0, -1,  0,  0 },
        {  0,  0,  1,  0 }, {  0,  0, -1,  0 },
        {  0,  0,  0,  1 }, {  0,  0,  0, -1 },
    };

    static const int EDGES[24][2] = {
        {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7},
        {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7},
        {2,4}, {2,5}, {2,6}, {2,7},
        {3,4}, {3,5}, {3,6}, {3,7},
        {4,6}, {4,7},
        {5,6}, {5,7},
    };

    return mesh4d_new(VERTICES, 8, EDGES, 24, Vector4Length(VERTICES[0]));
}

Mesh4D mesh4d_new_24cell()
{
    Vector4 verts[24];
    int count = 0;

    int axis_pairs[6][2] = {
        {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3},
    };

    for (int p = 0; p < 6; p += 1)
    {
        int a = axis_pairs[p][0];
        int b = axis_pairs[p][1];

        for (int sa = -1; sa <= 1; sa += 2)
        {
            for (int sb = -1; sb <= 1; sb += 2)
            {
                Vector4 v = { 0, 0, 0, 0 };
                float* coords = (float*)&v;
                coords[a] = (float)sa;
                coords[b] = (float)sb;
                verts[count] = v;
                count += 1;
            }
        }
    }

    int edges[96][2];
    int edge_count = 0;
    float edge_len_sq = 2.0f;

    for (int i = 0; i < 24; i += 1)
    {
        for (int j = i + 1; j < 24; j += 1)
        {
            float dx = verts[i].x - verts[j].x;
            float dy = verts[i].y - verts[j].y;
            float dz = verts[i].z - verts[j].z;
            float dw = verts[i].w - verts[j].w;
            float dist_sq = dx*dx + dy*dy + dz*dz + dw*dw;

            if (fabsf(dist_sq - edge_len_sq) < 0.001f)
            {
                edges[edge_count][0] = i;
                edges[edge_count][1] = j;
                edge_count += 1;
            }
        }
    }

    return mesh4d_new(verts, 24, (const int (*)[2])edges, edge_count, sqrtf(2.f));
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
    mesh->radius = 0;
}

// ------------------------------- 4D CAMERA ------------------------------- //

void update_camera_ortho()
{
    APP.view.camera_control.ortho_zoom -= GetMouseWheelMove() * 0.5f;
    APP.view.camera_control.ortho_zoom = Clamp(APP.view.camera_control.ortho_zoom,
                                               ORTHO_ZOOM_MIN,
                                               ORTHO_ZOOM_MAX);

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 mouse_delta = GetMouseDelta();
        APP.view.camera_control.ortho_pan.x -= mouse_delta.x * 0.01f * APP.view.camera_control.ortho_zoom;
        APP.view.camera_control.ortho_pan.y += mouse_delta.y * 0.01f * APP.view.camera_control.ortho_zoom;
    }

    Vector3 dir = Vector3Normalize(APP.view.camera_control.ortho_view_dir);
    Vector3 world_up = (Vector3){0, 1, 0};
    Vector3 right = Vector3Normalize(Vector3CrossProduct(world_up, dir));
    if (Vector3LengthSqr(right) < 0.0001f)
    {
        right = (Vector3){1, 0, 0};
    }
    Vector3 up = Vector3CrossProduct(dir, right);

    Vector3 target = Vector3Add(
        Vector3Scale(right, APP.view.camera_control.ortho_pan.x),
                                Vector3Scale(up, APP.view.camera_control.ortho_pan.y)
    );

    APP.view.camera.target = target;
    APP.view.camera.position = Vector3Subtract(target, Vector3Scale(dir, 20.0f));
    APP.view.camera.up = up;
    APP.view.camera.projection = CAMERA_ORTHOGRAPHIC;
    APP.view.camera.fovy = APP.view.camera_control.ortho_zoom * 10.0f;
}

void update_camera_orbital()
{
    float dt = GetFrameTime();
    float turn_speed = 1.5f;

    if (IsKeyDown(KEY_A)) { APP.view.camera_control.orbital_yaw -= turn_speed * dt; }
    if (IsKeyDown(KEY_D)) { APP.view.camera_control.orbital_yaw += turn_speed * dt; }
    if (IsKeyDown(KEY_W)) { APP.view.camera_control.orbital_pitch += turn_speed * dt; }
    if (IsKeyDown(KEY_S)) { APP.view.camera_control.orbital_pitch -= turn_speed * dt; }

    float pitch_limit = 1.5f;
    if (APP.view.camera_control.orbital_pitch > pitch_limit) { APP.view.camera_control.orbital_pitch = pitch_limit; }
    if (APP.view.camera_control.orbital_pitch < -pitch_limit) { APP.view.camera_control.orbital_pitch = -pitch_limit; }

    float wheel = GetMouseWheelMove();
    APP.view.camera_control.orbital_radius -= wheel * 1.0f;
    if (APP.view.camera_control.orbital_radius < 1.0f) { APP.view.camera_control.orbital_radius = 1.0f; }
    if (APP.view.camera_control.orbital_radius > 100.0f) { APP.view.camera_control.orbital_radius = 100.0f; }

    float yaw = APP.view.camera_control.orbital_yaw;
    float pitch = APP.view.camera_control.orbital_pitch;
    float radius = APP.view.camera_control.orbital_radius;

    Vector3 pos;
    pos.x = radius * cosf(pitch) * sinf(yaw);
    pos.y = radius * sinf(pitch);
    pos.z = radius * cosf(pitch) * cosf(yaw);

    APP.view.camera.position = pos;
    APP.view.camera.target = (Vector3){0, 0, 0};
    APP.view.camera.up = (Vector3){0, 1, 0};
}

// -------------------------------- 4D VIEW -------------------------------- //

bool view4d_is_any_axis_rotating(const View4D* v)
{
    return (v->rotation.xy.enabled && v->rotation.xy.speed != 0)
        || (v->rotation.xz.enabled && v->rotation.xz.speed != 0)
        || (v->rotation.xw.enabled && v->rotation.xw.speed != 0)
        || (v->rotation.yz.enabled && v->rotation.yz.speed != 0)
        || (v->rotation.yw.enabled && v->rotation.yw.speed != 0)
        || (v->rotation.zw.enabled && v->rotation.zw.speed != 0);
}

// ------------------------------- SETTINGS -------------------------------- //

Settings get_settings_default()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .world_axes_enabled = true,
        .local_axes_enabled = false,

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

        .world_axes_enabled = false,
        .local_axes_enabled = false,

        .w_alpha_fade_enabled = false,
        .w_color_fade_enabled = false,
        .w_thick_fade_enabled = false,
        .w_thick_near = 0.02f,
        .w_thick_far = 0.02f,
        .w_color_near = SKYBLUE,
        .w_color_far = SKYBLUE,
    };
}

Settings get_settings_max_fade()
{
    return (Settings) {

        .camera_mode = CM_3D_ORBITAL,

        .world_axes_enabled = true,
        .local_axes_enabled = true,

        .w_alpha_fade_enabled = true,
        .w_alpha_num_segments = 24,
        .w_alpha_dist_near = 0.2f,
        .w_alpha_dist_far = 2.0f,
        .w_alpha_near = 0.95f,
        .w_alpha_far = 0.4f,

        .w_color_fade_enabled = true,
        .w_color_num_segments = 24,
        .w_color_dist_near = 0.f,
        .w_color_dist_far = 0.025f,
        .w_color_near = (Color){255, 220, 80, 255},
        .w_color_far  = (Color){40, 60, 220, 255},

        .w_thick_fade_enabled = true,
        .w_thick_num_segments = 24,
        .w_thick_dist_near = 0.3f,
        .w_thick_dist_far = 5.0f,
        .w_thick_near = 0.06f,
        .w_thick_far = 0.02f,
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

void cma_convert_to_bool(const ConsoleMsgArg* arg, bool* out)
{
    switch(arg->type)
    {
        case CMAT_BOOL:
            *out = arg->get.bool_value;
            return;
        case CMAT_INT:
            cmatc_int_to_bool(arg->get.int_value, out);
            return;
        case CMAT_FLOAT:
            cmatc_float_to_bool(arg->get.float_value, out);
            return;
        case CMAT_STRING:
            cmatc_string_to_bool((char*)arg->get.string_value, out);
            return;
        default:
            return;
    }
}

void cma_convert_to_int(const ConsoleMsgArg* arg, int64_t* out)
{
    switch(arg->type)
    {
        case CMAT_BOOL:
            cmatc_bool_to_int(arg->get.bool_value, out);
            return;
        case CMAT_INT:
            *out = arg->get.int_value;
            return;
        case CMAT_FLOAT:
            cmatc_float_to_int(arg->get.float_value, out);
            return;
        case CMAT_STRING:
            cmatc_string_to_int((char*)arg->get.string_value, out);
            return;
        default:
            return;
    }
}

void cma_convert_to_float(const ConsoleMsgArg* arg, float* out)
{
    switch(arg->type)
    {
        case CMAT_BOOL:
            cmatc_bool_to_float(arg->get.bool_value, out);
            return;
        case CMAT_INT:
            cmatc_int_to_float(arg->get.int_value, out);
            return;
        case CMAT_FLOAT:
            *out = arg->get.float_value;
            return;
        case CMAT_STRING:
            cmatc_string_to_float((char*)arg->get.string_value, out);
            return;
        default:
            return;
    }
}

void cma_convert_to_string(const ConsoleMsgArg* arg, char* out)
{
    switch(arg->type)
    {
        case CMAT_BOOL:
            cmatc_bool_to_string(arg->get.bool_value, out);
            return;
        case CMAT_INT:
            cmatc_int_to_string(arg->get.int_value, out);
            return;
        case CMAT_FLOAT:
            cmatc_float_to_string(arg->get.float_value, out);
            return;
        case CMAT_STRING:
            memcpy(out, arg->get.string_value, CONSOLE_MSG_LEN-1);
            return;
        default:
            return;
    }
}

ConsoleMsgArgType cma_parse(const char* str, size_t len, bool* b_out, int64_t* i_out, float* f_out, char* s_out)
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
        cma_try_parse_string(str, len, s_out);
        return CMAT_STRING;
    }
}

bool cma_try_parse_bool(const char* str, size_t len, bool* out)
{
    if (!str || !len || !out)
    {
        return false;
    }

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
    if (!str || !len || !out)
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
    if (!str || !len || !out)
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

bool cma_try_parse_string(const char* str, size_t len, char* out)
{
    if (!str || !len || !out)
    {
        return false;
    }

    memcpy(out, str, MIN(len, CONSOLE_MSG_LEN - 1));
    return true;
}

void cmatc_int_to_float(int64_t in, float* out)
{
    *out = (float)in;
}

void cmatc_int_to_bool(int64_t in, bool* out)
{
    *out = (bool)(!!in);
}

void cmatc_int_to_string(int64_t in, char* out)
{
    snprintf(out, 63, "%"PRId64"", in);
}

void cmatc_float_to_int(float in, int64_t* out)
{
    *out = (int64_t)in;
}

void cmatc_float_to_bool(float in, bool* out)
{
    *out = (bool)in;
}

void cmatc_float_to_string(float in, char* out)
{
    snprintf(out, 63, "%f", in);
}

void cmatc_bool_to_int(bool in, int64_t* out)
{
    *out = (int64_t)in;
}

void cmatc_bool_to_float(bool in, float* out)
{
    *out = (float)in;
}

void cmatc_bool_to_string(bool in, char* out)
{
    if (in)
    {
        snprintf(out, 63, "%s", "true");
    }
    else
    {
        snprintf(out, 63, "%s", "false");
    }
}

void cmatc_string_to_int(char* in, int64_t* out)
{
    *out = (int64_t)strtoll(in, NULL, 10);
}

void cmatc_string_to_float(char* in, float* out)
{
    *out = strtof(in, NULL);
}

void cmatc_string_to_bool(char* in, bool* out)
{
    while (isspace((unsigned char)*in))
    {
        in += 1;
    }

    if (!strncasecmp(in, "true", 4) || !strncasecmp(in, "on", 2))
    {
        *out = true;
    }
    else
    {
        *out = false;
    }
}

// ------------------------ CONSOLE MESSAGE SCHEMAS ------------------------ //

ConsoleMsgSchema get_console_msg_schema(ConsoleMsgType type)
{
    static const ConsoleMsgSchema SCHEMAS[CMT_COUNT] = {

        [CMT_QUIT]  = { CMT_QUIT,  { "quit" },  1, { 0 }, 0 },
        [CMT_PLAY]  = { CMT_PLAY,  { "play" },  1, { 0 }, 0 },
        [CMT_PAUSE] = { CMT_PAUSE, { "pause" }, 1, { 0 }, 0 },
        [CMT_RESET] = { CMT_RESET, { "reset" }, 1, { 0 }, 0 },

        [CMT_CHANGE_VIEW_2D] = { CMT_CHANGE_VIEW_2D, { "view", "2d" }, 2, { 0 }, 0 },
        [CMT_CHANGE_VIEW_3D] = { CMT_CHANGE_VIEW_3D, { "view", "3d" }, 2, { 0 }, 0 },

        [CMT_TOGGLE_AXES] = { CMT_TOGGLE_AXES, { "axes" }, 1, { 0 }, 0 },

        [CMT_TOGGLE_ROTATION_XY] = { CMT_TOGGLE_ROTATION_XY, { "rotate", "xy" }, 2, { 0 }, 0 },
        [CMT_TOGGLE_ROTATION_XZ] = { CMT_TOGGLE_ROTATION_XZ, { "rotate", "xz" }, 2, { 0 }, 0 },
        [CMT_TOGGLE_ROTATION_XW] = { CMT_TOGGLE_ROTATION_XW, { "rotate", "xw" }, 2, { 0 }, 0 },
        [CMT_TOGGLE_ROTATION_YZ] = { CMT_TOGGLE_ROTATION_YZ, { "rotate", "yz" }, 2, { 0 }, 0 },
        [CMT_TOGGLE_ROTATION_YW] = { CMT_TOGGLE_ROTATION_YW, { "rotate", "yw" }, 2, { 0 }, 0 },
        [CMT_TOGGLE_ROTATION_ZW] = { CMT_TOGGLE_ROTATION_ZW, { "rotate", "zw" }, 2, { 0 }, 0 },

        [CMT_SET_MODEL_TETRA] = { CMT_SET_MODEL_TETRA, { "model", "tetra" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_CUBE]  = { CMT_SET_MODEL_CUBE, { "model", "cube" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_OCT]   = { CMT_SET_MODEL_OCT, { "model", "oct" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_ICO]   = { CMT_SET_MODEL_ICO, { "model", "ico" }, 2, { 0 }, 0 },

        [CMT_SET_MODEL_5CELL] = { CMT_SET_MODEL_5CELL, { "model", "5cell" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_8CELL] = { CMT_SET_MODEL_8CELL, { "model", "8cell" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_16CELL] = { CMT_SET_MODEL_16CELL, { "model", "16cell" }, 2, { 0 }, 0 },
        [CMT_SET_MODEL_24CELL] = { CMT_SET_MODEL_24CELL, { "model", "24cell" }, 2, { 0 }, 0 },

        [CMT_SET_WFADE_ALPHA_ENABLE]   = { CMT_SET_WFADE_ALPHA_ENABLE,   { "wfade", "alpha" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_ALPHA_RANGE]    = { CMT_SET_WFADE_ALPHA_RANGE,    { "wfade", "alpha", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_ALPHA_SEGMENTS] = { CMT_SET_WFADE_ALPHA_SEGMENTS, { "wfade", "alpha", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_ALPHA_NEAR]     = { CMT_SET_WFADE_ALPHA_NEAR,     { "wfade", "alpha", "near" },     3, { CMAT_FLOAT },                         1 },
        [CMT_SET_WFADE_ALPHA_FAR]      = { CMT_SET_WFADE_ALPHA_FAR,      { "wfade", "alpha", "far" },      3, { CMAT_FLOAT },                         1 },

        [CMT_SET_WFADE_COLOR_ENABLE]   = { CMT_SET_WFADE_COLOR_ENABLE,   { "wfade", "color" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_COLOR_RANGE]    = { CMT_SET_WFADE_COLOR_RANGE,    { "wfade", "color", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_COLOR_SEGMENTS] = { CMT_SET_WFADE_COLOR_SEGMENTS, { "wfade", "color", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_COLOR_NEAR]     = { CMT_SET_WFADE_COLOR_NEAR,     { "wfade", "color", "near" },     3, { CMAT_FLOAT, CMAT_FLOAT, CMAT_FLOAT }, 3 },
        [CMT_SET_WFADE_COLOR_FAR]      = { CMT_SET_WFADE_COLOR_FAR,      { "wfade", "color", "far" },      3, { CMAT_FLOAT, CMAT_FLOAT, CMAT_FLOAT }, 3 },

        [CMT_SET_WFADE_THICK_ENABLE]   = { CMT_SET_WFADE_THICK_ENABLE,   { "wfade", "thick" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_THICK_RANGE]    = { CMT_SET_WFADE_THICK_RANGE,    { "wfade", "thick", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_THICK_SEGMENTS] = { CMT_SET_WFADE_THICK_SEGMENTS, { "wfade", "thick", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_THICK_NEAR]     = { CMT_SET_WFADE_THICK_NEAR,     { "wfade", "thick", "near" },     3, { CMAT_FLOAT },                         1 },
        [CMT_SET_WFADE_THICK_FAR]      = { CMT_SET_WFADE_THICK_FAR,      { "wfade", "thick", "far" },      3, { CMAT_FLOAT },                         1 },
    };

    return SCHEMAS[type];
}

// ----------------------- CONSOLE MESSAGE HANDLERS ------------------------ //

ConsoleMsgResult cmh_quit(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    atomic_store(&APP.is_exiting, 1);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_play(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    atomic_store(&APP.is_paused, 0);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_pause(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    atomic_store(&APP.is_paused, 1);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_reset(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    app_reset();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_change_view_2d(const ConsoleMsg* msg)
{
    if (msg->type != CMT_CHANGE_VIEW_2D)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#726" };
    }
    APP.view.camera_control.mode = CM_2D_ORTHO;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_change_view_3d(const ConsoleMsg* msg)
{
    if (msg->type != CMT_CHANGE_VIEW_3D)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#719" };
    }
    APP.view.camera_control.mode = CM_3D_ORBITAL;
    APP.view.camera.projection = CAMERA_PERSPECTIVE;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_axes(const ConsoleMsg* msg)
{
    // takes 0 arguments (for now)
    if (msg->type != CMT_TOGGLE_AXES)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#734" };
    }

    // if (msg->num_args_received == 1)
    // {
    //     // try to parse "all", "none", "local", "model", or "world" out of it
    //
    //     char str[CONSOLE_MSG_LEN];
    //     cma_convert_to_string(&msg->args[0], str);
    //
    //     if (!strncasecmp(str, "local", 5) || !strncasecmp(str, "model", 5))
    //     {
    //         APP.settings.world_axes_enabled = false;
    //         APP.settings.local_axes_enabled = true;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "world", 5))
    //     {
    //         APP.settings.world_axes_enabled = true;
    //         APP.settings.local_axes_enabled = false;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "none", 4))
    //     {
    //         APP.settings.world_axes_enabled = false;
    //         APP.settings.local_axes_enabled = false;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "all", 3))
    //     {
    //         APP.settings.world_axes_enabled = true;
    //         APP.settings.local_axes_enabled = true;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    // }

    bool* world = &APP.settings.world_axes_enabled;
    bool* local = &APP.settings.local_axes_enabled;

    if (!*world && !*local)       { *world = true;  *local = false; }
    else if (*world && !*local)   { *world = false; *local = true;  }
    else if (!*world && *local)   { *world = true;  *local = true;  }
    else                          { *world = false; *local = false; }

    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_xy(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_XY)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#709" };
    }
    toggle_rotation_xy();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_xz(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_XZ)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#719" };
    }
    toggle_rotation_xz();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_xw(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_XW)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#729" };
    }
    toggle_rotation_xw();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_yz(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_YZ)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#739" };
    }
    toggle_rotation_yz();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_yw(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_YW)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#749" };
    }
    toggle_rotation_yw();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_rotation_zw(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_ROTATION_ZW)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#759" };
    }
    toggle_rotation_zw();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_tetra(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_TETRA)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#699" };
    }
    app_set_model(MT_TETRA);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_cube(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_CUBE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#709" };
    }
    app_set_model(MT_CUBE);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_oct(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_OCT)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#846" };
    }
    app_set_model(MT_OCT);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_ico(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_ICO)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#876" };
    }
    app_set_model(MT_ICO);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_5cell(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_5CELL)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#623" };
    }
    app_set_model(MT_5CELL);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_8cell(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_8CELL)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#633" };
    }
    app_set_model(MT_8CELL);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_16cell(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_16CELL)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#1044" };
    }
    app_set_model(MT_16CELL);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_model_24cell(const ConsoleMsg* msg)
{
    if (msg->type != CMT_SET_MODEL_24CELL)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#941" };
    }
    app_set_model(MT_24CELL);
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_enable(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // enabled:bool - whether or not to enable alpha
    if (msg->type != CMT_SET_WFADE_ALPHA_ENABLE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#517" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP.settings.w_alpha_fade_enabled = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_range(const ConsoleMsg* msg)
{
    // takes 2 arguments:
    // w-near:float - w-distance where alpha = w_alpha_near
    // w-far:float  - w-distance where alpha = w_alpha_far
    if (msg->type != CMT_SET_WFADE_ALPHA_RANGE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#536" };
    }
    if (msg->num_args_received < 2)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP.settings.w_alpha_dist_near = arg0;
    APP.settings.w_alpha_dist_far = arg1;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_segments(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // n:int - how many segments to split each line into
    if (msg->type != CMT_SET_WFADE_ALPHA_SEGMENTS)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#557" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP.settings.w_alpha_num_segments = (int)arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_near(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // a:float - alpha value at w_alpha_dist_near
    if (msg->type != CMT_SET_WFADE_ALPHA_NEAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#575" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha near <a:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP.settings.w_alpha_near = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_alpha_far(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // a:float - alpha value at w_alpha_dist_far
    if (msg->type != CMT_SET_WFADE_ALPHA_FAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#593" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade alpha far <a:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP.settings.w_alpha_far = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_enable(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // enabled:bool - whether or not to enable color fade
    if (msg->type != CMT_SET_WFADE_COLOR_ENABLE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#611" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade color <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP.settings.w_color_fade_enabled = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_range(const ConsoleMsg* msg)
{
    // takes 2 arguments:
    // w-near:float - w-distance where color = w_color_near
    // w-far:float  - w-distance where color = w_color_far
    if (msg->type != CMT_SET_WFADE_COLOR_RANGE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#630" };
    }
    if (msg->num_args_received < 2)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade color range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP.settings.w_color_dist_near = arg0;
    APP.settings.w_color_dist_far = arg1;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_segments(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // n:int - how many segments to split each line into
    if (msg->type != CMT_SET_WFADE_COLOR_SEGMENTS)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#651" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade color segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP.settings.w_color_num_segments = (int)arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_near(const ConsoleMsg* msg)
{
    // takes 3 arguments:
    // r:float, g:float, b:float - color at w_color_dist_near
    if (msg->type != CMT_SET_WFADE_COLOR_NEAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#669" };
    }
    if (msg->num_args_received < 3)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade color near <r:float> <g:float> <b:float>" };
    }
    float arg0;
    float arg1;
    float arg2;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    cma_convert_to_float(&msg->args[2], &arg2);
    APP.settings.w_color_near = (Color){ (unsigned char)arg0, (unsigned char)arg1, (unsigned char)arg2, 255 };
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_far(const ConsoleMsg* msg)
{
    // takes 3 arguments:
    // r:float, g:float, b:float - color at w_color_dist_far
    if (msg->type != CMT_SET_WFADE_COLOR_FAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#691" };
    }
    if (msg->num_args_received < 3)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade color far <r:float> <g:float> <b:float>" };
    }
    float arg0;
    float arg1;
    float arg2;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    cma_convert_to_float(&msg->args[2], &arg2);
    APP.settings.w_color_far = (Color){ (unsigned char)arg0, (unsigned char)arg1, (unsigned char)arg2, 255 };
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_enable(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // enabled:bool - whether or not to enable thickness fade
    if (msg->type != CMT_SET_WFADE_THICK_ENABLE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#713" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade thick <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP.settings.w_thick_fade_enabled = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_range(const ConsoleMsg* msg)
{
    // takes 2 arguments:
    // w-near:float - w-distance where thickness = w_thick_near
    // w-far:float  - w-distance where thickness = w_thick_far
    if (msg->type != CMT_SET_WFADE_THICK_RANGE)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#732" };
    }
    if (msg->num_args_received < 2)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade thick range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP.settings.w_thick_dist_near = arg0;
    APP.settings.w_thick_dist_far = arg1;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_segments(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // n:int - how many segments to split each line into
    if (msg->type != CMT_SET_WFADE_THICK_SEGMENTS)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#753" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade thick segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP.settings.w_thick_num_segments = (int)arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_near(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // t:float - thickness value at w_thick_dist_near
    if (msg->type != CMT_SET_WFADE_THICK_NEAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#771" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade thick near <t:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP.settings.w_thick_near = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_thick_far(const ConsoleMsg* msg)
{
    // takes 1 argument:
    // t:float - thickness value at w_thick_dist_far
    if (msg->type != CMT_SET_WFADE_THICK_FAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#921" };
    }
    if (msg->num_args_received < 1)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "wfade thick far <t:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP.settings.w_thick_far = arg0;
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
    console_gui_clear_autofill_entries();
    APP.console_gui_focused = false;

    atomic_store(&APP.is_exiting, 0);

    // init the console message handlers

    APP.cmh[CMT_QUIT]                       = cmh_quit;
    APP.cmh[CMT_PLAY]                       = cmh_play;
    APP.cmh[CMT_PAUSE]                      = cmh_pause;
    APP.cmh[CMT_RESET]                      = cmh_reset;

    APP.cmh[CMT_CHANGE_VIEW_2D]             = cmh_change_view_2d;
    APP.cmh[CMT_CHANGE_VIEW_3D]             = cmh_change_view_3d;

    APP.cmh[CMT_TOGGLE_AXES]                = cmh_toggle_axes;

    APP.cmh[CMT_TOGGLE_ROTATION_XY]         = cmh_toggle_rotation_xy;
    APP.cmh[CMT_TOGGLE_ROTATION_XZ]         = cmh_toggle_rotation_xz;
    APP.cmh[CMT_TOGGLE_ROTATION_XW]         = cmh_toggle_rotation_xw;
    APP.cmh[CMT_TOGGLE_ROTATION_YZ]         = cmh_toggle_rotation_yz;
    APP.cmh[CMT_TOGGLE_ROTATION_YW]         = cmh_toggle_rotation_yw;
    APP.cmh[CMT_TOGGLE_ROTATION_ZW]         = cmh_toggle_rotation_zw;

    APP.cmh[CMT_SET_MODEL_TETRA]            = cmh_set_model_tetra;
    APP.cmh[CMT_SET_MODEL_CUBE]             = cmh_set_model_cube;
    APP.cmh[CMT_SET_MODEL_OCT]              = cmh_set_model_oct;
    APP.cmh[CMT_SET_MODEL_ICO]              = cmh_set_model_ico;

    APP.cmh[CMT_SET_MODEL_5CELL]            = cmh_set_model_5cell;
    APP.cmh[CMT_SET_MODEL_8CELL]            = cmh_set_model_8cell;
    APP.cmh[CMT_SET_MODEL_16CELL]           = cmh_set_model_16cell;
    APP.cmh[CMT_SET_MODEL_24CELL]           = cmh_set_model_24cell;

    APP.cmh[CMT_SET_WFADE_ALPHA_ENABLE]     = cmh_set_wfade_alpha_enable;
    APP.cmh[CMT_SET_WFADE_ALPHA_RANGE]      = cmh_set_wfade_alpha_range;
    APP.cmh[CMT_SET_WFADE_ALPHA_SEGMENTS]   = cmh_set_wfade_alpha_segments;
    APP.cmh[CMT_SET_WFADE_ALPHA_NEAR]       = cmh_set_wfade_alpha_near;
    APP.cmh[CMT_SET_WFADE_ALPHA_FAR]        = cmh_set_wfade_alpha_far;

    APP.cmh[CMT_SET_WFADE_COLOR_ENABLE]     = cmh_set_wfade_color_enable;
    APP.cmh[CMT_SET_WFADE_COLOR_RANGE]      = cmh_set_wfade_color_range;
    APP.cmh[CMT_SET_WFADE_COLOR_SEGMENTS]   = cmh_set_wfade_color_segments;
    APP.cmh[CMT_SET_WFADE_COLOR_NEAR]       = cmh_set_wfade_color_near;
    APP.cmh[CMT_SET_WFADE_COLOR_FAR]        = cmh_set_wfade_color_far;

    APP.cmh[CMT_SET_WFADE_THICK_ENABLE]     = cmh_set_wfade_thick_enable;
    APP.cmh[CMT_SET_WFADE_THICK_RANGE]      = cmh_set_wfade_thick_range;
    APP.cmh[CMT_SET_WFADE_THICK_SEGMENTS]   = cmh_set_wfade_thick_segments;
    APP.cmh[CMT_SET_WFADE_THICK_NEAR]       = cmh_set_wfade_thick_near;
    APP.cmh[CMT_SET_WFADE_THICK_FAR]        = cmh_set_wfade_thick_far;

    cmq_init();

    // init all the rest of the simulation data to the defaults
    app_reset();
}

void app_reset()
{
    atomic_store(&APP.is_paused, 0);

    APP.view.axes.x = (Vector3){ 1, 0, 0 };
    APP.view.axes.y = (Vector3){ 0, 1, 0 };
    APP.view.axes.z = (Vector3){ 0, 0, 1 };
    APP.view.axes.w = Vector3Normalize((Vector3){1, 1, 1});

    APP.view.rotation.xy.enabled = false;
    APP.view.rotation.xy.speed = 0.33f;
    APP.view.rotation.xy.angle = 0.0f;
    APP.view.rotation.xz.enabled = false;
    APP.view.rotation.xz.speed = 0.33f;
    APP.view.rotation.xz.angle = 0.0f;
    APP.view.rotation.xw.enabled = false;
    APP.view.rotation.xw.speed = 0.18f;
    APP.view.rotation.xw.angle = 0.0f;
    APP.view.rotation.yz.enabled = false;
    APP.view.rotation.yz.speed = 0.33f;
    APP.view.rotation.yz.angle = 0.0f;
    APP.view.rotation.yw.enabled = false;
    APP.view.rotation.yw.speed = 0.18f;
    APP.view.rotation.yw.angle = 0.0f;
    APP.view.rotation.zw.enabled = false;
    APP.view.rotation.zw.speed = 0.18f;
    APP.view.rotation.zw.angle = 0.0f;

    APP.view.camera_control.mode = CM_3D_ORBITAL;
    APP.view.camera_control.orbital_yaw = -0.55f;
    APP.view.camera_control.orbital_pitch = 0.37f;
    APP.view.camera_control.orbital_radius = 9.0f;
    APP.view.camera_control.ortho_zoom = 4.0f;
    APP.view.camera_control.ortho_view_dir = (Vector3){0, 0, -1};

    APP.view.camera.fovy = 45.0f;
    APP.view.camera.projection = CAMERA_PERSPECTIVE;

    app_set_model(MT_CUBE);

    APP.settings = get_settings_max_fade();
}

void app_run()
{
    while (!WindowShouldClose() && !atomic_load(&APP.is_exiting)) {

        // update the camera
        if (APP.view.camera_control.mode == CM_2D_ORTHO && !APP.console_gui_focused)
        {
            update_camera_ortho();
        }
        else if (APP.view.camera_control.mode == CM_3D_ORBITAL && !APP.console_gui_focused)
        {
            update_camera_orbital();
        }

        // poll for keyinput
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
                console_gui_clear();
                console_gui_clear_autofill_entries();
                console_gui_refresh_autofill();
            }
            else
            {
                ConsoleMsg msg;
                if (console_parse_line(APP.console_gui_input, &msg))
                {
                    cmq_push(msg);
                }
                APP.console_gui_focused = false;
                console_gui_clear();
                console_gui_clear_autofill_entries();
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
            if (APP.view.rotation.xy.enabled) APP.view.rotation.xy.angle += APP.view.rotation.xy.speed * GetFrameTime();
            if (APP.view.rotation.xz.enabled) APP.view.rotation.xz.angle += APP.view.rotation.xz.speed * GetFrameTime();
            if (APP.view.rotation.xw.enabled) APP.view.rotation.xw.angle += APP.view.rotation.xw.speed * GetFrameTime();
            if (APP.view.rotation.yz.enabled) APP.view.rotation.yz.angle += APP.view.rotation.yz.speed * GetFrameTime();
            if (APP.view.rotation.yw.enabled) APP.view.rotation.yw.angle += APP.view.rotation.yw.speed * GetFrameTime();
            if (APP.view.rotation.zw.enabled) APP.view.rotation.zw.angle += APP.view.rotation.zw.speed * GetFrameTime();
        }

        Vector4 rotated[1024];
        for (int i = 0; i < APP.model.mesh.vertex_count; i += 1)
        {
            Vector4 v = APP.model.mesh.vertices[i];
            v = rotate_vector_in_plane(v, APP.view.rotation.xy.angle, 0, 1);
            v = rotate_vector_in_plane(v, APP.view.rotation.xz.angle, 0, 2);
            v = rotate_vector_in_plane(v, APP.view.rotation.xw.angle, 0, 3);
            v = rotate_vector_in_plane(v, APP.view.rotation.yz.angle, 1, 2);
            v = rotate_vector_in_plane(v, APP.view.rotation.yw.angle, 1, 3);
            v = rotate_vector_in_plane(v, APP.view.rotation.zw.angle, 2, 3);
            rotated[i] = v;
        }

        Vector4 x_axis = (Vector4){1,0,0,0};
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.xy.angle, 0, 1);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.xz.angle, 0, 2);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.xw.angle, 0, 3);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.yz.angle, 1, 2);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.yw.angle, 1, 3);
        x_axis = rotate_vector_in_plane(x_axis, APP.view.rotation.zw.angle, 2, 3);
        APP.model.axes.x = x_axis;

        Vector4 y_axis = (Vector4){0,1,0,0};
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.xy.angle, 0, 1);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.xz.angle, 0, 2);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.xw.angle, 0, 3);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.yz.angle, 1, 2);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.yw.angle, 1, 3);
        y_axis = rotate_vector_in_plane(y_axis, APP.view.rotation.zw.angle, 2, 3);
        APP.model.axes.y = y_axis;

        Vector4 z_axis = (Vector4){0,0,1,0};
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.xy.angle, 0, 1);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.xz.angle, 0, 2);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.xw.angle, 0, 3);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.yz.angle, 1, 2);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.yw.angle, 1, 3);
        z_axis = rotate_vector_in_plane(z_axis, APP.view.rotation.zw.angle, 2, 3);
        APP.model.axes.z = z_axis;

        Vector4 w_axis = (Vector4){0,0,0,1};
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.xy.angle, 0, 1);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.xz.angle, 0, 2);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.xw.angle, 0, 3);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.yz.angle, 1, 2);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.yw.angle, 1, 3);
        w_axis = rotate_vector_in_plane(w_axis, APP.view.rotation.zw.angle, 2, 3);
        APP.model.axes.w = w_axis;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(APP.view.camera);

        // draw xyzw axes
        if (APP.settings.world_axes_enabled)
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
        if (APP.settings.local_axes_enabled)
        {
            float scale_factor = 0.25f;

            // if (APP.view.rotation.xy.enabled || APP.view.rotation.xz.enabled || APP.view.rotation.xw.enabled)
            {
                Vector4 x_start_4d = { APP.model.axes.x.x, APP.model.axes.x.y, APP.model.axes.x.z, APP.model.axes.x.w };
                Vector4 x_end_4d   = { -x_start_4d.x, -x_start_4d.y, -x_start_4d.z, -x_start_4d.w };
                Vector3 x_start = Vector3Scale(projection4(x_start_4d), scale_factor);
                Vector3 x_end = Vector3Scale(projection4(x_end_4d), scale_factor);
                DrawCylinderEx(x_start, x_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_X_AXIS_COLOR);
                DrawSphere(x_start, ORIGIN_SIZE/3, LOCAL_X_AXIS_COLOR);
                DrawSphere(x_end, ORIGIN_SIZE/3, LOCAL_X_AXIS_COLOR);
            }

            // if (APP.view.rotation.xy.enabled || APP.view.rotation.yz.enabled || APP.view.rotation.yw.enabled)
            {
                Vector4 y_start_4d = { APP.model.axes.y.x, APP.model.axes.y.y, APP.model.axes.y.z, APP.model.axes.y.w };
                Vector4 y_end_4d   = { -y_start_4d.x, -y_start_4d.y, -y_start_4d.z, -y_start_4d.w };
                Vector3 y_start = Vector3Scale(projection4(y_start_4d), scale_factor);
                Vector3 y_end = Vector3Scale(projection4(y_end_4d), scale_factor);
                DrawCylinderEx(y_start, y_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Y_AXIS_COLOR);
                DrawSphere(y_start, ORIGIN_SIZE/3, LOCAL_Y_AXIS_COLOR);
                DrawSphere(y_end, ORIGIN_SIZE/3, LOCAL_Y_AXIS_COLOR);
            }

            // if (APP.view.rotation.xz.enabled || APP.view.rotation.yz.enabled || APP.view.rotation.zw.enabled)
            {
                Vector4 z_start_4d = { APP.model.axes.z.x, APP.model.axes.z.y, APP.model.axes.z.z, APP.model.axes.z.w };
                Vector4 z_end_4d   = { -z_start_4d.x, -z_start_4d.y, -z_start_4d.z, -z_start_4d.w };
                Vector3 z_start = Vector3Scale(projection4(z_start_4d), scale_factor);
                Vector3 z_end = Vector3Scale(projection4(z_end_4d), scale_factor);
                DrawCylinderEx(z_start, z_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_Z_AXIS_COLOR);
                DrawSphere(z_start, ORIGIN_SIZE/3, LOCAL_Z_AXIS_COLOR);
                DrawSphere(z_end, ORIGIN_SIZE/3, LOCAL_Z_AXIS_COLOR);
            }

            // if (APP.view.rotation.xw.enabled || APP.view.rotation.yw.enabled || APP.view.rotation.zw.enabled)
            {
                Vector4 w_start_4d = { APP.model.axes.w.x, APP.model.axes.w.y, APP.model.axes.w.z, APP.model.axes.w.w };
                Vector4 w_end_4d   = { -w_start_4d.x, -w_start_4d.y, -w_start_4d.z, -w_start_4d.w };
                Vector3 w_start = Vector3Scale(projection4(w_start_4d), scale_factor);
                Vector3 w_end = Vector3Scale(projection4(w_end_4d), scale_factor);
                DrawCylinderEx(w_start, w_end, AXIS_THICKNESS/2, AXIS_THICKNESS/2, AXIS_SEGMENTS, LOCAL_W_AXIS_COLOR);
                DrawSphere(w_start, ORIGIN_SIZE/3, LOCAL_W_AXIS_COLOR);
                DrawSphere(w_end, ORIGIN_SIZE/3, LOCAL_W_AXIS_COLOR);
            }

            DrawSphere((Vector3){0, 0, 0}, ORIGIN_SIZE/2, ORIGIN_COLOR);
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

        // draw the input window
        {
            Vector2 pos = {0, GetScreenHeight() - 90};
            float font_size = 50.0f;
            float row_height = 60.0f;
            float row_spacing = 10.0f;
            Vector2 prefix_size = MeasureTextEx(APP.ui_font, CONSOLE_MSG_PREFIX, font_size, 1.0f);
            Vector2 input_size = MeasureTextEx(APP.ui_font, APP.console_gui_input, font_size, 1.0f);
            float content_width = prefix_size.x + input_size.x;
            for (size_t i = 0; i < APP.console_gui_autofill_entries_len; i += 1)
            {
                Vector2 entry_size = MeasureTextEx(APP.ui_font, APP.console_gui_autofill_entries[i], font_size, 1.0f);
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
            Color border_color = (APP.console_gui_focused)
                ? (Color){180,180,180,255}
                : (Color){90,90,90,255};
            Color fill_color = (APP.console_gui_focused)
                ? (Color){120,120,120,255}
                : (Color){60,60,60,255};
            float extra_height = (row_height + row_spacing) * APP.console_gui_autofill_entries_len;
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

            for (size_t i = 0; i < APP.console_gui_autofill_entries_len; i += 1)
            {
                size_t entry_index = APP.console_gui_autofill_entries_len - 1 - i;
                float row_y = pos.y + 20 + (float)i * (row_height + row_spacing);
                DrawTextEx(APP.ui_font, APP.console_gui_autofill_entries[entry_index], (Vector2){pos.x+20+prefix_size.x+5, row_y}, font_size, 1.0f, DARKGRAY);
            }
            pos.y += extra_height;
            DrawTextEx(APP.ui_font, CONSOLE_MSG_PREFIX, (Vector2){pos.x+20, pos.y+20}, font_size, 1.0f, BLACK);
            DrawTextEx(APP.ui_font, APP.console_gui_input, (Vector2){pos.x+20+prefix_size.x+5, pos.y+20}, font_size, 1.0f, BLACK);
        }

        // draw debug information

#if DEBUG_CAMERA_COORDS

    if (APP.view.camera_control.mode == CM_3D_ORBITAL)
    {
        const char* camera_coord_text = TextFormat(
            "Camera:\n"
            "yaw: %f\n"
            "pitch: %f\n"
            "radius: %f\n",
            APP.view.camera_control.orbital_yaw,
            APP.view.camera_control.orbital_pitch,
            APP.view.camera_control.orbital_radius);
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
    _exit(0);
}

// -------------------------------- MODELS --------------------------------- //

void app_set_model(ModelType type)
{
    mesh4d_free(&APP.model.mesh);
    APP.model.type = type;

    APP.model.axes.x = (Vector4){ 1, 0, 0, 0 };
    APP.model.axes.y = (Vector4){ 0, 1, 0, 0 };
    APP.model.axes.z = (Vector4){ 0, 0, 1, 0 };
    APP.model.axes.w = (Vector4){ 0, 0, 0, 1 };

    // dont set rotation state in here, preserve it for comparing models

    switch(type)
    {
        case MT_TETRA:
            APP.model.mesh = mesh4d_new_tetra();
            break;
        case MT_CUBE:
            APP.model.mesh = mesh4d_new_cube();
            break;
        case MT_OCT:
            APP.model.mesh = mesh4d_new_oct();
            break;
        case MT_ICO:
            APP.model.mesh = mesh4d_new_ico();
            break;
        case MT_5CELL:
            APP.model.mesh = mesh4d_new_5cell();
            break;
        case MT_8CELL:
            APP.model.mesh = mesh4d_new_8cell();
            break;
        case MT_16CELL:
            APP.model.mesh = mesh4d_new_16cell();
            break;
        case MT_24CELL:
            APP.model.mesh = mesh4d_new_24cell();
            break;
        default:
            break;
    }
}

// --------------------------------- UTILS --------------------------------- //

void draw_line4(Vector4 p0, Vector4 p1)
{
    bool color_on = APP.settings.w_color_fade_enabled;
    bool alpha_on = APP.settings.w_alpha_fade_enabled;
    bool thick_on = APP.settings.w_thick_fade_enabled;
    Vector3 cam_pos = APP.view.camera.position;
    Color flat_color = APP.settings.w_color_near;

    // number of segments for the line
    // int n = 1;
    // if (color_on)
    // {
    //     n = APP.settings.w_color_num_segments;
    // }
    //
    // if (alpha_on && APP.settings.w_alpha_num_segments > n)
    // {
    //     n = APP.settings.w_alpha_num_segments;
    // }
    //
    // if (thick_on && APP.settings.w_thick_num_segments > n)
    // {
    //     n = APP.settings.w_thick_num_segments;
    // }

    // TODO make a setting for this, "wfade thick segments auto" +4color and alpha
    // TODO turn this 0.05 into a number that tracks the # of edges to speed up rendering
    // this one doesn't have to be a setting, (makes edges less accurate on 600cells)
    int n = (int) roundf(Vector4Distance(p0, p1) / 0.01f);

    float first_width = APP.settings.w_thick_near;
    Color first_color = flat_color;
    float last_width = APP.settings.w_thick_near;
    Color last_color = flat_color;

    if (!color_on && !alpha_on && !thick_on)
    {
        draw_line_segment4(p0, p1, APP.settings.w_thick_near, APP.settings.w_thick_near, cam_pos, flat_color);
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

            float width0 = thick_on ? w_to_thickness(seg_start.w) : APP.settings.w_thick_near;
            float width1 = thick_on ? w_to_thickness(seg_end.w)   : APP.settings.w_thick_near;

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
    char working[CONSOLE_MSG_LEN];
    strncpy(working, line, CONSOLE_MSG_LEN - 1);
    working[CONSOLE_MSG_LEN - 1] = '\0';

    char* tokens[3 + CONSOLE_MSG_MAX_ARG_COUNT];
    int token_count = 0;

    char* tok = strtok(working, " ");
    while (tok != NULL && token_count < 3 + CONSOLE_MSG_MAX_ARG_COUNT)
    {
        tokens[token_count] = tok;
        token_count += 1;
        tok = strtok(NULL, " ");
    }

    for (int t = 0; t < CMT_COUNT; t += 1)
    {
        ConsoleMsgSchema schema = get_console_msg_schema((ConsoleMsgType)t);

        if (token_count != schema.word_count + schema.arg_count)
        {
            continue;
        }

        bool words_match = true;
        for (int w = 0; w < schema.word_count; w += 1)
        {
            if (strcmp(tokens[w], schema.words[w]))
            {
                words_match = false;
                break;
            }
        }

        if (!words_match)
        {
            continue;
        }

        bool args_ok = true;
        ConsoleMsgArg parsed_args[CONSOLE_MSG_MAX_ARG_COUNT];

        for (int a = 0; a < schema.arg_count; a += 1)
        {
            const char* arg_str = tokens[schema.word_count + a];
            size_t arg_len = strlen(arg_str);

            if (schema.arg_types[a] == CMAT_BOOL)
            {
                bool value;
                if (!cma_try_parse_bool(arg_str, arg_len, &value))
                {
                    args_ok = false;
                    break;
                }
                parsed_args[a].type = CMAT_BOOL;
                parsed_args[a].get.bool_value = value;
            }
            else if (schema.arg_types[a] == CMAT_INT)
            {
                int64_t value;
                if (!cma_try_parse_int(arg_str, arg_len, &value))
                {
                    args_ok = false;
                    break;
                }
                parsed_args[a].type = CMAT_INT;
                parsed_args[a].get.int_value = value;
            }
            else if (schema.arg_types[a] == CMAT_FLOAT)
            {
                float value;
                if (!cma_try_parse_float(arg_str, arg_len, &value))
                {
                    args_ok = false;
                    break;
                }
                parsed_args[a].type = CMAT_FLOAT;
                parsed_args[a].get.float_value = value;
            }
        }

        if (!args_ok)
        {
            continue;
        }

        msg_out->type = schema.type;
        msg_out->num_args_received = schema.arg_count;
        for (int a = 0; a < schema.arg_count; a += 1)
        {
            msg_out->args[a] = parsed_args[a];
        }
        return true;
    }

    return false;
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

    console_gui_refresh_autofill();
}

void console_gui_backspace()
{
    if (APP.console_gui_input_len <= 0)
    {
        return;
    }

    APP.console_gui_input_len -= 1;
    APP.console_gui_input[APP.console_gui_input_len] = '\0';

    console_gui_refresh_autofill();
}

void console_gui_clear()
{
    APP.console_gui_input_len = 0;
    memset(APP.console_gui_input, 0, sizeof(char) * CONSOLE_MSG_LEN);
}

void console_gui_append_autofill_entry(const char* str)
{
    if (APP.console_gui_autofill_entries_len >= CONSOLE_GUI_AUTOFILL_ENTRY_COUNT - 1)
    {
        APP.console_gui_autofill_entries_len = CONSOLE_GUI_AUTOFILL_ENTRY_COUNT;
        return;
    }

    APP.console_gui_autofill_entries[APP.console_gui_autofill_entries_len] = str;
    APP.console_gui_autofill_entries_len += 1;
}

void console_gui_clear_autofill_entries()
{
    memset(APP.console_gui_autofill_entries, 0, sizeof(void*) * CONSOLE_GUI_AUTOFILL_ENTRY_COUNT);
    APP.console_gui_autofill_entries_len = 0;
}

void console_gui_refresh_autofill()
{
    // in reverse order as these entries will show up
    static const char* COMMANDS[] = {

        // static commands (known at compile time)
        "quit",
        "reset",

        "wfade thick far <t:float>",
        "wfade thick near <t:float>",
        "wfade thick segments <n:int>",
        "wfade thick range <w-near:float> <w-far:float>",
        "wfade thick <on|off>",

        "wfade color far <r:float> <g:float> <b:float>",
        "wfade color near <r:float> <g:float> <b:float>",
        "wfade color segments <n:int>",
        "wfade color range <w-near:float> <w-far:float>",
        "wfade color <on|off>",

        "wfade alpha far <a:float>",
        "wfade alpha near <a:float>",
        "wfade alpha segments <n:int>",
        "wfade alpha range <w-near:float> <w-far:float>",
        "wfade alpha <on|off>",

        "rotate zw",
        "rotate yw",
        "rotate yz",
        "rotate xw",
        "rotate xz",
        "rotate xy",

        "view 3d",
        "view 2d",

        // "axes world",
        // "axes model",
        // "axes none",
        // "axes all",
        "axes",

        "model 24cell",
        "model 16cell",
        "model 8cell",
        "model 5cell",

        "model ico",
        "model oct",
        "model cube",
        "model tetra",

        // generated commands
        NULL,
    };

    static int COMMAND_COUNT;
    COMMAND_COUNT = (sizeof(COMMANDS) / sizeof(COMMANDS[0]))
        - 1; // account for generated commands

    if (view4d_is_any_axis_rotating(&APP.view))
    {
        COMMAND_COUNT += 1;
        COMMANDS[COMMAND_COUNT - 1] = atomic_load(&APP.is_paused) ? "play" : "pause";
    }

    static char suggestion_buf[CONSOLE_GUI_AUTOFILL_ENTRY_COUNT][CONSOLE_MSG_LEN];

    console_gui_clear_autofill_entries();

    if (APP.console_gui_input_len == 0)
    {
        int suggestion_count = 0;

        for (int i = 0; i < COMMAND_COUNT; i += 1)
        {
            size_t first_word_len = 0;
            while (COMMANDS[i][first_word_len] != ' ' && COMMANDS[i][first_word_len] != '\0')
            {
                first_word_len += 1;
            }

            if (suggestion_count >= CONSOLE_GUI_AUTOFILL_ENTRY_COUNT)
            {
                break;
            }

            strncpy(suggestion_buf[suggestion_count], COMMANDS[i], first_word_len);
            suggestion_buf[suggestion_count][first_word_len] = '\0';

            bool already_present = false;
            for (int j = 0; j < suggestion_count; j += 1)
            {
                if (!strcmp(suggestion_buf[j], suggestion_buf[suggestion_count]))
                {
                    already_present = true;
                    break;
                }
            }

            if (!already_present)
            {
                console_gui_append_autofill_entry(suggestion_buf[suggestion_count]);
                suggestion_count += 1;
            }
        }

        return;
    }

    size_t input_len = APP.console_gui_input_len;
    while (input_len > 0 && APP.console_gui_input[input_len - 1] == ' ')
    {
        input_len -= 1;
    }
    if (input_len == 0)
    {
        return;
    }
    int suggestion_count = 0;
    for (int i = 0; i < COMMAND_COUNT; i += 1)
    {
        if (strncmp(COMMANDS[i], APP.console_gui_input, input_len))
        {
            continue;
        }
        size_t word_end = input_len;
        while (COMMANDS[i][word_end] != ' ' && COMMANDS[i][word_end] != '\0')
        {
            word_end += 1;
        }
        if (word_end == input_len && COMMANDS[i][word_end] == '\0')
        {
            continue;
        }
        size_t next_word_end = word_end;
        if (COMMANDS[i][next_word_end] == ' ')
        {
            next_word_end += 1;
            while (COMMANDS[i][next_word_end] != ' ' && COMMANDS[i][next_word_end] != '\0')
            {
                next_word_end += 1;
            }
        }
        while (COMMANDS[i][next_word_end] == ' ' && COMMANDS[i][next_word_end + 1] == '<')
        {
            bool first_word_is_placeholder = false;
            size_t check = word_end;
            if (COMMANDS[i][check] == ' ')
            {
                check += 1;
            }
            if (COMMANDS[i][check] == '<')
            {
                first_word_is_placeholder = true;
            }
            if (!first_word_is_placeholder)
            {
                break;
            }
            next_word_end += 1;
            while (COMMANDS[i][next_word_end] != ' ' && COMMANDS[i][next_word_end] != '\0')
            {
                next_word_end += 1;
            }
        }
        if (suggestion_count >= CONSOLE_GUI_AUTOFILL_ENTRY_COUNT)
        {
            break;
        }
        strncpy(suggestion_buf[suggestion_count], COMMANDS[i], next_word_end);
        suggestion_buf[suggestion_count][next_word_end] = '\0';
        bool already_present = false;
        for (int j = 0; j < suggestion_count; j += 1)
        {
            if (!strcmp(suggestion_buf[j], suggestion_buf[suggestion_count]))
            {
                already_present = true;
                break;
            }
        }
        if (!already_present)
        {
            console_gui_append_autofill_entry(suggestion_buf[suggestion_count]);
            suggestion_count += 1;
        }
    }
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
    toggle_rotation_plane(&APP.view.rotation.xy.enabled,
        &APP.view.rotation.xz.enabled, &APP.view.rotation.xw.enabled,
        &APP.view.rotation.yz.enabled, &APP.view.rotation.yw.enabled);
}

void toggle_rotation_xz()
{
    toggle_rotation_plane(&APP.view.rotation.xz.enabled,
        &APP.view.rotation.xy.enabled, &APP.view.rotation.xw.enabled,
        &APP.view.rotation.yz.enabled, &APP.view.rotation.zw.enabled);
}

void toggle_rotation_xw()
{
    toggle_rotation_plane(&APP.view.rotation.xw.enabled,
        &APP.view.rotation.xy.enabled, &APP.view.rotation.xz.enabled,
        &APP.view.rotation.yw.enabled, &APP.view.rotation.zw.enabled);
}

void toggle_rotation_yz()
{
    toggle_rotation_plane(&APP.view.rotation.yz.enabled,
        &APP.view.rotation.xy.enabled, &APP.view.rotation.xz.enabled,
        &APP.view.rotation.yw.enabled, &APP.view.rotation.zw.enabled);
}

void toggle_rotation_yw()
{
    toggle_rotation_plane(&APP.view.rotation.yw.enabled,
        &APP.view.rotation.xy.enabled, &APP.view.rotation.xw.enabled,
        &APP.view.rotation.yz.enabled, &APP.view.rotation.zw.enabled);
}

void toggle_rotation_zw()
{
    toggle_rotation_plane(&APP.view.rotation.zw.enabled,
        &APP.view.rotation.xz.enabled, &APP.view.rotation.xw.enabled,
        &APP.view.rotation.yz.enabled, &APP.view.rotation.yw.enabled);
}
