#pragma once
#include "List.h"

template<typename K, typename V>
struct SimpleMap
{
    struct Entry
    {
        K key;
        V value;

        bool operator==(const Entry& rhs) const
        {
            return key == rhs.key = value == rhs.value;
        }
    };
    
    List<Entry*> entries;
    
    V& operator[](const K& key)
    {
        return get_entry(key).value;
    }

    void clear()
    {
        entries.Clear();
    }

    Entry** begin()
    {
        return entries.begin();
    }

    Entry** end()
    {
        return entries.end();
    }

    const Entry** begin() const
    {
        return entries.begin();
    }

    const Entry** end() const
    {
        return entries.end();
    }

    uint size()
    {
        return entries.length();
    }

    void remove_key(const K& key)
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
                entries.RemoveAt(m);
                return;;
            }
        }
    }

    bool have_key(const K& key)
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

    Entry& get_entry(const K& key)
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
                return *entries[m];
            }
        }

        m = (l + r) / 2;
        
        entries.Insert(new Entry(key, V()), m);
        return *entries[m];
    }

    Entry* find_entry(const K& key)
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
                return entries[m];
            }
        }

        return nullptr;
    }
};
