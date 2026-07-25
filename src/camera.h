#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

// ------------------------------- 4D CAMERA ------------------------------- //

typedef enum {
    CM_2D_ORTHO, // you're locked to the z-axis which is hidden
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

#endif // CAMERA_H
