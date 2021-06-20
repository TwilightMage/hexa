#pragma once

#include "AnimatedFieldBase.h"
#include "Map.h"
#include "String.h"

class AnimatedFieldBase;

class EXPORT Animated
{
public:
    virtual ~Animated() {}
    
    FORCEINLINE const Map<String, AnimatedFieldBase*>& get_animated_fields() const { return animated_fields_; }
    FORCEINLINE void register_animated_field(AnimatedFieldBase* field) { animated_fields_.insert(field->get_name(), field); }
    FORCEINLINE AnimatedFieldBase* find_animated_field(const String& name) const { return animated_fields_.find_or_default(name); }

private:
    Map<String, AnimatedFieldBase*> animated_fields_;
};
