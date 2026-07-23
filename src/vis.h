#ifndef VIS_H
#define VIS_H

// TODO make windows compliant

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

// ------------------------------ PARAMETERS ------------------------------- //

// prefix to use for console commands
#define CONSOLE_MSG_PREFIX "> "

// maximum length of a console command
#define CONSOLE_MSG_LEN 64

// how many arguments are allowed in a console command
#define CONSOLE_MSG_MAX_ARG_COUNT 10

// how many console commands can be queued at once
#define CONSOLE_MSG_MAX_QUEUE_SIZE 10

// how many lines there can be in the console + 1 basically
#define CONSOLE_GUI_AUTOFILL_ENTRY_COUNT 10

// how long the x,y,z,w axes extend out from the origin
#define AXIS_MAGNITUDE 1000

// the thickness of the axes
#define AXIS_THICKNESS 0.03f

// how many segments to draw the axis cylinders
#define AXIS_SEGMENTS 16

// how many segments to draw a cylinder of a mesh
#define MESH_CYLINDER_SEGMENTS 16

// the axis colors
#define WORLD_X_AXIS_COLOR ((Color){ 255,  60,  60, 255 })
#define WORLD_Y_AXIS_COLOR ((Color){  60,  120, 255, 255 })
#define WORLD_Z_AXIS_COLOR ((Color){  60, 220,  60, 255 })
#define WORLD_W_AXIS_COLOR ((Color){ 230, 230,   0, 255 })

#define LOCAL_X_AXIS_COLOR ((Color){ 120,  25,  25, 255 })
#define LOCAL_Y_AXIS_COLOR ((Color){  15,  40, 160, 255 })
#define LOCAL_Z_AXIS_COLOR ((Color){  30, 110,  30, 255 })
#define LOCAL_W_AXIS_COLOR ((Color){ 110, 110,   0, 255 })

// the size of the origin sphere
#define ORIGIN_SIZE 0.15f

// the color of the origin sphere
#define ORIGIN_COLOR RAYWHITE

// orthographic 2d mode zoom (z-coordinates)
#define ORTHO_ZOOM_MIN 0.0001f
#define ORTHO_ZOOM_MAX 20.0f

// gui color scheme

#define GUI_COLOR_BORDER_LIGHT ((Color){180,180,180,255})
#define GUI_COLOR_BORDER_DARK ((Color){90,90,90,255})
#define GUI_COLOR_FILL_LIGHT ((Color){120,120,120,255})
#define GUI_COLOR_FILL_DARK ((Color){60,60,60,255})

// ------------------------------ MODEL TYPES ------------------------------ //

typedef enum {
    MT_NONE,

    MT_TETRA,
    MT_CUBE,
    MT_OCT,
    MT_ICO,

    MT_5CELL,
    MT_8CELL,
    MT_16CELL,
    MT_24CELL,
    // MT_120CELL,
    // MT_600CELL,
    // ...

    MT_COUNT
} ModelType;

// -------------------------------- 4D MESH -------------------------------- //

typedef struct{
    Vector4* vertices;
    int vertex_count;
    int (*edges)[2];
    int edge_count;
    float radius;
} Mesh4D;

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                const int (*edges)[2], int edge_count,
                float radius);

Mesh4D mesh4d_new_empty();

// 3d shapes - w=0
Mesh4D mesh4d_new_tetra();
Mesh4D mesh4d_new_cube();
Mesh4D mesh4d_new_oct();

// 4d shapes
Mesh4D mesh4d_new_5cell();
Mesh4D mesh4d_new_8cell();
Mesh4D mesh4d_new_16cell();
Mesh4D mesh4d_new_24cell();

void mesh4d_free(Mesh4D* mesh);

// -------------------------------- 4D AXES -------------------------------- //

// where the 4 local axes point in 4d space
typedef struct {
    Vector4 x, y, z, w;
} Axes4D;

// where the 4 world axes point after being projected down to 3d space
typedef struct {
    Vector3 x, y, z, w;
} ProjectionAxes4D;

// ------------------------------- 4D CAMERA ------------------------------- //

