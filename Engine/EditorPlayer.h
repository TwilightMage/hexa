#pragma once

#include "BasicTypes.h"
#include "ITickable.h"
#include "Player.h"

class Entity;

class EXPORT EditorPlayer : public Player, public ITickable
{
public:
    void tick(float delta_time) override;

    void mouse_button_down(int button) override;
    void mouse_button_up(int button) override;
    void scroll(const Vector2& delta) override;

    Vector3 position_offset;

private:
    bool is_rotating_ = false;
    float distance_ = 2.0f;
    float target_distance_ = 2.0f;
};
