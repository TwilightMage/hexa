#pragma once

#include "Engine/Mod.h"
#include "Engine/World.h"

class EXPORT ExampleMod : public Mod
{
public:
    ExampleMod(const Path& root) : Mod(root, "ExampleMod") {};
    
    void on_loaded(const Shared<EventBus>& event_bus) override;

private:
    void on_world_opened(const Shared<World>& world);
};
