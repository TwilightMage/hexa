#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "Pointers.h"

class World;
class Entity;

class EXPORT EntityComponent : public EnableSharedFromThis<EntityComponent>
{
    friend Entity;
    friend World;

public:
    FORCEINLINE Shared<Entity> get_owner() const { return owner.lock(); }

protected:
    virtual void on_start();
    virtual void on_tick(float delta_time);
    virtual void on_destroy();

private:
    Weak<Entity> owner;
};
