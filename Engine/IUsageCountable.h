#pragma once

#include "IUsageCountableBase.h"
#include "UsageCounter.h"
#include "UsageCounterBase.h"

template<class C>
class IUsageCountable : public IUsageCountableBase
{
protected:   
    void unload_all() const
    {
        auto& counter = *get_usage_counter();
        for (auto& usage : counter) static_cast<IUsageCountable<C>*>(usage.first)->unload();
        counter.clear();
    }
    
    uint usage_count() const
    {
        return get_usage_counter_generic()->count(const_cast<IUsageCountable*>(this));
    }

    virtual UsageCounter<C>* get_usage_counter() const = 0;
};

#define IMPLEMENT_USAGE_COUNTER(Type)\
    private:\
    inline static UsageCounter<Type> usage_counter_ = UsageCounter<Type>();\
    static UsageCounter<Type>& get_usage_counter_static() { return usage_counter_; }\
    static void unload_all_static()\
    {\
        for (auto& usage : usage_counter_) usage.first->unload();\
        usage_counter_.clear();\
    }\
    UsageCounterBase* get_usage_counter_generic() const override { return &usage_counter_; }\
    UsageCounter<Type>* get_usage_counter() const override { return &usage_counter_; }\
    public:\
    static void print_usage_dump()\
    {\
        auto dump = usage_counter_.dump();\
        List<String> result;\
        for (const auto& kvp : dump)\
        {\
            result.add(String::format("  [%s]: %i", kvp.first.c(), kvp.second));\
        }\
        print_debug(#Type, "Usage dump:\n%s", String::join(result, '\n').c());\
    }\
    private:\
