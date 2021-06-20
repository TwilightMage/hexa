#pragma once

#include "Engine/Mod.h"
#include "Engine/World.h"

class EXPORT ExampleMod : public Mod
{
public:
    void on_loaded(EventBus* event_bus) override;

private:
    void on_world_opened(const Weak<World>& world);
};
