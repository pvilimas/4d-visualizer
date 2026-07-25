#ifndef AXES_H
#define AXES_H

#include "common.h"

// -------------------------------- 4D AXES -------------------------------- //

// where the 4 local axes point in 4d space
typedef struct {
    Vector4 x, y, z, w;
} Axes4D;

// where the 4 world axes point after being projected down to 3d space
typedef struct {
    Vector3 x, y, z, w;
} ProjectionAxes4D;

#endif // AXES_H
