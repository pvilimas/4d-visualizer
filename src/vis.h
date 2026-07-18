#ifndef VIS_H
#define VIS_H

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#include "raylib.h"
#include "raymath.h"

// ------------------------------ PARAMETERS ------------------------------- //

// prefix to use for console commands
#define CONSOLE_MSG_PREFIX "> "

// maximum length of a console command
#define CONSOLE_MSG_LEN 64

// how many arguments are allowed in a console command
#define CONSOLE_MSG_MAX_ARG_COUNT 10

// how many console commands can be queued at once
#define CONSOLE_MSG_MAX_QUEUE_SIZE 10

// how long the x,y,z,w axes extend out from the origin
#define AXIS_MAGNITUDE 1000

// the thickness of the axes
#define AXIS_THICKNESS 0.03f

// how many segments to draw the axis cylinders
#define AXIS_SEGMENTS 16

// the axis colors
#define WORLD_X_AXIS_COLOR ((Color){ 255, 0, 0, 255 })
#define WORLD_Y_AXIS_COLOR ((Color){ 0, 0, 255, 255 })
#define WORLD_Z_AXIS_COLOR ((Color){ 0, 255, 0, 255 })
#define WORLD_W_AXIS_COLOR ((Color){ 255, 255, 0, 255 })

#define LOCAL_X_AXIS_COLOR ((Color){ 255/4, 0, 0, 255 })
#define LOCAL_Y_AXIS_COLOR ((Color){ 0, 0, 255/4, 255 })
#define LOCAL_Z_AXIS_COLOR ((Color){ 0, 255/4, 0, 255 })
#define LOCAL_W_AXIS_COLOR ((Color){ 255/4, 255/4, 0, 255 })

// the size of the origin sphere
#define ORIGIN_SIZE 0.15f

// the color of the origin sphere
#define ORIGIN_COLOR WHITE

// -------------------------------- 4D MESH -------------------------------- //

typedef struct{
    Vector4* vertices;
    int vertex_count;
    int (*edges)[2];
    int edge_count;
} Mesh4D;

Mesh4D mesh4d_new(const Vector4* vertices, int vertex_count,
                const int (*edges)[2], int edge_count);

Mesh4D mesh4d_new_5cell();

Mesh4D mesh4d_new_8cell();

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
} View4D;

// ------------------------------- SETTINGS -------------------------------- //

