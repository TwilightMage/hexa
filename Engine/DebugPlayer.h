#pragma once
#include "IControllable.h"
#include "ITickable.h"
#include "Entity.h"

class DebugPlayer : public Entity, public IControllable, public ITickable
{
public:
    virtual void start() override;
    
    void key_down(int key) override;
    void key_up(int key) override;

    void tick(float delta_time) override;
    class Camera* camera;

private:
    float move_forward;
    float move_right;
    float move_up;
};
