#pragma once

#include <set>
#include <stdexcept>

#include "Array.h"
#include "framework.h"

template<typename T>
class Set : public Array<T>
{
    using Array<T>::get_allocate_size;
    using Array<T>::reallocate;
    using Array<T>::slack;
    
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
        reallocate(rhs.length_);

        for (uint i = 0; i < length_; i++)
        {
            inner_[i] = std::move(rhs.inner_[i]);
        }
    }

    Set(T* data_ptr, uint length)
    {
        reallocate(length);

        for (uint i = 0; i < length; i++)
        {
            Add(data_ptr[i]);
        }

        slack();
    }

    Set(const std::initializer_list<T>& il)
    {
        reallocate(il.size());

        for (uint i = 0; i < il.size(); i++)
        {
            Add(il[i]);
        }

        slack();
    }

    Set& operator=(const Set& rhs)
    {
        if (this == &rhs) return *this;

        reallocate(rhs.length_);

        for (uint i = 0; i < length_; i++)
        {
            inner_[i] = std::move(rhs.inner_[i]);
        }

        return *this;
    }

    void add(const T& item)
    {
        uint l = 0;
        uint r = length_;
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (inner_[m] < item)
            {
                l = m + 1;
            }
            else if (inner_[m] > item)
            {
                r = m;
            }
            else
            {
                return;
            }
        }

        m = (l + r) / 2;

        T item_copy = item;
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        ++length_;
        
        if (m < length_)
        {
            for (uint i = 0; i < length_ - m - 1; i++)
            {
                inner_[length_ - i - 1] = std::move(inner_[length_ - i - 2]);
            }
        }

        inner_[m] = std::move(item_copy);
    }

    void remove(const T& item)
    {
        uint l = 0;
        uint r = length_;
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (inner_[m] < item)
            {
                l = m + 1;
            }
            else if (inner_[m] > item)
            {
                r = m;
            }
            else
            {
                if (m < length_ - 1)
                {
                    for (uint i = m; i < length_ - 1; i++)
                    {
                        inner_[i] = std::move(inner_[i + 1]);
                    }
                }

                inner_[length_ - 1] = T();
        
                --length_;
                
                return;
            }
        }
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
    using Array<T>::inner_;
    using Array<T>::length_;
    using Array<T>::allocated_length_;
};