typedef enum {
    CM_2D_ORTHO,    // you're locked to the z-axis which is hidden
    CM_3D_ORBITAL,
} CameraMode4D;

typedef struct {
    CameraMode4D mode;
    // CM_2D_ORTHO
    Vector3 ortho_view_dir;
    Vector2 ortho_pan;
    float ortho_zoom;
    // CM_3D_ORBITAL
    float orbital_yaw;
    float orbital_pitch;
    float orbital_radius;
} Camera4D;

void update_camera_ortho();
void update_camera_orbital();

// -------------------------------- 4D VIEW -------------------------------- //

// stores information about the rotation of a single plane of rotation
typedef struct {
    bool enabled;
    float angle;
    float speed;
} RotationPlane4D;

// stores information about all 6 planes of rotation
typedef struct {
    RotationPlane4D xy, xz, xw, yz, yw, zw;
} Rotation4D;

typedef struct
{
    ProjectionAxes4D axes;
    Rotation4D rotation;
    Camera3D camera;
    Camera4D camera_control;
} View4D;

bool view4d_is_any_axis_rotating(const View4D* v);

// ------------------------------- SETTINGS -------------------------------- //

typedef struct {

    // console settings

    // simulation settings

    // which camera mode to display the simulation in
    CameraMode4D camera_mode;

    // rendering settings
    bool world_axes_enabled;     // enable/disable world axes
    bool local_axes_enabled;     // enable/disable local axes for the model

    // fade alpha linearly along the w-axis (further = more transparent)
    bool w_alpha_fade_enabled;   // enable/disable alpha fade
    int w_alpha_num_segments;    // how many segments to split each line into
    float w_alpha_dist_near;     // w-distance where alpha = w_alpha_near
    float w_alpha_dist_far;      // w-distance where alpha = w_alpha_far
    float w_alpha_near;          // alpha value at w_alpha_dist_near
    float w_alpha_far;           // alpha value at w_alpha_dist_far

    // interpolate color linearly along the w-axis (further = towards color_far)
    bool w_color_fade_enabled;   // enable/disable color fade
    int w_color_num_segments;    // how many segments to split each line into
    float w_color_dist_near;     // w-distance where color = w_color_near
    float w_color_dist_far;      // w-distance where color = w_color_far
    Color w_color_near;          // color at w_color_dist_near
    Color w_color_far;           // color at w_color_dist_far

    // fade thickness linearly along the w-axis (further = thinner)
    bool w_thick_fade_enabled;   // enable/disable thickness fade
    int w_thick_num_segments;    // how many segments to split each line into
    float w_thick_dist_near;     // w-distance where thickness = w_thick_near
    float w_thick_dist_far;      // w-distance where thickness = w_thick_far
    float w_thick_near;          // thickness value at w_thick_dist_near
    float w_thick_far;           // thickness value at w_thick_dist_far

} Settings;

// get different presets of the settings
Settings get_settings_default();    // experimental defaults
Settings get_settings_no_fade();    // no color or alpha fading, no w-depth cues
Settings get_settings_max_fade();   // strong w-depth cues

// ------------------------------- CONSOLE --------------------------------- //

