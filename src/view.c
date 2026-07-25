#include "view.h"

// -------------------------------- 4D VIEW -------------------------------- //

bool view4d_is_any_axis_rotating(const View4D* v)
{
    return (v->rotation.xy.enabled && v->rotation.xy.speed != 0)
    || (v->rotation.xz.enabled && v->rotation.xz.speed != 0)
    || (v->rotation.xw.enabled && v->rotation.xw.speed != 0)
    || (v->rotation.yz.enabled && v->rotation.yz.speed != 0)
    || (v->rotation.yw.enabled && v->rotation.yw.speed != 0)
    || (v->rotation.zw.enabled && v->rotation.zw.speed != 0);
}
