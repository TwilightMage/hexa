#pragma once

#include "Entity.h"
#include "ICamera.h"
#include "IControllable.h"

class EXPORT Player : public Entity, public IControllable, public ICamera
{
public:
    void on_possess() override;

    CameraInfo get_camera_info() const override;
};