typedef enum {

    // commands to change application state

    // quit
    CMT_QUIT,

    // play and pause the simulation
    CMT_PLAY,
    CMT_PAUSE,

    // reset
    CMT_RESET,

    // simulation settings

    // view <2d|3d>
    CMT_CHANGE_VIEW_2D,
    CMT_CHANGE_VIEW_3D,

    // axes
    CMT_TOGGLE_AXES,

    // rotate <plane0:string> [plane1:string]
    CMT_TOGGLE_ROTATION_XY,
    CMT_TOGGLE_ROTATION_XZ,
    CMT_TOGGLE_ROTATION_XW,
    CMT_TOGGLE_ROTATION_YZ,
    CMT_TOGGLE_ROTATION_YW,
    CMT_TOGGLE_ROTATION_ZW,

    // model <model:string>
    CMT_SET_MODEL_TETRA,
    CMT_SET_MODEL_CUBE,
    CMT_SET_MODEL_OCT,
    CMT_SET_MODEL_ICO,

    CMT_SET_MODEL_5CELL,
    CMT_SET_MODEL_8CELL,
    CMT_SET_MODEL_16CELL,
    CMT_SET_MODEL_24CELL,

    // rendering settings

    // wfade alpha <enabled:bool>
    CMT_SET_WFADE_ALPHA_ENABLE,
    // wfade alpha range <w-near:float> <w-far:float>
    CMT_SET_WFADE_ALPHA_RANGE,
    // wfade alpha segments <n:int>
    CMT_SET_WFADE_ALPHA_SEGMENTS,
    // wfade alpha near <a:float>
    CMT_SET_WFADE_ALPHA_NEAR,
    // wfade alpha far <a:float>
    CMT_SET_WFADE_ALPHA_FAR,

    // wfade color <enabled:bool>
    CMT_SET_WFADE_COLOR_ENABLE,
    // wfade color range <w-near:float> <w-far:float>
    CMT_SET_WFADE_COLOR_RANGE,
    // wfade color segments <n:int>
    CMT_SET_WFADE_COLOR_SEGMENTS,
    // wfade color near <r:float> <g:float> <b:float>
    CMT_SET_WFADE_COLOR_NEAR,
    // wfade color far <r:float> <g:float> <b:float>
    CMT_SET_WFADE_COLOR_FAR,

    // wfade thick <enabled:bool>
    CMT_SET_WFADE_THICK_ENABLE,
    // wfade thick range <w-near:float> <w-far:float>
    CMT_SET_WFADE_THICK_RANGE,
    // wfade thick segments <n:int>
    CMT_SET_WFADE_THICK_SEGMENTS,
    // wfade thick near <t:float>
    CMT_SET_WFADE_THICK_NEAR,
    // wfade thick far <t:float>
    CMT_SET_WFADE_THICK_FAR,

    // ...

    CMT_COUNT
} ConsoleMsgType;

// ----------------------- CONSOLE MESSAGE ARGUMENTS ----------------------- //

// TODO fix all bugs related to argument parsing
// parse different types of console command arguments
// order of priority is int > float > bool > string
typedef enum {
    CMAT_INT,   // int64_t
    CMAT_FLOAT, // float
    CMAT_BOOL,  // bool
    CMAT_STRING,// char[64]
} ConsoleMsgArgType;

typedef struct {
    ConsoleMsgArgType type;
    union {
        bool bool_value;
        int64_t int_value;
        float float_value;
        char string_value[CONSOLE_MSG_LEN];
    } get;
} ConsoleMsgArg;

// parse as a specific type depending on what's needed
void cma_convert_to_bool(const ConsoleMsgArg* arg, bool* out);
void cma_convert_to_int(const ConsoleMsgArg* arg, int64_t* out);
void cma_convert_to_float(const ConsoleMsgArg* arg, float* out);
void cma_convert_to_string(const ConsoleMsgArg* arg, char* out);

// return the most strictly matched type
ConsoleMsgArgType cma_parse(const char* str, size_t len, bool* b_out, int64_t* i_out, float* f_out, char* s_out);
bool cma_try_parse_bool(const char* str, size_t len, bool* out);
bool cma_try_parse_int(const char* str, size_t len, int64_t* out);
bool cma_try_parse_float(const char* str, size_t len, float* out);
bool cma_try_parse_string(const char* str, size_t len, char* out);

// perform all possible type conversions - type errors don't exist here
void cmatc_int_to_float(int64_t in, float* out);
void cmatc_int_to_bool(int64_t in, bool* out);
void cmatc_int_to_string(int64_t in, char* out);
void cmatc_float_to_int(float in, int64_t* out);
void cmatc_float_to_bool(float in, bool* out);
void cmatc_float_to_string(float in, char* out);
void cmatc_bool_to_int(bool in, int64_t* out);
void cmatc_bool_to_float(bool in, float* out);
void cmatc_bool_to_string(bool in, char* out);
void cmatc_string_to_int(char* in, int64_t* out);
void cmatc_string_to_float(char* in, float* out);
void cmatc_string_to_bool(char* in, bool* out);

