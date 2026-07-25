#include "visualizer.h"

AppContext _APP;
AppContext* APP = &_APP;

int main() {

    app_init();
    app_run();
    app_exit();

    return 0;
}
