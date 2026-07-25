#ifndef VIEW_H
#define VIEW_H

#include "common.h"

#include "axes.h"
#include "camera.h"

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

#endif // VIEW_H
