#pragma once

#include "framework.h"

class SlotBase;
class UsageCounterBase;

class EXPORT IUsageCountableBase
{
    friend SlotBase;
    
protected:
    virtual UsageCounterBase* get_usage_counter_generic() const = 0;
    virtual void load() {};
    virtual void unload() {};
};
