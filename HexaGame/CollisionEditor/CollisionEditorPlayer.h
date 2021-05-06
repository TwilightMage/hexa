#pragma once
#include "Engine/Entity.h"
#include "Engine/IControllable.h"
#include "Engine/ITickable.h"

class MeshEntity;
class Camera;

class CollisionEditorPlayer : public Entity, public IControllable, public ITickable
{
public:
    void on_start() override;
    void tick(float delta_time) override;

    void on_possess() override;

    void mouse_button_down(int button) override;
    void mouse_button_up(int button) override;

    Shared<MeshEntity> sample;

private:
    bool is_rotating_ = false;

    Shared<Camera> camera_;
};
