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

    using Array<T>::get_data;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    List()
    {
    }

    List(const List& rhs)
        : List(rhs.inner_, rhs.length())
    {
    }

    List(T* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = std::move(inner[i]);
            }
        }
    }

    List(const T* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = std::move(inner[i]);
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

    List(uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = T();
            }
        }
    }

    List& operator=(const List& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner_;

        length_ = rhs.length_;
        allocated_length_ = rhs.allocated_length_;

        if (length_ > 0)
        {
            inner_ = new T[allocated_length_];
            for (uint i = 0; i < length_; i++)
            {
                inner_[i] = std::move(rhs.inner_[i]);
            }
        }

        return *this;
    }

    void Add(const T& item)
    {
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        inner_[length_] = std::move(item);
        length_++;
    }

    bool Contains(const T& item)
    {
        if (length_ == 0) return false;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return true;
        }
        return false;
    }

    int IndexOf(const T& item)
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return i;
        }
        return -1;
    }

    void RemoveAt(uint index)
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        if (index < length_ - 1)
        {
            for (uint i = index; i < length_ - 1; i++)
            {
                inner_[i] = inner_[i + 1];
            }
        }
        
        length_--;
    }

    void Remove(const T& item)
    {
        uint entriesAmount = 0;
        uint* entries = new uint[length_];

        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item)
            {
                entries[entriesAmount++] = i;
            }
        }

        // TODO: Remove memcpy or start using malloc and free
        for (uint i = 0; i < entriesAmount - 1; i++)
        {
            memcpy(inner_ + entries[i] - i, inner_ + entries[i] + 1, sizeof(T) * (entries[i + 1] - entries[i] - 1));
        }

        // Don't try to understand, I was drunk
        memcpy(inner_ + entries[entriesAmount - 1] - entriesAmount + 1, inner_ + entries[entriesAmount - 1] + 1, sizeof(T) * (length_ - entries[entriesAmount - 1] - 1));

        length_ -= entriesAmount;

        delete[] entries;
    }

    T& operator[](uint index)
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    const T& operator[](uint index) const
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    void Insert(const T& item, uint indexAt)
    {
        if (indexAt > length_)
        {
            throw new std::out_of_range("Parameter \"indexAt\" is greater than last available index");
        }

        T item_copy = item;
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        ++length_;
        
        if (indexAt < length_)
        {
            for (uint i = 0; i < length_ - indexAt - 1; i++)
            {
                inner_[length_ - i - 1] = std::move(inner_[length_ - i - 2]);
            }
        }

        inner_[indexAt] = std::move(item_copy);
    }

    void Clear()
    {
        reallocate(0);
        length_ = 0;
    }

private:
    using Array<T>::get_allocate_size;
    using Array<T>::reallocate;
    using Array<T>::slack;

    using Array<T>::inner_;
    using Array<T>::length_;
    using Array<T>::allocated_length_;
};
