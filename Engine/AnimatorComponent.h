#pragma once

#include "Animation.h"
#include "AnimationHandle.h"
#include "AnimationInstance.h"
#include "ConversionPointer.h"
#include "EntityComponent.h"

class AnimationHandle;

class EXPORT AnimatorComponent : public EntityComponent
{
    friend AnimationHandle;
public:
    AnimationHandle play_animation(const Shared<Animation>& animation, float start_time_offset = 0.0f, float speed = 1.0f, bool reversed = false);

    void on_tick(float delta_time) override;

private:
    void pause(uint instance_id);
    void stop(uint instance_id);
    
    Map<uint, Shared<AnimationInstance>> instances_;
    uint id_generator_;
};
