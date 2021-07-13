﻿#pragma once

#include "Engine/Entity.h"

class EXPORT Arrow : public Entity
{
public:
    Arrow();
    
    void on_start() override;

    void on_tick(float delta_time) override;

protected:
    bool is_rigid_body() const override { return false; }
};
