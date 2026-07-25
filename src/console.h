#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"
#include "utils.h"

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

    // bg <r:int> <g:int> <b:int>
    CMT_CHANGE_BGCOLOR,

    // view <2d|3d>
    CMT_CHANGE_VIEW_2D,
    CMT_CHANGE_VIEW_3D,

    // axes
    CMT_TOGGLE_AXES,
    // hud
    CMT_TOGGLE_HUD,

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
    // wfade color near <r:int> <g:int> <b:int>
    CMT_SET_WFADE_COLOR_NEAR,
    // wfade color far <r:int> <g:int> <b:int>
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

struct ConsoleMsg {
    ConsoleMsgType type;
    ConsoleMsgArg args[CONSOLE_MSG_MAX_ARG_COUNT];
    size_t num_args_received;
};
typedef struct ConsoleMsg ConsoleMsg;

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

ConsoleMsgResult cmh_change_bgcolor(const ConsoleMsg* msg);             // CMT_CHANGE_BGCOLOR

ConsoleMsgResult cmh_change_view_2d(const ConsoleMsg* msg);             // CMT_CHANGE_VIEW_2D
ConsoleMsgResult cmh_change_view_3d(const ConsoleMsg* msg);             // CMT_CHANGE_VIEW_3D

ConsoleMsgResult cmh_toggle_axes(const ConsoleMsg* msg);                // CMT_TOGGLE_AXES
ConsoleMsgResult cmh_toggle_hud(const ConsoleMsg* msg);                 // CMT_TOGGLE_HUD

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
ConsoleMsgResult cmh_set_model_16cell(const ConsoleMsg* msg);           // CMT_SET_MODEL_16CELL
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

// utils

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

#endif // CONSOLE_H
