#pragma once

#include <stdexcept>

#include "Array.h"
#include "BasicTypes.h"
#include "framework.h"

template<typename T>
class List : public Array<T>
{
public:
    using Array<T>::begin;
    using Array<T>::end;

    using Array<T>::GetData;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    List()
    {
    }

    List(const List& rhs)
        : List(rhs.inner, rhs.Length())
    {
    }

    List(T* inner, uint length)
    {
        allocatedLength = GetAllocateSize(length);
        this->length = length;
        if (length > 0)
        {
            this->inner = new T[allocatedLength];
            for (uint i = 0; i < length; i++)
            {
                this->inner[i] = std::move(inner[i]);
            }
        }
    }

    List(const T* inner, uint length)
    {
        allocatedLength = GetAllocateSize(length);
        this->length = length;
        if (length > 0)
        {
            this->inner = new T[allocatedLength];
            for (uint i = 0; i < length; i++)
            {
                this->inner[i] = std::move(inner[i]);
            }
        }
    }

    List(const std::vector<T>& vec)
        : List(vec.data(), static_cast<uint>(vec.size()))
    {
    }

    List(const std::initializer_list<T>& il)
        : List(static_cast<const T*>(il.begin()), static_cast<uint>(il.size()))
    {
    }

    List& operator=(const List& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner;

        length = rhs.length;
        allocatedLength = rhs.allocatedLength;

        if (length > 0)
        {
            inner = new T[allocatedLength];
            for (uint i = 0; i < length; i++)
            {
                inner[i] = std::move(rhs.inner[i]);
            }
        }

        return *this;
    }

    void Add(const T& item)
    {
        if (length == allocatedLength)
        {
            Reallocate(GetAllocateSize(length + 1));
        }

        inner[length] = std::move(item);
        length++;
    }

    void RemoveAt(uint index)
    {
        if (index >= length)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        if (index < length - 1)
        {
            for (uint i = index; i < length - 1; i++)
            {
                inner[i] = inner[i + 1];
            }
        }
        
        length--;
    }

    void Remove(const T& item)
    {
        uint entriesAmount = 0;
        uint* entries = new uint[length];

        for (uint i = 0; i < length; i++)
        {
            if (inner[i] == item)
            {
                entries[entriesAmount++] = i;
            }
        }

        // TODO: Remove memcpy or start using malloc and free
        for (uint i = 0; i < entriesAmount - 1; i++)
        {
            memcpy(inner + entries[i] - i, inner + entries[i] + 1, sizeof(T) * (entries[i + 1] - entries[i] - 1));
        }

        // Don't try to understand, I was drunk
        memcpy(inner + entries[entriesAmount - 1] - entriesAmount + 1, inner + entries[entriesAmount - 1] + 1, sizeof(T) * (length - entries[entriesAmount - 1] - 1));

        length -= entriesAmount;

        delete[] entries;
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

    void Insert(const T& item, uint indexAt)
    {
        if (indexAt > length)
        {
            throw new std::out_of_range("Parameter \"indexAt\" is greater than last available index");
        }

        T item_copy = item;
        if (length == allocatedLength)
        {
            Reallocate(GetAllocateSize(length + 1));
        }

        ++length;
        
        if (indexAt < length)
        {
            for (uint i = 0; i < length - indexAt - 1; i++)
            {
                inner[length - i - 1] = std::move(inner[length - i - 2]);
            }
        }

        inner[indexAt] = std::move(item_copy);
    }

private:
    using Array<T>::GetAllocateSize;
    using Array<T>::Reallocate;
    using Array<T>::Slack;

    using Array<T>::inner;
    using Array<T>::length;
    using Array<T>::allocatedLength;
};
