#pragma once

#include "Engine/Entity.h"

EXTERN class EXPORT DemoMeshEntity : public Entity
{
public:
    DemoMeshEntity();
    
    virtual void on_start() override;
};
