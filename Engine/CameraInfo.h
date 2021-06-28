#pragma once

#include "Quaternion.h"
#include "Vector3.h"

struct CameraInfo
{
    Vector3 position = Vector3::zero();
    Quaternion rotation = Vector3::zero();
    float fov = 45.0f;
};
