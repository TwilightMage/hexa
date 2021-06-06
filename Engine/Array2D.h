#pragma once

#include <assert.h>

#include "BasicTypes.h"
#include "String.h"

template<typename T>
class Array2D
{
public:
    Array2D(uint size_x, uint size_y)
        : size_x_(size_x)
        , size_y_(size_y)
    {
        data_ = new T[size()];
        for (uint i = 0; i < size(); i++)
        {
            data_[i] = T();
        }
    }

    Array2D(const Array2D& rhs)
        : size_x_(rhs.size_x_)
        , size_y_(rhs.size_y_)
    {
        data_ = new T[size()];
        for (uint i = 0; i < size(); i++)
        {
            data_[i] = std::move(rhs.data_[i]);
        }
    }

    Array2D& operator=(const Array2D& rhs)
    {
        if (this == &rhs) return *this;
        
        if (size() != rhs.size())
        {
            size_x_ = rhs.size_x_;
            size_y_ = rhs.size_y_;

            delete[] data_;
            data_ = new T[size()];
        }

        for (uint i = 0; i < size(); i++)
        {
            data_[i] = std::move(rhs.data_[i]);
        }

        return *this;
    }

    ~Array2D()
    {
        delete[] data_;
    }

    T& at(uint x, uint y)
    {
        assert(x >= 0);
        assert(x < size_x_);
        assert(y >= 0);
        assert(y < size_y_);
        
        return data_[y * size_x_ + x];
    }

    FORCEINLINE uint get_size_x() const
    {
        return size_x_;
    }

    FORCEINLINE uint get_size_y() const
    {
        return size_y_;
    }

    FORCEINLINE uint size() const
    {
        return size_x_ * size_y_;
    }

private:
    T* data_ = nullptr;
    uint size_x_;
    uint size_y_;
};
