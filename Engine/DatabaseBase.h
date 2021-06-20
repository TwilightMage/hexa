#pragma once

#include "Object.h"

class EXPORT DatabaseBase : public Object
{
public:
    virtual ~DatabaseBase(){ }
    
    DatabaseBase(const String& name);
};
