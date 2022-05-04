#pragma once

#include "Engine/Entity.h"

class EXPORT DemoMeshEntity : public Entity
{
public:
    DemoMeshEntity();

    void on_start() override;
};
