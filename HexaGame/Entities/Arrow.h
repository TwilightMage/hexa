#pragma once

#include "Engine/Entity.h"

EXTERN class EXPORT Arrow : public Entity
{
public:
    Arrow();
    
    virtual void on_start() override;
    
};
