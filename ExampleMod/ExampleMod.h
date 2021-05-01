#pragma once

#include "Engine/Mod.h"
#include "Engine/World.h"

EXTERN class EXPORT ExampleMod : public Mod
{
public:
    virtual void on_init(EventBus* event_bus) override;

private:
    void on_world_opened(const Weak<World>& world);
};
