#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "common.h"

#include "camera.h"
#include "mesh.h"
#include "axes.h"
#include "view.h"
#include "console.h"
#include "settings.h"

// -------------------------------- MODELS --------------------------------- //

typedef struct {
    ModelType type;
    Axes4D axes;
    Mesh4D mesh;
} Model4D;

// ------------------------------ APPLICATION ------------------------------ //

struct AppContext{

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

};
extern AppContext* APP; // declared in main.c

void app_init();    // inits raylib + data
void app_reset();   // just inits the data back to defaults
void app_run();
void app_exit();

// set which model to display
void app_set_model(ModelType type);

// ------------------------------ DEBUG FLAGS ------------------------------ //

// display camera coordinates on the screen
#define DEBUG_CAMERA_COORDS 0

#endif // VISUALIZER_H
