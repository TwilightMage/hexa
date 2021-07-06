#pragma once

template<typename K, typename V>
struct Pair
{
    K key;
    V value;

    Pair(const K& key, const V& value)
        : key(key)
        , value(value)
    {
    }
    
    bool operator==(const Pair& rhs) const
    {
        return key == rhs.key = value == rhs.value;
    }
};
