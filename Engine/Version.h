#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "String.h"

struct EXPORT Version
{
    Version();
    Version(uint version_number, uint patch_number, uint fix_number);
    Version(const String& version_string);

    String to_string() const;

    bool operator<(const Version& rhs) const;
    bool operator>(const Version& rhs) const;
    bool operator<=(const Version& rhs) const;
    bool operator>=(const Version& rhs) const;
    bool operator==(const Version& rhs) const;
    bool operator!=(const Version& rhs) const;

    uint version_number;
    uint patch_number;
    uint fix_number;
};
