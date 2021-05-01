#include "Mod.h"


#include "File.h"
#include "Game.h"
#include "Path.h"

typedef Mod* (__stdcall *f_get_mod)();

void Mod::init(EventBus* event_bus)
{
    info_ = load_mod_info();
    on_init(event_bus);
}

const ModInfo& Mod::get_mod_info() const
{
    return info_;
}

bool Mod::verify_signature(const Path& path)
{
    if (!path.Exists()) return false;
    if (!path.GetChild(path.filename + ".dll").Exists()) return false;
    if (!path.GetChild(path.filename + ".meta").Exists()) return false;

    return true;
}

Shared<Mod> Mod::load(const Path& path)
{
    try
    {
        if (HINSTANCE dll = LoadLibrary(path.get_absolute().ToString().wc()))
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

void Mod::on_init(EventBus* event_bus)
{
}

ModInfo Mod::load_mod_info()
{
    ModInfo result;

    return result;
}
