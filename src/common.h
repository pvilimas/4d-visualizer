#ifndef COMMON_H
#define COMMON_H

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
#include "rlgl.h"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define CONSTANT_SQRT2 1.41421356237309504880f
#define CONSTANT_SQRT3 1.73205080756887729353f
#define CONSTANT_SQRT5 2.23606797749978969641f
#define CONSTANT_PI    3.14159265358979323846f
#define CONSTANT_PHI   1.61803398874989484820f

// ------------------------ COMPILE-TIME PARAMETERS ------------------------ //

// prefix to use for console commands
#define CONSOLE_MSG_PREFIX "> "

// maximum length of a console command
#define CONSOLE_MSG_LEN 64

// how many arguments are allowed in a console command
#define CONSOLE_MSG_MAX_ARG_COUNT 10

// how many console commands can be queued at once
#define CONSOLE_MSG_MAX_QUEUE_SIZE 10

// how many lines there can be in the console + 1 basically
#define CONSOLE_GUI_AUTOFILL_ENTRY_COUNT 20

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
// #define ORIGIN_COLOR RAYWHITE
#define ORIGIN_COLOR GUI_COLOR_BORDER_LIGHT

// orthographic 2d mode zoom (z-coordinates)
#define ORTHO_ZOOM_MIN 0.0001f
#define ORTHO_ZOOM_MAX 20.0f

// gui color scheme

#define GUI_COLOR_BORDER_LIGHT ((Color){180,180,180,255})
#define GUI_COLOR_BORDER_DARK ((Color){90,90,90,255})
#define GUI_COLOR_FILL_LIGHT ((Color){120,120,120,255})
#define GUI_COLOR_FILL_DARK ((Color){60,60,60,255})

// ------------------------------ DEBUG FLAGS ------------------------------ //

// display camera coordinates on the screen
#define DEBUG_CAMERA_COORDS 0

// draw a sphere of radius 0.5 around the origin
#define DEBUG_CIRCUMSPHERE 0

// ------------------------------ MODEL TYPES ------------------------------ //

typedef enum {
    MT_NONE,

    MT_CUSTOM, // a model with special operations done to it

    // presets

    MT_3D_TETRA,
    MT_3D_CUBE,
    MT_3D_OCT,
    MT_3D_ICO,

    MT_4D_5CELL,
    MT_4D_8CELL,
    MT_4D_16CELL,
    MT_4D_24CELL,
    // MT_4D_120CELL,
    // MT_4D_600CELL,
    // ...

    MT_COUNT
} ModelType;

// other

struct AppContext;
typedef struct AppContext AppContext;
extern AppContext* APP;

#endif // COMMON_H
