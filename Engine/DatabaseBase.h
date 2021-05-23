#pragma once

#include "Object.h"

EXTERN class EXPORT DatabaseBase : public Object
{
public:
    virtual ~DatabaseBase(){ }
    
    DatabaseBase(const String& name);
};
