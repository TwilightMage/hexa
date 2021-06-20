#pragma once

#include <functional>

#include "Animated.h"
#include "AnimatedFieldBase.h"
#include "framework.h"

class Animated;

template<typename T>
class AnimatedField : public AnimatedFieldBase
{
public:
    AnimatedField(std::function<T()> getter, std::function<void(T)> setter, const T& default_value, const String& name, Animated* owner)
        : AnimatedFieldBase(typeid(T).name(), name)
        , getter_(getter)
        , setter_(setter)
        , default_value_(default_value)
    {
        owner->register_animated_field(this);
    }

    FORCEINLINE T get() const { return getter_(); }
    FORCEINLINE void set(T val) { setter_(val); }
    FORCEINLINE T get_default_value() const { return default_value_; }
    FORCEINLINE void reset() { setter_(default_value_); }

private:
    std::function<T()> getter_;
    std::function<void(T val)> setter_;
    T default_value_;
};

#define ANIMATED_FIELD(type, name, default_value, getter, setter) AnimatedField<type> name##_field = AnimatedField<type>(getter, setter, default_value, String(#name).replace('_', ' '), this);