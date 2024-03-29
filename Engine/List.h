﻿#pragma once

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
        inner = nullptr;
        length = 0;
        allocatedLength = 0;
    }

    List(const List& rhs)
    {
        inner = new T[rhs.allocatedLength];
        length = rhs.length;
        allocatedLength = rhs.allocatedLength;

        memcpy(inner, rhs.inner, sizeof(T) * length);
    }

    List(T* inner, uint length)
    {
        allocatedLength = GetAllocateSize(length);

        if (length == allocatedLength)
        {
            this->inner = inner;
        }
        else
        {
            this->inner = new T[allocatedLength];
            memcpy(this->inner, inner, sizeof(T) * length);
        }
        this->length = length;
    }

    List(const std::vector<T>& vec)
    {
        allocatedLength = GetAllocateSize(length);

        inner = new T[allocatedLength];
        length = static_cast<uint>(vec.size());

        memcpy(inner, vec.data(), sizeof(T) * vec.size());
    }

    List(const std::initializer_list<T>& il)
    {
        allocatedLength = GetAllocateSize(static_cast<uint>(il.size()));

        inner = new T[allocatedLength];
        length = static_cast<uint>(il.size());


        memcpy(inner, il.begin(), sizeof(T) * il.size());
    }

    List& operator=(const List& rhs)
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

        memcpy(inner + (index + 1), inner + index, sizeof(T) * (length - index - 1));
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

        if (length == allocatedLength)
        {
            Reallocate(GetAllocateSize(length + 1));
        }

        if (indexAt < length)
        {
            memcpy_b(inner + (indexAt + 1), inner + indexAt, sizeof(T) * (length - indexAt));
        }

        inner[indexAt] = std::move(item);

        length++;
    }

private:
    using Array<T>::GetAllocateSize;
    using Array<T>::Reallocate;
    using Array<T>::Slack;

    using Array<T>::inner;
    using Array<T>::length;
    using Array<T>::allocatedLength;
};
