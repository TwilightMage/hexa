#pragma once

#include "CameraInfo.h"

class ICamera
{
public:
    virtual CameraInfo get_camera_info() const = 0;
};
