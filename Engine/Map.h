#pragma once

#include "Tree1D.h"

template<typename Key, typename Value>
class Map
{
public:
    FORCEINLINE void insert(const Key& key, const Value& new_value)
    {
        data_.insert(key, new_value);
    }

    FORCEINLINE Value& find_or_insert(const Key& key, const Value& new_value = Value())
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, new_value);
        return *data_.find(key);
    }

    FORCEINLINE bool contains(const Key& key)
    {
        return data_.find(key) != nullptr;
    }

    FORCEINLINE void remove(const Key& key)
    {
        data_.remove(key);
    }

    FORCEINLINE Value* find(const Key& key) const
    {
        return data_.find(key);
    }

    FORCEINLINE uint size() const
    {
        return data_.size();
    }

    FORCEINLINE typename Tree1D<Key, Value>::Iterator begin() const
    {
        return data_.begin();
    }

    FORCEINLINE typename Tree1D<Key, Value>::Iterator end() const
    {
        return data_.end();
    }

private:
    Tree1D<Key, Value> data_;
};
