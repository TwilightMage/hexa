﻿#pragma once

#include "MeshEntity.h"
#include "Engine/Entity.h"
#include "Engine/IControllable.h"
#include "Engine/ITickable.h"


class Camera;

EXTERN class EXPORT DebugPlayer : public Entity, public IControllable, public ITickable
{
public:
    DebugPlayer();
    
    void on_start() override;
    
    void key_down(int key) override;
    void key_up(int key) override;
    void mouse_button_down(int button) override;

    void on_possess() override;

    void tick(float delta_time) override;

private:
    float move_forward_ = 0;
    float move_right_ = 0;
    float move_up_ = 0;

    Shared<Camera> camera_;
    Shared<MeshEntity> arrows_;
};
