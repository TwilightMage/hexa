#include <iostream>

#include "pch.h"
#include "ExampleMod.h"
#include "Engine/framework.h"
#include "Engine/Mod.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

EXTERN Mod EXPORT *GetMod()
{
    return new ExampleMod();
}