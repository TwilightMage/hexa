#pragma once

#include "BasicTypes.h"
#include "framework.h"

class AnimationInstance;
class Animation;
class AnimatorComponent;

class EXPORT AnimationHandle
{
public:
    AnimationHandle();
    AnimationHandle(Weak<AnimatorComponent> animator, uint instance_id);
    
    bool is_playing() const;
    bool is_finished() const;
    float get_time() const;
    bool is_looped() const;
    Shared<Animation> get_animation() const;
    bool is_valid() const;

    void stop() const;

private:
    Shared<AnimationInstance> get_instance() const;
    
    Weak<AnimatorComponent> animator_;
    uint instance_id_;
};
