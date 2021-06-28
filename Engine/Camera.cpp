#include "Camera.h"

#include "Entity.h"

CameraInfo Camera::get_camera_info() const
{
    return {
        get_position(),
        get_rotation(),
        fov
    };
}
