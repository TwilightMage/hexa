#pragma once

#include "BasicTypes.h"

EXTERN class EXPORT UsageCounterBase
{
public:
    virtual bool increase(IUsageCountableBase* ptr) = 0;
    virtual bool decrease(IUsageCountableBase* ptr) = 0;
    virtual void clear() = 0;
    virtual uint count(IUsageCountableBase* ptr) const = 0;
};
