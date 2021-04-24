#pragma once

#include <string>

#include "pch.h"
#include "ModInfo.h"
#include "Path.h"

EXTERN class EXPORT Mod
{
public:
    void Init(const ModInfo& newInfo);

    const ModInfo& GetModInfo() const;

    static bool VerifySignature(const Path& path);

private:
    ModInfo info;
};