typedef struct {

    // console settings

    bool console_mode_gui;      // if false use popout-terminal mode, if true gui

    // simulation settings

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

Settings get_default_settings();

// ------------------------------- CONSOLE --------------------------------- //

typedef enum {

    // commands to change application state

    // quit
    CMT_QUIT,

    // pause
    CMT_TOGGLE_PAUSE,

    // commands to modify the settings

    // wfade alpha <enabled:bool>
    CMT_SET_WFADE_ALPHA_ENABLE,
    // wfade alpha range <w-near:float> <w-far:float>
    CMT_SET_WFADE_ALPHA_RANGE,
    // wfade alpha segments <n:float>
    CMT_SET_WFADE_ALPHA_SEGMENTS,
    // wfade alpha near <a:float>
    CMT_SET_WFADE_ALPHA_NEAR,
    // wfade alpha far <a:float>
    CMT_SET_WFADE_ALPHA_FAR,

    // wfade color <enabled:bool>
    CMT_SET_WFADE_COLOR_ENABLE,
    // wfade color range <w-near:float> <w-far:float>
    CMT_SET_WFADE_COLOR_RANGE,
    // wfade color segments <n:float>
    CMT_SET_WFADE_COLOR_SEGMENTS,
    // wfade color near <r:float> <g:float> <b:float>
    CMT_SET_WFADE_COLOR_NEAR,
    // wfade color far <r:float> <g:float> <b:float>
    CMT_SET_WFADE_COLOR_FAR,

    // wfade thick <enabled:bool>
    CMT_SET_WFADE_THICK_ENABLE,
    // wfade thick range <w-near:float> <w-far:float>
    CMT_SET_WFADE_THICK_RANGE,
    // wfade thick segments <n>
    CMT_SET_WFADE_THICK_SEGMENTS,
    // wfade thick near <t:float>
    CMT_SET_WFADE_THICK_NEAR,
    // wfade thick far <t:float>
    CMT_SET_WFADE_THICK_FAR,

    // ...

    CMT_COUNT
} ConsoleMsgType;

// ----------------------- CONSOLE MESSAGE ARGUMENTS ----------------------- //

// parse different types of console command arguments
// order of priority is int > float > bool > string
typedef enum {
    CMAT_INVALID = 0,
    CMAT_INT,   // int64_t
    CMAT_FLOAT, // float
    CMAT_BOOL,  // bool
    // CMAT_STR
} ConsoleMsgArgType;
ConsoleMsgArgType cma_try_parse(const char* str, size_t len, bool* b_out, int64_t* i_out, float* f_out);
bool cma_try_parse_bool(const char* str, size_t len, bool* out);
bool cma_try_parse_int(const char* str, size_t len, int64_t* out);
bool cma_try_parse_float(const char* str, size_t len, float* out);

// perform all possible type conversions - type errors don't exist here
void cma_convert_int_to_float(int64_t in, float* out);
void cma_convert_int_to_bool(int64_t in, bool* out);
void cma_convert_float_to_int(float in, int64_t* out);
void cma_convert_float_to_bool(float in, bool* out);
void cma_convert_bool_to_int(bool in, int64_t* out);
void cma_convert_bool_to_float(bool in, float* out);

typedef struct {
    ConsoleMsgArgType type;
    union {
        bool bool_value;
        int64_t int_value;
        float float_value;
        // char[64] str_value;
    } get;
} ConsoleMsgArg;

typedef struct {
    ConsoleMsgType type;
    ConsoleMsgArg args[CONSOLE_MSG_MAX_ARG_COUNT];
    size_t num_args_received;
} ConsoleMsg;

typedef struct {
    pthread_mutex_t lock;
    ConsoleMsg messages[CONSOLE_MSG_MAX_QUEUE_SIZE];
    int size;
} ConsoleMsgQueue;

// initialize the message queue to empty
void cmq_init();

// push a console message to the queue, waits for lock (called by job thread)
// returns false if the message got dropped because the queue is full
bool cmq_push(ConsoleMsg msg);

// pop a console message from the queue, exits if locked (called by main thread)
// returns false if locked or if no messages are in the queue
bool cmq_pop(ConsoleMsg* msg_out);

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

ConsoleMsgResult cmh_quit(const ConsoleMsg* msg);                       // CMT_QUIT
ConsoleMsgResult cmh_toggle_pause(const ConsoleMsg* msg);               // CMT_TOGGLE_PAUSE

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
    Axes4D axes;
    Mesh4D mesh;
} Model4D;

// set which model to display
void app_set_model_5cell();
void app_set_model_8cell();

// ------------------------------ APPLICATION ------------------------------ //

typedef struct {

    // current render
    Model4D model;
    View4D view;
    Settings settings;

    // raylib stuff
    Font ui_font;

    // internals
    atomic_int is_paused;
    atomic_int is_exiting;
    pthread_t console_thread;
    ConsoleMsgQueue cmq;
    ConsoleMsgHandler cmh[CMT_COUNT];

    char console_gui_input[CONSOLE_MSG_LEN];
    size_t console_gui_input_len;
    bool console_gui_focused;

} AppContext;
extern AppContext APP; // declared in main.c

void app_init();
void app_run();
void app_exit();

// --------------------------------- UTILS --------------------------------- //

// drawing

// draws an entire line
void draw_line4(Vector4 p0, Vector4 p1);

// draws a camera-facing quad for a segment, width0 at p0, width1 at p1
void draw_line_segment3(Vector3 p0, Vector3 p1,
                        float width0, float width1,
                        Vector3 cam_pos,
                        Color color);

// draws a camera-facing quad for a segment, width0 at p0, width1 at p1
void draw_line_segment4(Vector4 p0, Vector4 p1,
                       float width0, float width1,
                       Vector3 cam_pos,
                       Color color);

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

// console jobs
typedef void* (*ConsoleJob)(void*);

// job thread function to poll the terminal window for input
void* console_job_terminal(void* arg);

// parse a single line of text from the user
bool console_parse_line(const char *line, ConsoleMsg *msg_out);

// control the gui console
void console_gui_append_char(char c);
void console_gui_backspace();
void console_gui_clear();

#endif // VIS_H
