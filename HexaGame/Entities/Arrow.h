#pragma once

#include "Engine/Entity.h"
#include "Engine/ITickable.h"

EXTERN class EXPORT Arrow : public Entity, public ITickable
{
public:
    Arrow();
    
    virtual void on_start() override;

    void tick(float delta_time) override;
};
