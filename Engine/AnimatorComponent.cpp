#include "AnimatorComponent.h"

#include "Entity.h"

AnimationHandle AnimatorComponent::play_animation(const Shared<Animation>& animation, float start_time_offset, float speed, bool reversed)
{
    if (auto animated_owner = cast<Animated>(get_owner()))
    {
        for (auto& instance : instances_)
        {
            if (instance.value->animation == animation)
            {
                instance.value->is_playing = false;
                instance.value->is_finished = true;
                break;
            }
        }

        List<AnimatedField<float>*> fields;
        for (auto named_curve : animation->named_curves_)
        {
            fields.Add(cast<AnimatedField<float>>(animated_owner->find_animated_field(named_curve.name)));
        }

        auto result = MakeShared<AnimationInstance>(animation, fields);
        result->is_playing = true;
        result->time = reversed ? animation->get_length() - start_time_offset : start_time_offset;
        result->speed = speed;
        result->reversed = reversed;

        if (reversed)
        {
            for (uint i = 0; i < result->playing_point_indices.length(); i++)
            {
                result->playing_point_indices[i] = animation->named_curves_[i].curve->get_segments().length();
            }
        }
        
        instances_.insert(id_generator_++, result);
            
        return AnimationHandle(cast<AnimatorComponent>(shared_from_this()), id_generator_ - 1);
    }

    return AnimationHandle();
}

void AnimatorComponent::on_tick(float delta_time)
{
    List<uint> instances_to_remove;

    for (auto instance : instances_)
    {
        if (!instance.value->is_finished)
        {
            instance.value->tick(delta_time);
        }
        
        if (instance.value->is_finished)
        {
            instance.value->revert();
            instance.value->on_end();
            instance.value->is_playing = false;
            instances_to_remove.Add(instance.key);
        }
    }

    for (auto instance_to_remove : instances_to_remove)
    {
        instances_.remove(instance_to_remove);
    }
}

void AnimatorComponent::pause(uint instance_id)
{
    if (const auto instance = instances_.find(instance_id))
    {
        
    }
}

void AnimatorComponent::stop(uint instance_id)
{
    if (const auto instance = instances_.find(instance_id))
    {
        (*instance)->revert();
        (*instance)->is_playing = false;
        instances_.remove(instance_id);
    }
}
