#include "pch.h"
#include "Engine/Set.h"

#include "HexaGame/HexaGame.h"

/*typedef Mod*(*f_mod_get)();

enum class EModLoadResult
{
    Success = 0,
    ModuleNotFound = 1,
    EntryPointNotFound = 2
};

std::vector<Shared<Mod>> loaded_mods;

EModLoadResult LoadMod(const Path& path)
{
    if (const HINSTANCE dll = LoadLibrary(path.GetChild(path.filename + ".dll").ToString().wc()))
    {
        if (const auto mod_get = (f_mod_get)GetProcAddress(dll, "GetMod"))
        {
            ModInfo info;
            info.ReadFrom(path.GetChild(path.filename + ".meta").ToString());

            Mod* mod = mod_get();
            mod->Init(info);
            std::cout << "Mod " << mod->GetModInfo().display_name << " Loaded" << std::endl;

            loaded_mods.push_back(Shared<Mod>(mod));

            return EModLoadResult::Success;
        }
        return EModLoadResult::EntryPointNotFound;
    }
    return EModLoadResult::ModuleNotFound;
}

void UnloadMod(Shared<Mod>)
{
    
}*/

int main(int argc, char* argv[])
{
    HexaGame game(argc, argv);
    game.launch();

    /*for (const auto& entry : Path("mods").List())
    {
        if (Mod::VerifySignature(entry))
        {
            LoadMod(entry);
        }
    }*/

    return 0;
}
