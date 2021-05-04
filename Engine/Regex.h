#pragma once

#include "String.h"

EXTERN class EXPORT Regex
{
public:
    Regex(const String& expression);

    bool check(const String& subject) const;

private:
    const char* expression_;
};
