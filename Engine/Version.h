#pragma once

#include "BasicTypes.h"
#include "framework.h"

EXTERN struct EXPORT Version
{
    Version();
    Version(uint version_number, uint patch_number, uint fix_number);

    bool operator <(const Version& rhs) const;
    bool operator >(const Version& rhs) const;
    bool operator <=(const Version& rhs) const;
    bool operator >=(const Version& rhs) const;
    bool operator ==(const Version& rhs) const;
    bool operator !=(const Version& rhs) const;

    uint version_number;
    uint patch_number;
    uint fix_number;
};
