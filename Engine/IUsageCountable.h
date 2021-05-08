#pragma once

#include <map>

#include "BasicTypes.h"

template<class C>
class IUsageCountable
{
protected:
    virtual std::map<C*, uint>& usage_counter() = 0;
    virtual void unload_all() = 0;
    virtual void load() {};
    virtual void unload() {};
};

#define IMPLEMENT_USAGE_COUNTER(Type)\
    private:\
    static std::map<Type*, uint> usage_counter_;\
    protected:\
    std::map<Type*, uint>& usage_counter() override { return usage_counter_; }\
    void unload_all() override\
    {\
        for (auto& usage : usage_counter_) usage.first->unload();\
        usage_counter_.clear();\
    }\
    private:\
