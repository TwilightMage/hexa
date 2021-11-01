#include "Name.h"

Name::Name()
    : data_(nullptr)
    , size_(0)
    , hash_(0)
{
}

Name::Name(const Name& rhs)
{
    if (rhs.hash_ != 0)
    {
        size_ = rhs.size_;
        data_ = new char[size_ + 1];
        memcpy(data_, rhs.data_, size_ + 1);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }
}

Name::Name(const char* c_str)
{
    size_ = strlen(c_str);
    if (size_ > 0)
    {
        data_ = new char[size_ + 1];
        memcpy(data_, c_str, size_ + 1);

        const int p = 31;
        const int m = 1e9 + 9;
        hash_ = 0;
        uint64 p_pow = 1;
        for (uint i = 0; i < size_; i++)
        {
            hash_ = (hash_ + (data_[i] - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
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
        size_ = rhs.size_;
        data_ = new char[size_ + 1];
        memcpy(data_, rhs.data_, size_ + 1);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }

    return *this;
}
