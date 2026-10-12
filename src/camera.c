#include "camera.h"
#include "visualizer.h"

// ------------------------------- 4D CAMERA ------------------------------- //

void update_camera_ortho()
{
    APP->view.camera_control.ortho_zoom -= GetMouseWheelMove() * 0.5f;
    APP->view.camera_control.ortho_zoom = Clamp(APP->view.camera_control.ortho_zoom,
                                               ORTHO_ZOOM_MIN,
                                               ORTHO_ZOOM_MAX);

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 mouse_delta = GetMouseDelta();
        APP->view.camera_control.ortho_pan.x -= mouse_delta.x * 0.01f * APP->view.camera_control.ortho_zoom;
        APP->view.camera_control.ortho_pan.y += mouse_delta.y * 0.01f * APP->view.camera_control.ortho_zoom;
    }

    Vector3 dir = Vector3Normalize(APP->view.camera_control.ortho_view_dir);
    Vector3 world_up = (Vector3){0, 1, 0};
    Vector3 right = Vector3Normalize(Vector3CrossProduct(world_up, dir));
    if (Vector3LengthSqr(right) < 0.0001f)
    {
        right = (Vector3){1, 0, 0};
    }
    Vector3 up = Vector3CrossProduct(dir, right);

    Vector3 target = Vector3Add(
        Vector3Scale(right, APP->view.camera_control.ortho_pan.x),
                                Vector3Scale(up, APP->view.camera_control.ortho_pan.y)
    );

    APP->view.camera.target = target;
    APP->view.camera.position = Vector3Subtract(target, Vector3Scale(dir, 20.0f));
    APP->view.camera.up = up;
    APP->view.camera.projection = CAMERA_ORTHOGRAPHIC;
    APP->view.camera.fovy = APP->view.camera_control.ortho_zoom * 10.0f;
}

void update_camera_orbital()
{
    static const float ORBITAL_TURN_SPEED = 1.5f;
    static const float ORBITAL_ROLL_SPEED = 1.5f;
    static const float ORBITAL_RADIUS_MIN = 0.5f;
    static const float ORBITAL_RADIUS_MAX = 100.0f;
    static const float ORBITAL_ZOOM_SENSITIVITY = 1.0f;

    float dt = GetFrameTime();

    float yaw_delta = 0.0f;
    float pitch_delta = 0.0f;

    if (IsKeyDown(KEY_A)) { yaw_delta -= ORBITAL_TURN_SPEED * dt; }
    if (IsKeyDown(KEY_D)) { yaw_delta += ORBITAL_TURN_SPEED * dt; }
    if (IsKeyDown(KEY_W)) { pitch_delta += ORBITAL_TURN_SPEED * dt; }
    if (IsKeyDown(KEY_S)) { pitch_delta -= ORBITAL_TURN_SPEED * dt; }

    APP->view.camera_control.orbital_yaw += yaw_delta;
    APP->view.camera_control.orbital_pitch += pitch_delta;

    if (IsKeyDown(KEY_Q)) { APP->view.camera_control.orbital_roll -= ORBITAL_ROLL_SPEED * dt; }
    if (IsKeyDown(KEY_E)) { APP->view.camera_control.orbital_roll += ORBITAL_ROLL_SPEED * dt; }

    float wheel = GetMouseWheelMove();
    APP->view.camera_control.orbital_radius -= wheel * ORBITAL_ZOOM_SENSITIVITY;
    if (APP->view.camera_control.orbital_radius < ORBITAL_RADIUS_MIN) { APP->view.camera_control.orbital_radius = ORBITAL_RADIUS_MIN; }
    if (APP->view.camera_control.orbital_radius > ORBITAL_RADIUS_MAX) { APP->view.camera_control.orbital_radius = ORBITAL_RADIUS_MAX; }

    float yaw = APP->view.camera_control.orbital_yaw;
    float pitch = APP->view.camera_control.orbital_pitch;
    float radius = APP->view.camera_control.orbital_radius;

    Vector3 pos;
    pos.x = radius * cosf(pitch) * sinf(yaw);
    pos.y = radius * sinf(pitch);
    pos.z = radius * cosf(pitch) * cosf(yaw);

    Vector3 forward = Vector3Normalize(Vector3Negate(pos));

    // pitch_axis derived from the YAW-ONLY horizontal direction, never from
    // "up" or from "forward" directly — both of those go parallel to
    // world_up near the poles and cause a degenerate cross product there.
    // this horizontal_forward is never vertical, so it's always safe.
    Vector3 world_up = { 0, 1, 0 };
    Vector3 horizontal_forward = { sinf(yaw), 0.0f, cosf(yaw) };
    Vector3 pitch_axis = Vector3Normalize(Vector3CrossProduct(horizontal_forward, world_up));

    Vector3 up = APP->view.camera_control.orbital_up;
    up = Vector3RotateByAxisAngle(up, world_up, yaw_delta);
    up = Vector3RotateByAxisAngle(up, pitch_axis, pitch_delta);

    if (IsKeyDown(KEY_Q)) { up = Vector3RotateByAxisAngle(up, forward, -ORBITAL_ROLL_SPEED * dt); }
    if (IsKeyDown(KEY_E)) { up = Vector3RotateByAxisAngle(up, forward, ORBITAL_ROLL_SPEED * dt); }

    up = Vector3Normalize(up);
    APP->view.camera_control.orbital_up = up;

    APP->view.camera.position = pos;
    APP->view.camera.target = (Vector3){ 0, 0, 0 };
    APP->view.camera.up = up;
}

