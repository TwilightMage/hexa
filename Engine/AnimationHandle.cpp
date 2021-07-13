#include "AnimationHandle.h"

#include "AnimationInstance.h"
#include "AnimatorComponent.h"

AnimationHandle::AnimationHandle()
    : animator_(null_weak(AnimatorComponent))
    , instance_id_(0)
{
}

AnimationHandle::AnimationHandle(Weak<AnimatorComponent> animator, uint instance_id)
    : animator_(animator)
    , instance_id_(instance_id)
{
}

bool AnimationHandle::is_playing() const
{
    if (const auto instance = get_instance())
    {
        return instance->is_playing;
    }

    return false;
}

bool AnimationHandle::is_finished() const
{
    if (const auto instance = get_instance())
    {
        return instance->is_finished;
    }

    return false;
}

float AnimationHandle::get_time() const
{
    if (const auto instance = get_instance())
    {
        return instance->time;
    }

    return 0.0f;
}

bool AnimationHandle::is_looped() const
{
    if (const auto instance = get_instance())
    {
        return instance->looped;
    }

    return false;
}

Shared<Animation> AnimationHandle::get_animation() const
{
    if (const auto instance = get_instance())
    {
        return instance->animation;
    }

    return nullptr;
}

bool AnimationHandle::is_valid() const
{
    return get_instance() != nullptr;
}

void AnimationHandle::stop() const
{
    if (const auto animator = animator_.lock())
    {
        animator->stop(instance_id_);
    }
}

Delegate<>* AnimationHandle::on_end() const
{
    if (const auto instance = get_instance())
    {
        return &instance->on_end;
    }

    return nullptr;
}

Shared<AnimationInstance> AnimationHandle::get_instance() const
{
    if (const auto animator = animator_.lock())
    {
        return animator->instances_.find_or_default(instance_id_);
    }

    return nullptr;
}
