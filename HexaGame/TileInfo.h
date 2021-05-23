#pragma once

#include "Engine/DatabaseEntry.h"

class Texture;

class TileInfo : public DatabaseEntry
{
public:
    Shared<Texture> texture;
};
