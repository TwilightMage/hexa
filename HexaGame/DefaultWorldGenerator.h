#pragma once

#include "WorldGenerator.h"

class DefaultWorldGenerator : public WorldGenerator
{
public:
    Shared<WorldChunkData> generate_chunk(int x, int y) override;
};
