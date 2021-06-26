#pragma once

#include "Engine/Entity.h"
#include "Engine/ITickable.h"

class EXPORT Arrow : public Entity, public ITickable
{
public:
    Arrow();
    
    void on_start() override;

    void tick(float delta_time) override;

protected:
    bool is_rigid_body() const override { return false; }
};
