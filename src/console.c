#include "console.h"
#include "visualizer.h"

// ------------------------------- CONSOLE --------------------------------- //

void cmq_init()
{
    APP->cmq.size = 0;
}

bool cmq_push(ConsoleMsg msg)
{
    bool ok = false;

    if (APP->cmq.size < CONSOLE_MSG_MAX_QUEUE_SIZE) {
        APP->cmq.messages[APP->cmq.size] = msg;
        APP->cmq.size += 1;
        ok = true;
    }

    return ok;
}

bool cmq_pop(ConsoleMsg *msg_out)
{
    bool got = false;

    if (APP->cmq.size > 0) {
        *msg_out = APP->cmq.messages[0];

        // shift everything down by one so the front of the queue is always at [0]
        for (int i = 1; i < APP->cmq.size; i += 1) {
            APP->cmq.messages[i - 1] = APP->cmq.messages[i];
        }
        APP->cmq.size -= 1;
        got = true;
    }

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

        [CMT_CHANGE_BGCOLOR] = { CMT_CHANGE_BGCOLOR, { "bgcolor" }, 1, { CMAT_INT, CMAT_INT, CMAT_INT }, 3 },

        [CMT_TOGGLE_AXES] = { CMT_TOGGLE_AXES, { "axes" }, 1, { 0 }, 0 },
        [CMT_TOGGLE_HUD] = { CMT_TOGGLE_HUD, { "hud" }, 1, { 0 }, 0 },

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

        [CMT_SET_WFADE_ALPHA_ENABLE]   = { CMT_SET_WFADE_ALPHA_ENABLE,   { "fade", "alpha" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_ALPHA_RANGE]    = { CMT_SET_WFADE_ALPHA_RANGE,    { "fade", "alpha", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_ALPHA_SEGMENTS] = { CMT_SET_WFADE_ALPHA_SEGMENTS, { "fade", "alpha", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_ALPHA_NEAR]     = { CMT_SET_WFADE_ALPHA_NEAR,     { "fade", "alpha", "near" },     3, { CMAT_FLOAT },                         1 },
        [CMT_SET_WFADE_ALPHA_FAR]      = { CMT_SET_WFADE_ALPHA_FAR,      { "fade", "alpha", "far" },      3, { CMAT_FLOAT },                         1 },

        [CMT_SET_WFADE_COLOR_ENABLE]   = { CMT_SET_WFADE_COLOR_ENABLE,   { "fade", "color" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_COLOR_RANGE]    = { CMT_SET_WFADE_COLOR_RANGE,    { "fade", "color", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_COLOR_SEGMENTS] = { CMT_SET_WFADE_COLOR_SEGMENTS, { "fade", "color", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_COLOR_NEAR]     = { CMT_SET_WFADE_COLOR_NEAR,     { "fade", "color", "near" },     3, { CMAT_INT, CMAT_INT, CMAT_INT },       3 },
        [CMT_SET_WFADE_COLOR_FAR]      = { CMT_SET_WFADE_COLOR_FAR,      { "fade", "color", "far" },      3, { CMAT_INT, CMAT_INT, CMAT_INT },       3 },

        [CMT_SET_WFADE_THICK_ENABLE]   = { CMT_SET_WFADE_THICK_ENABLE,   { "fade", "thick" },             2, { CMAT_BOOL },                          1 },
        [CMT_SET_WFADE_THICK_RANGE]    = { CMT_SET_WFADE_THICK_RANGE,    { "fade", "thick", "range" },    3, { CMAT_FLOAT, CMAT_FLOAT },             2 },
        [CMT_SET_WFADE_THICK_SEGMENTS] = { CMT_SET_WFADE_THICK_SEGMENTS, { "fade", "thick", "segments" }, 3, { CMAT_INT },                           1 },
        [CMT_SET_WFADE_THICK_NEAR]     = { CMT_SET_WFADE_THICK_NEAR,     { "fade", "thick", "near" },     3, { CMAT_FLOAT },                         1 },
        [CMT_SET_WFADE_THICK_FAR]      = { CMT_SET_WFADE_THICK_FAR,      { "fade", "thick", "far" },      3, { CMAT_FLOAT },                         1 },
    };

    return SCHEMAS[type];
}

// ----------------------- CONSOLE MESSAGE HANDLERS ------------------------ //

ConsoleMsgResult cmh_quit(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    APP->is_exiting = true;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_play(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    APP->is_paused = false;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_pause(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    APP->is_paused = true;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_reset(const ConsoleMsg* msg)
{
    // takes 0 arguments, ignores all that are given
    app_reset();
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_change_bgcolor(const ConsoleMsg* msg)
{
    // takes 3 arguments:
    // r:int, g:int, b:int - color at w_color_dist_near
    if (msg->type != CMT_CHANGE_BGCOLOR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#893" };
    }
    if (msg->num_args_received < 3)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "bgcolor <r:int> <g:int> <b:int>" };
    }
    int64_t arg0;
    int64_t arg1;
    int64_t arg2;
    cma_convert_to_int(&msg->args[0], &arg0);
    cma_convert_to_int(&msg->args[1], &arg1);
    cma_convert_to_int(&msg->args[2], &arg2);
    APP->settings.background_color = (Color){ (unsigned char)arg0, (unsigned char)arg1, (unsigned char)arg2, 255 };
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_change_view_2d(const ConsoleMsg* msg)
{
    if (msg->type != CMT_CHANGE_VIEW_2D)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#726" };
    }
    APP->view.camera_control.mode = CM_2D_ORTHO;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_change_view_3d(const ConsoleMsg* msg)
{
    if (msg->type != CMT_CHANGE_VIEW_3D)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#719" };
    }
    APP->view.camera_control.mode = CM_3D_ORBITAL;
    APP->view.camera.projection = CAMERA_PERSPECTIVE;
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
    //         APP->settings.world_axes_enabled = false;
    //         APP->settings.local_axes_enabled = true;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "world", 5))
    //     {
    //         APP->settings.world_axes_enabled = true;
    //         APP->settings.local_axes_enabled = false;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "none", 4))
    //     {
    //         APP->settings.world_axes_enabled = false;
    //         APP->settings.local_axes_enabled = false;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    //     else if (!strncasecmp(str, "all", 3))
    //     {
    //         APP->settings.world_axes_enabled = true;
    //         APP->settings.local_axes_enabled = true;
    //         return (ConsoleMsgResult){ CMRT_OK, "" };
    //     }
    // }

    bool* world = &APP->settings.world_axes_enabled;
    bool* local = &APP->settings.local_axes_enabled;

    if (!*world && !*local)       { *world = true;  *local = false; }
    else if (*world && !*local)   { *world = false; *local = true;  }
    else if (!*world && *local)   { *world = true;  *local = true;  }
    else                          { *world = false; *local = false; }

    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_toggle_hud(const ConsoleMsg* msg)
{
    if (msg->type != CMT_TOGGLE_HUD)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#950" };
    }
    APP->settings.hud_enabled = !APP->settings.hud_enabled;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade alpha <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP->settings.w_alpha_fade_enabled = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade alpha range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP->settings.w_alpha_dist_near = arg0;
    APP->settings.w_alpha_dist_far = arg1;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade alpha segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP->settings.w_alpha_num_segments = (int)arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade alpha near <a:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP->settings.w_alpha_near = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade alpha far <a:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP->settings.w_alpha_far = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade color <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP->settings.w_color_fade_enabled = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade color range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP->settings.w_color_dist_near = arg0;
    APP->settings.w_color_dist_far = arg1;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade color segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP->settings.w_color_num_segments = (int)arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_near(const ConsoleMsg* msg)
{
    // takes 3 arguments:
    // r:int, g:int, b:int - color at w_color_dist_near
    if (msg->type != CMT_SET_WFADE_COLOR_NEAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#669" };
    }
    if (msg->num_args_received < 3)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade color near <r:int> <g:int> <b:int>" };
    }
    int64_t arg0;
    int64_t arg1;
    int64_t arg2;
    cma_convert_to_int(&msg->args[0], &arg0);
    cma_convert_to_int(&msg->args[1], &arg1);
    cma_convert_to_int(&msg->args[2], &arg2);
    APP->settings.w_color_near = (Color){ (unsigned char)arg0, (unsigned char)arg1, (unsigned char)arg2, 255 };
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

ConsoleMsgResult cmh_set_wfade_color_far(const ConsoleMsg* msg)
{
    // takes 3 arguments:
    // r:int, g:int, b:int - color at w_color_dist_far
    if (msg->type != CMT_SET_WFADE_COLOR_FAR)
    {
        return (ConsoleMsgResult){ CMRT_INTERNAL_ERROR, "L#691" };
    }
    if (msg->num_args_received < 3)
    {
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade color far <r:int> <g:int> <b:int>" };
    }
    int64_t arg0;
    int64_t arg1;
    int64_t arg2;
    cma_convert_to_int(&msg->args[0], &arg0);
    cma_convert_to_int(&msg->args[1], &arg1);
    cma_convert_to_int(&msg->args[2], &arg2);
    APP->settings.w_color_far = (Color){ (unsigned char)arg0, (unsigned char)arg1, (unsigned char)arg2, 255 };
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade thick <enabled:bool>" };
    }
    bool arg0;
    cma_convert_to_bool(&msg->args[0], &arg0);
    APP->settings.w_thick_fade_enabled = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade thick range <w-near:float> <w-far:float>" };
    }
    float arg0;
    float arg1;
    cma_convert_to_float(&msg->args[0], &arg0);
    cma_convert_to_float(&msg->args[1], &arg1);
    APP->settings.w_thick_dist_near = arg0;
    APP->settings.w_thick_dist_far = arg1;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade thick segments <n:int>" };
    }
    int64_t arg0;
    cma_convert_to_int(&msg->args[0], &arg0);
    APP->settings.w_thick_num_segments = (int)arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade thick near <t:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP->settings.w_thick_near = arg0;
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
        return (ConsoleMsgResult){ CMRT_PARSE_ERROR, "fade thick far <t:float>" };
    }
    float arg0;
    cma_convert_to_float(&msg->args[0], &arg0);
    APP->settings.w_thick_far = arg0;
    return (ConsoleMsgResult){ CMRT_OK, "" };
}

// utils

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
            if (strcasecmp(tokens[w], schema.words[w]))
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
    if (APP->console_gui_input_len >= CONSOLE_MSG_LEN - 1)
    {
        APP->console_gui_input_len = CONSOLE_MSG_LEN - 1;
        return;
    }

    APP->console_gui_input[APP->console_gui_input_len] = c;
    APP->console_gui_input_len += 1;
    APP->console_gui_input[APP->console_gui_input_len] = '\0';

    console_gui_refresh_autofill();
}

void console_gui_backspace()
{
    if (APP->console_gui_input_len <= 0)
    {
        return;
    }

    APP->console_gui_input_len -= 1;
    APP->console_gui_input[APP->console_gui_input_len] = '\0';

    console_gui_refresh_autofill();
}

void console_gui_clear()
{
    APP->console_gui_input_len = 0;
    memset(APP->console_gui_input, 0, sizeof(char) * CONSOLE_MSG_LEN);
}

void console_gui_append_autofill_entry(const char* str)
{
    if (APP->console_gui_autofill_entries_len >= CONSOLE_GUI_AUTOFILL_ENTRY_COUNT - 1)
    {
        APP->console_gui_autofill_entries_len = CONSOLE_GUI_AUTOFILL_ENTRY_COUNT;
        return;
    }

    APP->console_gui_autofill_entries[APP->console_gui_autofill_entries_len] = str;
    APP->console_gui_autofill_entries_len += 1;
}

void console_gui_clear_autofill_entries()
{
    memset(APP->console_gui_autofill_entries, 0, sizeof(void*) * CONSOLE_GUI_AUTOFILL_ENTRY_COUNT);
    APP->console_gui_autofill_entries_len = 0;
}

void console_gui_refresh_autofill()
{
    // in reverse order as these entries will show up
    static const char* COMMANDS[] = {

        // static commands (known at compile time)
        "quit",
        "reset",

        "fade thick far <t:float>",
        "fade thick near <t:float>",
        "fade thick segments <n:int>",
        "fade thick range <w-near:float> <w-far:float>",
        "fade thick <on|off>",

        "fade color far <r:int> <g:int> <b:int>",
        "fade color near <r:int> <g:int> <b:int>",
        "fade color segments <n:int>",
        "fade color range <w-near:float> <w-far:float>",
        "fade color <on|off>",

        "fade alpha far <a:float>",
        "fade alpha near <a:float>",
        "fade alpha segments <n:int>",
        "fade alpha range <w-near:float> <w-far:float>",
        "fade alpha <on|off>",

        "rotate zw",
        "rotate yw",
        "rotate yz",
        "rotate xw",
        "rotate xz",
        "rotate xy",

        "model 24cell",
        "model 16cell",
        "model 8cell",
        "model 5cell",

        "model ico",
        "model oct",
        "model cube",
        "model tetra",

        "view 3d",
        "view 2d",

        "bgcolor <r:int> <g:int> <b:int>",

        // "axes world",
        // "axes model",
        // "axes none",
        // "axes all",
        "axes",
        "hud",

        // generated commands
        NULL,
    };

    static int COMMAND_COUNT;
    COMMAND_COUNT = (sizeof(COMMANDS) / sizeof(COMMANDS[0]))
    - 1; // account for generated commands, increment this when adding any

    if (view4d_is_any_axis_rotating(&APP->view))
    {
        COMMAND_COUNT += 1;
        COMMANDS[COMMAND_COUNT - 1] = APP->is_paused ? "play" : "pause";
    }

    static char suggestion_buf[CONSOLE_GUI_AUTOFILL_ENTRY_COUNT][CONSOLE_MSG_LEN];

    console_gui_clear_autofill_entries();

    if (APP->console_gui_input_len == 0)
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
                if (!strcasecmp(suggestion_buf[j], suggestion_buf[suggestion_count]))
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

    size_t input_len = APP->console_gui_input_len;
    while (input_len > 0 && APP->console_gui_input[input_len - 1] == ' ')
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
        if (strncasecmp(COMMANDS[i], APP->console_gui_input, input_len))
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
            if (!strcasecmp(suggestion_buf[j], suggestion_buf[suggestion_count]))
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
