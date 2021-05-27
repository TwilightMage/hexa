#pragma once

#include <assert.h>

#include "BasicTypes.h"

template<typename T>
class Array2D
{
public:
    Array2D(uint size_x, uint size_y)
        : size_x_(size_x)
        , size_y_(size_y)
    {
        data_ = new T*[size_x];
        for (uint i = 0; i < size_x; i++)
        {
            data_[i] = new T[size_y];
            for (uint j = 0; j < size_y; j++)
            {
                data_[i][j] = T();
            }
        }
    }

    ~Array2D()
    {
        for (uint i = 0; i < size_x_; i++)
        {
            delete[] data_[i];
        }
        delete[] data_;
    }

    T& at(uint x, uint y)
    {
        assert(x >= 0);
        assert(x < size_x_);
        assert(y >= 0);
        assert(y < size_y_);
        
        return data_[x][y];
    }

    uint get_size_x() const
    {
        return size_x_;
    }

    uint get_size_y() const
    {
        return size_y_;
    }

private:
    T** data_;
    uint size_x_;
    uint size_y_;
};
