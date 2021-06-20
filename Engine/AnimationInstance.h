#pragma once

#include "AnimatedField.h"
#include "BasicTypes.h"
#include "List.h"

class Animation;
class AnimatorComponent;

class EXPORT AnimationInstance
{
public:
    AnimationInstance(Shared<Animation> animation, List<AnimatedField<float>*> fields);

    void tick(float delta_time);
    void update_data_pointers();
    void revert();

    bool is_playing = false;
    bool is_finished = false;
    float time = 0.0f;
    float speed = 1.0f;
    bool reversed = false;
    bool looped = false;
    Shared<Animation> animation;
    List<AnimatedField<float>*> fields;
    List<uint> playing_point_indices;
};