typedef struct {
    ConsoleMsgType type;
    ConsoleMsgArg args[CONSOLE_MSG_MAX_ARG_COUNT];
    size_t num_args_received;
} ConsoleMsg;

typedef struct {
    ConsoleMsg messages[CONSOLE_MSG_MAX_QUEUE_SIZE];
    int size;
} ConsoleMsgQueue;

// initialize the message queue to empty
void cmq_init();

// push a console message to the queue
// returns false if the message got dropped because the queue is full
bool cmq_push(ConsoleMsg msg);

// pop a console message from the queue
// returns false if no messages are in the queue
bool cmq_pop(ConsoleMsg* msg_out);

// ------------------------ CONSOLE MESSAGE SCHEMAS ------------------------ //

typedef struct
{
    ConsoleMsgType type;
    const char* words[6];
    int word_count;
    ConsoleMsgArgType arg_types[CONSOLE_MSG_MAX_ARG_COUNT];
    int arg_count;
} ConsoleMsgSchema;

ConsoleMsgSchema get_console_msg_schema(ConsoleMsgType type);

// ----------------------- CONSOLE MESSAGE HANDLERS ------------------------ //

typedef enum {
    CMRT_OK,
    CMRT_PARSE_ERROR,
    CMRT_INTERNAL_ERROR
} ConsoleMsgResultType;

typedef struct {
    ConsoleMsgResultType type;
    const char* errmsg; // should be "" if CMRT_OK
} ConsoleMsgResult;

// a function type that executes a console message
typedef ConsoleMsgResult (*ConsoleMsgHandler)(const ConsoleMsg*);

// console message handler implementations for all message types
ConsoleMsgResult cmh_quit(const ConsoleMsg* msg);                       // CMT_QUIT
ConsoleMsgResult cmh_play(const ConsoleMsg* msg);                       // CMT_PLAY
ConsoleMsgResult cmh_pause(const ConsoleMsg* msg);                      // CMT_PAUSE

ConsoleMsgResult cmh_reset(const ConsoleMsg* msg);                      // CMT_RESET

ConsoleMsgResult cmh_change_view_2d(const ConsoleMsg* msg);             // CMT_CHANGE_VIEW_2D
ConsoleMsgResult cmh_change_view_3d(const ConsoleMsg* msg);             // CMT_CHANGE_VIEW_3D

ConsoleMsgResult cmh_set_axes_enabled(const ConsoleMsg* msg);           // CMT_SET_AXES_ENABLED

