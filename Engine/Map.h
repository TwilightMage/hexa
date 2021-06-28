#pragma once

#include "Tree1D.h"

template<typename Key, typename Value>
class Map
{
public:
    struct Pair
    {
        Key key;
        Value value;
    };
    
    Map()
    {
    }

    Map(const std::initializer_list<Pair>& initializer_list)
    {
        for (const auto& pair : initializer_list)
        {
            insert(pair.key, pair.value);
        }
    }
    
    FORCEINLINE void insert(const Key& key, const Value& new_value)
    {
        data_.insert(key, new_value);
    }

    FORCEINLINE Value& find_or_insert(const Key& key, std::function<Value()> generator)
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, generator());
        return *data_.find(key);
    }
    
    Value& find_or_insert(const Key& key, const Value& new_value = Value())
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, new_value);
        return *data_.find(key);
    }

    Value& operator[](const Key& key)
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, Value());
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

    FORCEINLINE void clear()
    {
        data_.clear();
    }

    FORCEINLINE Value* find(const Key& key) const
    {
        return data_.find(key);
    }

    FORCEINLINE Value find_or_default(const Key& key) const
    {
        if (auto found = data_.find(key))
        {
            return *found;
        }
        else
        {
            return Value();
        }
    }

    FORCEINLINE Value find_or_default(const Key& key, const Value& default_value) const
    {
        if (auto found = data_.find(key))
        {
            return *found;
        }
        else
        {
            return default_value;
        }
    }

    List<Key> get_keys() const
    {
        List<Key> result;
        for (auto entry : data_)
        {
            result.Add(entry.x);
        }

        return result;
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
