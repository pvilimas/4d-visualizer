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
    float dt = GetFrameTime();
    float turn_speed = 1.5f;

    if (IsKeyDown(KEY_A)) { APP->view.camera_control.orbital_yaw -= turn_speed * dt; }
    if (IsKeyDown(KEY_D)) { APP->view.camera_control.orbital_yaw += turn_speed * dt; }
    if (IsKeyDown(KEY_W)) { APP->view.camera_control.orbital_pitch += turn_speed * dt; }
    if (IsKeyDown(KEY_S)) { APP->view.camera_control.orbital_pitch -= turn_speed * dt; }

    float pitch_limit = 1.5f;
    if (APP->view.camera_control.orbital_pitch > pitch_limit) { APP->view.camera_control.orbital_pitch = pitch_limit; }
    if (APP->view.camera_control.orbital_pitch < -pitch_limit) { APP->view.camera_control.orbital_pitch = -pitch_limit; }

    float wheel = GetMouseWheelMove();
    APP->view.camera_control.orbital_radius -= wheel * 1.0f;
    if (APP->view.camera_control.orbital_radius < 1.0f) { APP->view.camera_control.orbital_radius = 1.0f; }
    if (APP->view.camera_control.orbital_radius > 100.0f) { APP->view.camera_control.orbital_radius = 100.0f; }

    float yaw = APP->view.camera_control.orbital_yaw;
    float pitch = APP->view.camera_control.orbital_pitch;
    float radius = APP->view.camera_control.orbital_radius;

    Vector3 pos;
    pos.x = radius * cosf(pitch) * sinf(yaw);
    pos.y = radius * sinf(pitch);
    pos.z = radius * cosf(pitch) * cosf(yaw);

    APP->view.camera.position = pos;
    APP->view.camera.target = (Vector3){0, 0, 0};
    APP->view.camera.up = (Vector3){0, 1, 0};
}
