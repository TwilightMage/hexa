#pragma once

#include <map>

#include "UsageCounterBase.h"

template<class C>
class UsageCounter : public UsageCounterBase
{
public:
    bool increase(IUsageCountableBase* ptr) override
    {
        if (auto t_ptr = cast<C>(ptr))
        {
            auto& value = counter_[t_ptr];
            return value++ == 0;
        }
        return false;
    }
    
    bool decrease(IUsageCountableBase* ptr) override
    {
        auto t_ptr = cast<C>(ptr);
        {
            auto& value = counter_[t_ptr];
            if (--value == 0)
            {
                counter_.erase(t_ptr);
                return true;
            }
        }
        return false;
    }

    void clear() override
    {
        counter_.clear();
    }

    uint count(IUsageCountableBase* ptr) const override
    {
        if (auto t_ptr = cast<C>(ptr))
        {
            for (const auto& kvp : counter_)
            {
                if (kvp.first == t_ptr) return kvp.second;
            }
        }
        return 0;
    }

    typename std::map<C*, uint>::iterator begin()
    {
        return counter_.begin();
    }

    typename std::map<C*, uint>::iterator end()
    {
        return counter_.end();
    }

    typename std::map<C*, uint>::const_iterator begin() const
    {
        return counter_.begin();
    }

    typename std::map<C*, uint>::const_iterator end() const
    {
        return counter_.end();
    }

    std::map<String, uint> dump() const
    {
        std::map<String, uint> result;
        for (const auto& kvp : counter_)
        {
            if (result.count(kvp.first->get_name()) == 1)
            {
                uint i = 1;
                String alt;
                do
                {
                    alt = String::format("%s (%i)", kvp.first->get_name().c(), i++);
                } while (result.count(alt) == 1);
                result[alt] = kvp.second;
            }
            else
            {
                result[kvp.first->get_name()] = kvp.second;
            }
        }

        return result;
    }

private:
    std::map<C*, uint> counter_;
};