ConsoleMsgResult cmh_toggle_rotation_xy(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_XY
ConsoleMsgResult cmh_toggle_rotation_xz(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_XZ
ConsoleMsgResult cmh_toggle_rotation_xw(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_XW
ConsoleMsgResult cmh_toggle_rotation_yz(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_YZ
ConsoleMsgResult cmh_toggle_rotation_yw(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_YW
ConsoleMsgResult cmh_toggle_rotation_zw(const ConsoleMsg* msg);         // CMT_TOGGLE_ROTATION_ZW

ConsoleMsgResult cmh_set_model_tetra(const ConsoleMsg* msg);            // CMT_SET_MODEL_TETRA
ConsoleMsgResult cmh_set_model_cube(const ConsoleMsg* msg);             // CMT_SET_MODEL_CUBE
ConsoleMsgResult cmh_set_model_oct(const ConsoleMsg* msg);              // CMT_SET_MODEL_OCT
ConsoleMsgResult cmh_set_model_ico(const ConsoleMsg* msg);              // CMT_SET_MODEL_ICO
ConsoleMsgResult cmh_set_model_5cell(const ConsoleMsg* msg);            // CMT_SET_MODEL_5CELL
ConsoleMsgResult cmh_set_model_8cell(const ConsoleMsg* msg);            // CMT_SET_MODEL_8CELL
ConsoleMsgResult cmh_set_model_24cell(const ConsoleMsg* msg);           // CMT_SET_MODEL_24CELL

ConsoleMsgResult cmh_set_wfade_alpha_enable(const ConsoleMsg* msg);     // CMT_SET_WFADE_ALPHA_ENABLE
ConsoleMsgResult cmh_set_wfade_alpha_range(const ConsoleMsg* msg);      // CMT_SET_WFADE_ALPHA_RANGE
ConsoleMsgResult cmh_set_wfade_alpha_segments(const ConsoleMsg* msg);   // CMT_SET_WFADE_ALPHA_SEGMENTS
ConsoleMsgResult cmh_set_wfade_alpha_near(const ConsoleMsg* msg);       // CMT_SET_WFADE_ALPHA_NEAR
ConsoleMsgResult cmh_set_wfade_alpha_far(const ConsoleMsg* msg);        // CMT_SET_WFADE_ALPHA_FAR

ConsoleMsgResult cmh_set_wfade_color_enable(const ConsoleMsg* msg);     // CMT_SET_WFADE_COLOR_ENABLE
ConsoleMsgResult cmh_set_wfade_color_range(const ConsoleMsg* msg);      // CMT_SET_WFADE_COLOR_RANGE
ConsoleMsgResult cmh_set_wfade_color_segments(const ConsoleMsg* msg);   // CMT_SET_WFADE_COLOR_SEGMENTS
ConsoleMsgResult cmh_set_wfade_color_near(const ConsoleMsg* msg);       // CMT_SET_WFADE_COLOR_NEAR
ConsoleMsgResult cmh_set_wfade_color_far(const ConsoleMsg* msg);        // CMT_SET_WFADE_COLOR_FAR

ConsoleMsgResult cmh_set_wfade_thick_enable(const ConsoleMsg* msg);     // CMT_SET_WFADE_THICK_ENABLE
ConsoleMsgResult cmh_set_wfade_thick_range(const ConsoleMsg* msg);      // CMT_SET_WFADE_THICK_RANGE
ConsoleMsgResult cmh_set_wfade_thick_segments(const ConsoleMsg* msg);   // CMT_SET_WFADE_THICK_SEGMENTS
ConsoleMsgResult cmh_set_wfade_thick_near(const ConsoleMsg* msg);       // CMT_SET_WFADE_THICK_NEAR
ConsoleMsgResult cmh_set_wfade_thick_far(const ConsoleMsg* msg);        // CMT_SET_WFADE_THICK_FAR

// -------------------------------- MODELS --------------------------------- //

typedef struct {
    ModelType type;
    Axes4D axes;
    Mesh4D mesh;
} Model4D;

// set which model to display
void app_set_model(ModelType type);

// ------------------------------ APPLICATION ------------------------------ //

typedef struct {

    // current render
    Model4D model;
    View4D view;
    Settings settings;

    // raylib stuff
    Font ui_font;

    // internals
    bool is_paused;
    bool is_exiting;
    ConsoleMsgQueue cmq;
    ConsoleMsgHandler cmh[CMT_COUNT];

    char console_gui_input[CONSOLE_MSG_LEN];
    size_t console_gui_input_len;
    bool console_gui_focused;
    const char* console_gui_autofill_entries[CONSOLE_GUI_AUTOFILL_ENTRY_COUNT];
    size_t console_gui_autofill_entries_len;

} AppContext;
extern AppContext APP; // declared in main.c

void app_init();    // inits raylib + data
void app_reset();   // just inits the data back to defaults
void app_run();
void app_exit();

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

// console

// parse a single line of text from the user
bool console_parse_line(const char *line, ConsoleMsg *msg_out);

// control the gui console
void console_gui_append_char(char c);
void console_gui_backspace();
void console_gui_clear();
// control the gui console autofill entries
void console_gui_append_autofill_entry(const char* str); // appends to top
void console_gui_clear_autofill_entries();
void console_gui_refresh_autofill();

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

// ------------------------------ DEBUG FLAGS ------------------------------ //

// display camera coordinates on the screen
#define DEBUG_CAMERA_COORDS 0

#endif // VIS_H
