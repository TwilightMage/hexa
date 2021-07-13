#include "Name.h"

Name::Name()
    : data_(nullptr)
    , data_size_(0)
    , hash_(0)
{
}

Name::Name(const Name& rhs)
{
    if (rhs.hash_ != 0)
    {
        data_size_ = rhs.data_size_;
        data_ = new char[data_size_];
        memcpy(data_, rhs.data_, data_size_);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        data_size_ = 0;
        hash_ = 0;
    }
}

Name::Name(const char* c_str)
{
    const auto len = strlen(c_str);
    if (len > 0)
    {
        data_ = new char[len];
        data_size_ = (uint)len;
        memcpy(data_, c_str, data_size_);

        const int p = 31;
        const int m = 1e9 + 9;
        hash_ = 0;
        uint64 p_pow = 1;
        for (uint i = 0; i < len; i++)
        {
            hash_ = (hash_ + (data_[i] - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
    }
    else
    {
        data_ = nullptr;
        data_size_ = 0;
        hash_ = 0;
    }
}

Name::Name(const String& str)
    : Name(str.c())
{
}

Name::~Name()
{
    delete[] data_;
}

Name& Name::operator=(const Name& rhs)
{
    if (&rhs == this) return *this;

    if (rhs.hash_ != 0)
    {
        data_size_ = rhs.data_size_;
        data_ = new char[data_size_];
        memcpy(data_, rhs.data_, data_size_);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        data_size_ = 0;
        hash_ = 0;
    }

    return *this;
}
