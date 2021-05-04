#include "Mod.h"


#include "File.h"
#include "Game.h"
#include "Path.h"

typedef Mod* (__stdcall *f_get_mod)();

const ModInfo& Mod::get_mod_info() const
{
    return info_;
}

bool Mod::verify_signature(const Path& path)
{
    if (!path.exists()) return false;
    if (!path.get_child(path.filename + ".dll").exists()) return false;
    if (!path.get_child(path.filename + ".meta").exists()) return false;

    return true;
}

Path Mod::mod_path(const String& subPath) const
{
    return "./mods/" + info_.name + "/" + subPath;
}

void Mod::loading_stage()
{
}

void Mod::on_loaded(EventBus* event_bus)
{
}

Shared<Mod> Mod::load(const Path& path)
{
    try
    {
        if (HINSTANCE dll = LoadLibrary(path.get_absolute().to_string().wc()))
        {
            if (f_get_mod get_mod = (f_get_mod)GetProcAddress(dll, "get_mod"))
            {
                Shared<Mod> mod = Shared<Mod>(get_mod());
                mod->dll = dll;
                return mod;
            }
            else
            {
                print_error("Mod Loader", "Unable to find entry point get_mod in mod %s", (path.filename + path.extension).c());
                FreeLibrary(dll);
            }
        }
        else
        {
            print_error("Mod Loader", "Unknown error on loading mod %s", (path.filename + path.extension).c());
        }
    }
    catch (std::exception exc)
    {
        print_error("Mod Loader", "Failed to load mod %s: %s", (path.filename + path.extension).c(), exc.what());
    }

    return nullptr;
}

ModInfo Mod::load_mod_info(const Path& path)
{
    ModInfo result;
    result.name = path.filename;

    return result;
}