// void update_camera_orbital()
// {
//     static const float ORBITAL_ROTATE_SPEED = 0.9f;       // 54 degrees per second
//     static const float ORBITAL_ROLL_SPEED = 1.5f;
//     static const float ORBITAL_RADIUS_MIN = 0.5f;
//     static const float ORBITAL_RADIUS_MAX = 100.0f;
//     static const float ORBITAL_ZOOM_SENSITIVITY = 1.0f;
//     static const float ORBITAL_SNAP_ANGLE = 0.13f;        // 7.5deg
//     static const float ORBITAL_SNAP_SPEED = 6.0f;         // ease-in rate once triggered
//
//     static const Vector3 WORLD_UP = { 0, 1, 0 };
//
//     static const Vector3 CARDINAL_DIRS_XYZ[6] = {
//         {  1,  0,  0 }, { -1,  0,  0 },
//         {  0,  1,  0 }, {  0, -1,  0 },
//         {  0,  0,  1 }, {  0,  0, -1 },
//     };
//
//     float dt = GetFrameTime();
//
//     Vector3 pos = APP->view.camera_control.orbital_position;
//     Vector3 up = APP->view.camera_control.orbital_up;
//     float radius = Vector3Length(pos);
//
//     Vector3 forward = Vector3Normalize(Vector3Negate(pos));
//
//     bool rotating = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A)
//     || IsKeyDown(KEY_D) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_E);
//
//     if (rotating)
//     {
//         float pitch_delta = 0.0f;
//         float yaw_delta = 0.0f;
//
//         if (IsKeyDown(KEY_W)) { pitch_delta += ORBITAL_ROTATE_SPEED * dt; }
//         if (IsKeyDown(KEY_S)) { pitch_delta -= ORBITAL_ROTATE_SPEED * dt; }
//         if (IsKeyDown(KEY_D)) { yaw_delta += ORBITAL_ROTATE_SPEED * dt; }
//         if (IsKeyDown(KEY_A)) { yaw_delta -= ORBITAL_ROTATE_SPEED * dt; }
//
//         // pitch axis: derived from forward's projection onto the horizontal
//         // (xz) plane, NOT from the camera's current (possibly tilted) up/right.
//         // this is what keeps the pitch circle lying in a fixed vertical plane
//         // instead of an oblique one once you've rotated off-axis.
//         Vector3 horizontal_forward = { forward.x, 0.0f, forward.z };
//         float horizontal_len = Vector3Length(horizontal_forward);
//
//         Vector3 pitch_axis;
//         if (horizontal_len > 0.0001f)
//         {
//             horizontal_forward = Vector3Scale(horizontal_forward, 1.0f / horizontal_len);
//             pitch_axis = Vector3Normalize(Vector3CrossProduct(horizontal_forward, WORLD_UP));
//         }
//         else
//         {
//             // forward is exactly vertical (rare, only right after a pole
//             // snap) -- fall back to current right's horizontal projection
//             Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
//             pitch_axis = Vector3Normalize((Vector3){ right.x, 0.0f, right.z });
//         }
//
//         pos = Vector3RotateByAxisAngle(pos, pitch_axis, pitch_delta);
//         up = Vector3RotateByAxisAngle(up, pitch_axis, pitch_delta);
//
//         // yaw axis: always WORLD_UP, not the camera's current up, so yaw
//         // circles always lie in the xz plane regardless of current pitch/roll
//         pos = Vector3RotateByAxisAngle(pos, WORLD_UP, yaw_delta);
//         up = Vector3RotateByAxisAngle(up, WORLD_UP, yaw_delta);
//
//         forward = Vector3Normalize(Vector3Negate(pos));
//
//         if (IsKeyDown(KEY_Q)) { up = Vector3RotateByAxisAngle(up, forward, -ORBITAL_ROLL_SPEED * dt); }
//         if (IsKeyDown(KEY_E)) { up = Vector3RotateByAxisAngle(up, forward, ORBITAL_ROLL_SPEED * dt); }
//
//         up = Vector3Normalize(up);
//     }
//     else
//     {
//         Vector3 w_dir = Vector3Normalize(APP->view.axes.w);
//
//         Vector3 candidates[8];
//         for (int i = 0; i < 6; i += 1)
//         {
//             candidates[i] = CARDINAL_DIRS_XYZ[i];
//         }
//         candidates[6] = w_dir;
//         candidates[7] = Vector3Negate(w_dir);
//
//         int best_index = -1;
//         float best_dot = -1.0f;
//
//         for (int i = 0; i < 8; i += 1)
//         {
//             float d = Vector3DotProduct(forward, candidates[i]);
//             if (d > best_dot)
//             {
//                 best_dot = d;
//                 best_index = i;
//             }
//         }
//
//         float angle_to_target = acosf(fminf(fmaxf(best_dot, -1.0f), 1.0f));
//
//         if (angle_to_target < ORBITAL_SNAP_ANGLE && angle_to_target > 0.0001f)
//         {
//             Vector3 target_forward = candidates[best_index];
//
//             Vector3 target_up_reference = { 0, 1, 0 };
//             if (fabsf(Vector3DotProduct(target_forward, target_up_reference)) > 0.99f)
//             {
//                 target_up_reference = (Vector3){ 0, 0, 1 };
//             }
//
//             Vector3 target_right = Vector3Normalize(Vector3CrossProduct(target_forward, target_up_reference));
//             Vector3 target_up = Vector3CrossProduct(target_right, target_forward);
//
//             float t = fminf(ORBITAL_SNAP_SPEED * dt / angle_to_target, 1.0f);
//
//             Vector4 quat_from = QuaternionFromVector3ToVector3(forward, target_forward);
//             Vector3 new_forward = Vector3RotateByQuaternion(forward, QuaternionSlerp(QuaternionIdentity(), quat_from, t));
//             new_forward = Vector3Normalize(new_forward);
//
//             Vector4 up_quat_from = QuaternionFromVector3ToVector3(up, target_up);
//             up = Vector3RotateByQuaternion(up, QuaternionSlerp(QuaternionIdentity(), up_quat_from, t));
//             up = Vector3Normalize(up);
//
//             pos = Vector3Scale(Vector3Negate(new_forward), radius);
//         }
//     }
//
//     float wheel = GetMouseWheelMove();
//     radius -= wheel * ORBITAL_ZOOM_SENSITIVITY;
//     if (radius < ORBITAL_RADIUS_MIN) { radius = ORBITAL_RADIUS_MIN; }
//     if (radius > ORBITAL_RADIUS_MAX) { radius = ORBITAL_RADIUS_MAX; }
//     pos = Vector3Scale(Vector3Normalize(pos), radius);
//
//     APP->view.camera_control.orbital_position = pos;
//     APP->view.camera_control.orbital_up = up;
//
//     APP->view.camera.position = pos;
//     APP->view.camera.target = (Vector3){ 0, 0, 0 };
//     APP->view.camera.up = up;
// }
