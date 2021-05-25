#include "HexaWorld.h"

HexaWorld::HexaWorld(const Shared<WorldGenerator>& generator)
    : World()
    , generator_(generator)
{
}

const Shared<WorldGenerator>& HexaWorld::get_generator() const
{
    return generator_;
}
