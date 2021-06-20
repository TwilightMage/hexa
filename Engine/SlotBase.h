#pragma once

#include "framework.h"

class IUsageCountableBase;

class EXPORT SlotBase
{
protected:
    static void inc_ref(IUsageCountableBase* countable);
    static void dec_ref(IUsageCountableBase* countable);
};
