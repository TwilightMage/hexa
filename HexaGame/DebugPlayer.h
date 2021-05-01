#pragma once

#include "Engine/Entity.h"
#include "Engine/IControllable.h"
#include "Engine/ITickable.h"


class Camera;

EXTERN class EXPORT DebugPlayer : public Entity, public IControllable, public ITickable
{
public:
    virtual void start() override;
    
    void key_down(int key) override;
    void key_up(int key) override;

    void on_possess() override;

    void tick(float delta_time) override;
    Shared<Camera> camera;

private:
    float move_forward;
    float move_right;
    float move_up;
};
