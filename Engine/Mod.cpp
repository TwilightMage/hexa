#include "Mod.h"


#include "File.h"
#include "Game.h"
#include "Path.h"

typedef Mod* (__stdcall *f_get_mod)();

const Mod::Info& Mod::get_mod_info() const
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

Path Mod::mod_path(const String& sub_path) const
{
    return "./mods/" + info_.name + "/" + sub_path;
}

void Mod::loading_stage()
{
}

void Mod::on_loaded(const Shared<EventBus>& event_bus)
{
}

Shared<Mod> Mod::load(const Path& path)
{
    try
    {
        if (const auto dll = LoadLibrary(path.get_absolute().to_string().wc()))
        {
            if (f_get_mod get_mod = (f_get_mod)GetProcAddress(dll, "get_mod"))
            {
                Shared<Mod> mod = Shared<Mod>(get_mod());
                mod->dll_ = dll;
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

Mod::Info Mod::load_mod_info(const Path& path)
{
    Info result;
    result.name = path.filename;
    if (auto reader = File::Reader::open(path))
    {
        result.display_name = reader->read_line();
        result.mod_version = reader->read_line();
        result.target_game_version = reader->read_line();
    }
    if (result.display_name.is_empty()) result.display_name = result.name;

    return result;
}
