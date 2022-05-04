#pragma once

#include "Engine/Mod.h"
#include "Engine/World.h"

class EXPORT ExampleMod : public Mod
{
public:
    ExampleMod() : Mod("example_mod") {};
    
    void on_start(const Shared<EventBus>& event_bus) override;

private:
    void on_world_opened(const Shared<World>& world);
};
