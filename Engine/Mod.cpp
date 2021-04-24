#include "Mod.h"


#include "File.h"
#include "Path.h"

void Mod::Init(const ModInfo& newInfo)
{
    info = newInfo;
}

const ModInfo& Mod::GetModInfo() const
{
    return info;
}

bool Mod::VerifySignature(const Path& path)
{
    if (!path.Exists()) return false;
    if (!path.GetChild(path.filename + ".dll").Exists()) return false;
    if (!path.GetChild(path.filename + ".meta").Exists()) return false;

    return true;
}
