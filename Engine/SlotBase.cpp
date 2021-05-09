#include "SlotBase.h"

#include "IUsageCountableBase.h"
#include "UsageCounterBase.h"

void SlotBase::inc_ref(IUsageCountableBase* countable)
{
    if (countable && countable->get_usage_counter_generic()->increase(countable))
    {
        countable->load();
    }
}

void SlotBase::dec_ref(IUsageCountableBase* countable)
{
    if (countable && countable->get_usage_counter_generic()->decrease(countable))
    {
        countable->unload();
    }
}
