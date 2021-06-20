#include "AnimationInstance.h"

#include "Animation.h"

AnimationInstance::AnimationInstance(Shared<Animation> animation, List<AnimatedField<float>*> fields)
    : animation(animation)
    , fields(fields)
{
    playing_point_indices = List<uint>(fields.length());
}

void AnimationInstance::tick(float delta_time)
{
    if (reversed)
    {
        time -= delta_time * speed;
    }
    else
    {
        time += delta_time * speed;
    }
    update_data_pointers();
    if (looped)
    {
        if (time > animation->get_length())
        {
            time -= animation->get_length();
        }
        else if (time < 0)
        {
            time += animation->get_length();
        }
    }
    else if (time > animation->get_length() || time < 0)
    {
        is_finished = true;
    }
}

void AnimationInstance::update_data_pointers()
{
    for (uint i = 0; i < fields.length(); i++)
    {
        if (fields[i])
        {
            if (reversed)
            {
                fields[i]->set(animation->evaluate_curve_from_backwards(i, time, playing_point_indices[i]));
            }
            else
            {
                fields[i]->set(animation->evaluate_curve_from(i, time, playing_point_indices[i]));
            }
        }
    }
}

void AnimationInstance::revert()
{
    for (uint i = 0; i < fields.length(); i++)
    {
        if (fields[i])
        {
            fields[i]->reset();
        }
    }
}
