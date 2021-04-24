#pragma once

#include "framework.h"
#include "String.h"
#include "Version.h"

EXTERN struct EXPORT ModInfo
{
    void ReadFrom(const String& path);

    String name;
    String display_name;
    Version mod_version;
    Version target_game_version;
};
