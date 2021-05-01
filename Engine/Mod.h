#pragma once

#include <string>

#include "ModInfo.h"
#include "Path.h"

class EventBus;
EXTERN class EXPORT Mod
{
public:
    void init(EventBus* event_bus);

    const ModInfo& get_mod_info() const;

    static bool verify_signature(const Path& path);

    static Shared<Mod> load(const Path& path);

protected:
    virtual void on_init(EventBus* event_bus);

private:
    static ModInfo load_mod_info();
    
    ModInfo info_;
    HINSTANCE dll;
};
