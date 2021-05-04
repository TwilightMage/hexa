#include "Version.h"

Version::Version()
    : version_number(0)
    , patch_number(0)
    , fix_number(0)
{}

Version::Version(uint version_number, uint patch_number, uint fix_number)
    : version_number(version_number)
    , patch_number(patch_number)
    , fix_number(fix_number)
{}

Version::Version(const String& version_string)
{
    auto parts = version_string.split('.');
    version_number = StringParse<uint>(parts[0]);
    patch_number = StringParse<uint>(parts[1]);
    fix_number = StringParse<uint>(parts[2]);
}

String Version::to_string() const
{
    return StringFormat("%i.%i.%i", version_number, patch_number, fix_number);
}

bool Version::operator<(const Version& rhs) const
{
    if (version_number < rhs.version_number) return true;
    if (version_number == rhs.version_number)
    {
        if (patch_number < rhs.patch_number) return true;
        if (patch_number == rhs.patch_number)
        {
            if (patch_number < rhs.patch_number) return true;
        }
    }

    return false;
}

bool Version::operator>(const Version& rhs) const
{
    if (version_number > rhs.version_number) return true;
    if (version_number == rhs.version_number)
    {
        if (patch_number > rhs.patch_number) return true;
        if (patch_number == rhs.patch_number)
        {
            if (patch_number > rhs.patch_number) return true;
        }
    }

    return false;
}

bool Version::operator <=(const Version& rhs) const
{
    return version_number <= rhs.version_number && patch_number <= rhs.patch_number && fix_number <= rhs.fix_number;
}

bool Version::operator >=(const Version& rhs) const
{
    return version_number >= rhs.version_number && patch_number >= rhs.patch_number && fix_number >= rhs.fix_number;
}

bool Version::operator ==(const Version& rhs) const
{
    return version_number == rhs.version_number && patch_number == rhs.patch_number && fix_number == rhs.fix_number;
}

bool Version::operator !=(const Version& rhs) const
{
    return version_number != rhs.version_number || patch_number != rhs.patch_number || fix_number != rhs.fix_number;
}
