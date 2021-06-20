#pragma once

#include "Entity.h"
#include "IControllable.h"

class EXPORT Player : public Entity, public IControllable
{
public:
    void on_start() override;
    void on_possess() override;

    FORCEINLINE const Shared<Camera>& get_camera() const { return camera_; }

private:
    Shared<Camera> camera_;
};
