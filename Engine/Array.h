#pragma once

#include "BasicTypes.h"

template<typename T>
class Array
{
public:
    ~Array()
    {
        delete[] inner;
    }

    T* begin()
    {
        return inner;
    }

    T* end()
    {
        return inner + length;
    }

    [[nodiscard]] uint Length() const
    {
        return length;
    }

    T* GetData()
    {
        return inner;
    }

    const T* GetData() const
    {
        return inner;
    }

    bool operator==(const Array& rhs) const
    {
        if (length != rhs.length) return false;

        for (uint i = 0; i < length; i++)
        {
            if (inner[i] != rhs[i]) return false;
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

        if (newLength > 0)
        {
            memcpy(newInner, inner, sizeof(T) * length);
        }

        delete[] inner;
        inner = newInner;

        allocatedLength = newLength;
    }

    void Slack()
    {
        const uint desiredSize = GetAllocateSize(length);

        if (desiredSize != allocatedLength)
        {
            Reallocate(desiredSize);
        }
    }

    T* inner = nullptr;
    uint length = 0;
    uint allocatedLength = 0;
};
