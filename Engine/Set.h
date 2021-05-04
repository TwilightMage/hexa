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

    using Array<T>::get_data;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    Set()
    {
        inner_ = nullptr;
        length_ = 0;
        allocated_length_ = 0;
    }

    Set(const Set& rhs)
    {
        inner_ = new T[rhs.allocated_length_];
        length_ = rhs.length_;
        allocated_length_ = rhs.allocated_length_;

        memcpy(inner_, rhs.inner_, sizeof(T) * length_);
    }

    Set(T* inner_, uint length_)
    {
        allocated_length_ = get_allocated_size(length_);
        this->inner_ = new T[allocated_length_];

        for (uint i = 0; i < length_; i++)
        {
            Add(inner_[i]);
        }

        slack();
    }

    Set(const std::set<T, Compare>& set)
    {
        inner_ = new T[set.size()];
        length_ = static_cast<uint>(set.size());
        allocated_length_ = get_allocated_size(length_);

        memcpy(inner_, set.data(), sizeof(T) * set.size());
    }

    Set(const std::initializer_list<T>& il)
    {
        allocated_length_ = get_allocated_size(il.size());
        inner_ = new T[allocated_length_];

        for (uint i = 0; i < il.size(); i++)
        {
            Add(il[i]);
        }

        slack();
    }

    Set& operator=(const Set& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner_;

        length_ = rhs.length_;
        allocated_length_ = rhs.allocated_length_;

        inner_ = new T[allocated_length_];
        memcpy(inner_, rhs.inner_, sizeof(T) * length_);

        return *this;
    }

    void Add(const T& item)
    {
        Compare compare;

        if (length_ == allocated_length_)
        {
            reallocate(get_allocated_size(length_ + 1));
        }

        bool inserted = false;
        for (uint i = 0; i < length_; i++)
        {
            if (compare(item, inner_[i]))
            {
                memcpy_b(inner_ + (i + 1), inner_ + i, sizeof(T) * (length_ - i));

                inner_[i] = std::move(item);

                inserted = true;

                break;
            }
            else if (!compare(inner_[i], item))
            {
                return;
            }
        }

        if (!inserted)
        {
            inner_[length_] = std::move(item);
        }

        length_++;
    }

    void Remove(const T& item)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item)
            {
                memcpy(inner_ + (i + 1), inner_ + i, sizeof(T) * (length_ - i - 1));

                length_--;

                break;
            }
        }
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

private:
    using Array<T>::get_allocated_size;
    using Array<T>::reallocate;
    using Array<T>::slack;

    using Array<T>::inner_;
    using Array<T>::length_;
    using Array<T>::allocated_length_;
};
