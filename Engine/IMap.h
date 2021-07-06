#pragma once
#include "BasicTypes.h"
#include "List.h"
#include "Pair.h"

template<typename K, typename V>
class IMap
{
public:
    virtual uint size() const = 0;
    virtual V& operator[](const K& key) = 0;
    virtual const V& operator[](const K& key) const = 0;
    virtual V& at(const K& key) = 0;
    virtual const V& at(const K& key) const = 0;
    virtual void remove(const K& key) = 0;
    virtual bool contains(const K& key) const = 0;
    virtual V* find(const K& key) const = 0;
    virtual void clear() = 0;
    virtual List<K> get_keys() const = 0;
};
