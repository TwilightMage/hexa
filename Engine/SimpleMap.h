#pragma once

#include "IMap.h"
#include "List.h"
#include "Pair.h"

template<typename K, typename V>
struct SimpleMap : public IMap<K, V>
{
    List<Pair<K, V>*> entries;
    
    V& operator[](const K& key) override
    {
        return at(key);
    }

    const V& operator[](const K& key) const override
    {
        return at(key);
    }

    void clear() override
    {
        entries.clear();
    }

    Pair<K, V>** begin()
    {
        return entries.begin();
    }

    Pair<K, V>** end()
    {
        return entries.end();
    }

    const Pair<K, V>** begin() const
    {
        return entries.begin();
    }

    const Pair<K, V>** end() const
    {
        return entries.end();
    }

    uint size() const override
    {
        return entries.length();
    }

    void remove(const K& key) override
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                entries.remove_at(m);
                return;;
            }
        }
    }

    bool contains(const K& key) const override
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    V& at(const K& key) override
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return entries[m]->value;
            }
        }

        m = (l + r) / 2;
        
        entries.insert(new Pair<K, V>(key, V()), m);
        return entries[m]->value;
    }

    const V& at(const K& key) const override
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return entries[m]->value;
            }
        }

        throw std::runtime_error("Unable to find value in simple map");
    }

    V* find(const K& key) const override
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return &entries[m]->value;
            }
        }

        return nullptr;
    }

    List<K> get_keys() const override
    {
        List<K> result;

        for (auto& entry : entries)
        {
            result.add(entry->key);
        }

        return result;
    }
};
