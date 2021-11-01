#pragma once

#include "BasicTypes.h"
#include "FastOperator.h"
#include "Map.h"
#include "String.h"

class EXPORT Name
{
public:
    Name();
    Name(const Name& rhs);
    Name(const char* c_str);
    explicit Name(const String& str);
    ~Name();

    FAST_OPERATOR(Name, ==, hash_);
    FAST_OPERATOR(Name, <=, hash_);
    FAST_OPERATOR(Name, <, hash_);
    FAST_OPERATOR(Name, !=, hash_);
    FAST_OPERATOR(Name, >, hash_);
    FAST_OPERATOR(Name, >=, hash_);

    Name& operator=(const Name& rhs);

    FORCEINLINE String to_string() const { return String(data_); }

    FORCEINLINE bool is_valid() const { return hash_ == 0; }

private:
    char* data_;
    uint size_;
    uint64 hash_;
};
