#pragma once

#include "Engine/World.h"

class WorldGenerator;

class HexaWorld : public World
{
public:
    HexaWorld(const Shared<WorldGenerator>& generator);

    const Shared<WorldGenerator>& get_generator() const;

private:
    Shared<WorldGenerator> generator_;
};
