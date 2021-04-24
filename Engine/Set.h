#pragma once

#include <set>
#include <stdexcept>

#include "Array.h"
#include "framework.h"

template<typename T, class Compare = std::less<T>>
class Set : public Array<T>
{
public:
    using Array<T>::begin;
    using Array<T>::end;

    using Array<T>::GetData;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    Set()
    {
        inner = nullptr;
        length = 0;
        allocatedLength = 0;
    }

    Set(const Set& rhs)
    {
        inner = new T[rhs.allocatedLength];
        length = rhs.length;
        allocatedLength = rhs.allocatedLength;

        memcpy(inner, rhs.inner, sizeof(T) * length);
    }

    Set(T* inner, uint length)
    {
        allocatedLength = GetAllocateSize(length);
        this->inner = new T[allocatedLength];

        for (uint i = 0; i < length; i++)
        {
            Add(inner[i]);
        }

        Slack();
    }

    Set(const std::set<T, Compare>& set)
    {
        inner = new T[set.size()];
        length = static_cast<uint>(set.size());
        allocatedLength = GetAllocateSize(length);

        memcpy(inner, set.data(), sizeof(T) * set.size());
    }

    Set(const std::initializer_list<T>& il)
    {
        allocatedLength = GetAllocateSize(il.size());
        inner = new T[allocatedLength];

        for (uint i = 0; i < il.size(); i++)
        {
            Add(il[i]);
        }

        Slack();
    }

    Set& operator=(const Set& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner;

        length = rhs.length;
        allocatedLength = rhs.allocatedLength;

        inner = new T[allocatedLength];
        memcpy(inner, rhs.inner, sizeof(T) * length);

        return *this;
    }

    void Add(const T& item)
    {
        Compare compare;

        if (length == allocatedLength)
        {
            Reallocate(GetAllocateSize(length + 1));
        }

        bool inserted = false;
        for (uint i = 0; i < length; i++)
        {
            if (compare(item, inner[i]))
            {
                memcpy_b(inner + (i + 1), inner + i, sizeof(T) * (length - i));

                inner[i] = std::move(item);

                inserted = true;

                break;
            }
            else if (!compare(inner[i], item))
            {
                return;
            }
        }

        if (!inserted)
        {
            inner[length] = std::move(item);
        }

        length++;
    }

    void Remove(const T& item)
    {
        for (uint i = 0; i < length; i++)
        {
            if (inner[i] == item)
            {
                memcpy(inner + (i + 1), inner + i, sizeof(T) * (length - i - 1));

                length--;

                break;
            }
        }
    }

    T& operator[](uint index)
    {
        if (index >= length)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner[index];
    }

    const T& operator[](uint index) const
    {
        if (index >= length)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner[index];
    }

private:
    using Array<T>::GetAllocateSize;
    using Array<T>::Reallocate;
    using Array<T>::Slack;

    using Array<T>::inner;
    using Array<T>::length;
    using Array<T>::allocatedLength;
};
