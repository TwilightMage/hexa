#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/String.h"

class WorldGenerator;

class EXPORT WorldGeneratorInfo
{
public:
    virtual String get_name() const = 0;
    virtual Shared<WorldGenerator> create_generator() const = 0;
};
