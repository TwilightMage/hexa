﻿#pragma once

#include "framework.h"

template<typename T>
class Array
{
public:   
    ~Array()
    {
        delete[] inner_;
    }

    FORCEINLINE const T* begin() const
    {
        return inner_;
    }

    FORCEINLINE const T* end() const
    {
        return inner_ + length_;
    }

    FORCEINLINE T* begin()
    {
        return inner_;
    }

    FORCEINLINE T* end()
    {
        return inner_ + length_;
    }

    FORCEINLINE [[nodiscard]] uint length() const
    {
        return length_;
    }

    FORCEINLINE T* get_data()
    {
        return inner_;
    }

    FORCEINLINE const T* get_data() const
    {
        return inner_;
    }

    FORCEINLINE bool operator==(const Array& rhs) const
    {
        if (length_ != rhs.length_) return false;

        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] != rhs.inner_[i]) return false;
        }

        return true;
    }

    FORCEINLINE bool operator!=(const Array& rhs) const
    {
        return !operator==(rhs);
    }

protected:
    static uint get_allocate_size(uint len)
    {
        if (len == 0) return 0;

        uint result = 1;
        while (result < len)
        {
            result *= 2;
        }

        return result;
    }

    void reallocate(uint newLength)
    {
        T* new_inner = newLength > 0 ? new T[newLength] : nullptr;

        for (uint i = 0; i < (length_ < newLength ? length_ : newLength); i++)
        {
            new_inner[i] = std::move(inner_[i]);
        }

        delete[] inner_;
        inner_ = new_inner;

        allocated_length_ = newLength;
    }

    void slack()
    {
        const uint desired_size = get_allocate_size(length_);

        if (desired_size != allocated_length_)
        {
            reallocate(desired_size);
        }
    }

    T* inner_ = nullptr;
    uint length_ = 0;
    uint allocated_length_ = 0;
};
