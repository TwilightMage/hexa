#pragma once

#include "Engine/Entity.h"

class SingleTile : public Entity
{
public:
    void on_start() override;

protected:
    bool is_rigid_body() override;
};
