#pragma once

#include "framework.h"

template<typename T>
class Array
{
public:   
    ~Array()
    {
        delete[] inner_;
    }

    const T* begin() const
    {
        return inner_;
    }

    const T* end() const
    {
        return inner_ + length_;
    }

    T* begin()
    {
        return inner_;
    }

    T* end()
    {
        return inner_ + length_;
    }

    [[nodiscard]] uint Length() const
    {
        return length_;
    }

    T* GetData()
    {
        return inner_;
    }

    const T* GetData() const
    {
        return inner_;
    }

    bool operator==(const Array& rhs) const
    {
        if (length_ != rhs.length_) return false;

        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] != rhs[i]) return false;
        }

        return true;
    }

    bool operator!=(const Array& rhs) const
    {
        return !operator==(rhs);
    }

protected:
    static uint GetAllocateSize(uint len)
    {
        if (len == 0) return 0;

        uint result = 1;
        while (result < len)
        {
            result *= 2;
        }

        return result;
    }

    void Reallocate(uint newLength)
    {
        T* newInner = newLength > 0 ? new T[newLength] : nullptr;

        for (uint i = 0; i < (length_ < newLength ? length_ : newLength); i++)
        {
            newInner[i] = std::move(inner_[i]);
        }

        delete[] inner_;
        inner_ = newInner;

        allocated_length_ = newLength;
    }

    void Slack()
    {
        const uint desiredSize = GetAllocateSize(length_);

        if (desiredSize != allocated_length_)
        {
            Reallocate(desiredSize);
        }
    }

    T* inner_ = nullptr;
    uint length_ = 0;
    uint allocated_length_ = 0;
};
