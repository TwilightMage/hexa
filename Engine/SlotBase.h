#pragma once

#include "framework.h"

class IUsageCountableBase;

EXTERN class EXPORT SlotBase
{
protected:
    static void inc_ref(IUsageCountableBase* countable);
    static void dec_ref(IUsageCountableBase* countable);
};
