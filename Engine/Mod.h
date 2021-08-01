#pragma once

#include "Module.h"
#include "Path.h"
#include "Pointers.h"
#include "Version.h"

class Game;
class EventBus;

class EXPORT Mod : public Module
{
    friend Game;
    
public:
    Mod(const Path& root, const String& module_name);
    
    struct Info
    {
        void ReadFrom(const String& path);

        String name;
        String display_name;
        Version mod_version;
        Version target_game_version;
    };
    
    const Info& get_mod_info() const;

    static bool verify_signature(const Path& path);

    Path mod_path(const String& sub_path) const;
    
protected:
    virtual void loading_stage();
    virtual void on_loaded(const Shared<EventBus>& event_bus);

private:
    static Shared<Mod> load(const Path& path);
    static Info load_mod_info(const Path& path);
    
    Info info_;
    HINSTANCE dll_;
};

#define IMPLEMENT_MOD_ENTRY(ModTypeName) extern "C" Mod EXPORT *get_mod(const Path& root) { return new ModTypeName(root); }