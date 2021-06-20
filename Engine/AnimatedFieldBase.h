#pragma once

#include "String.h"

class EXPORT AnimatedFieldBase
{
public:
    AnimatedFieldBase(String type_name, String name);
    
    virtual ~AnimatedFieldBase() {}

    FORCEINLINE const String& get_type_name() const { return type_name_; }
    FORCEINLINE const String& get_name() const { return name_; }

private:
    String type_name_;
    String name_;
};
