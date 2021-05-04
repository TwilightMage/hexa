#pragma once

#include <string>

#include "ModInfo.h"
#include "Path.h"

class Game;
class EventBus;
EXTERN class EXPORT Mod
{
    friend Game;
    
public:
    const ModInfo& get_mod_info() const;

    static bool verify_signature(const Path& path);

    Path mod_path(const String& subPath) const;
    
protected:
    virtual void loading_stage();
    virtual void on_loaded(EventBus* event_bus);

private:
    static Shared<Mod> load(const Path& path);
    static ModInfo load_mod_info(const Path& path);
    
    ModInfo info_;
    HINSTANCE dll;
};
