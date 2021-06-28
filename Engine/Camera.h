#pragma once

#include "Entity.h"
#include "framework.h"
#include "ICamera.h"

class Entity;

class EXPORT Camera : public Entity, public ICamera
{
public:
    float fov = 45.0f;

    CameraInfo get_camera_info() const override;
};
