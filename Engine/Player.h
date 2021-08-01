#pragma once

#include "Entity.h"
#include "IControllable.h"

class EXPORT Player : public Entity, public IControllable
{
public:
    Player();
    
    void on_possess() override;

private:
    Shared<CameraComponent> camera_;
};
