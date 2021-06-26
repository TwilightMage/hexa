#pragma once

#include "ItemType.h"
#include "Engine/DatabaseEntry.h"

class Mesh;
class Texture;

class ItemInfo : public DatabaseEntry
{
public:

    Shared<Texture> icon;

    Shared<Mesh> mesh;
    Shared<Texture> mesh_texture;

    ItemType type;
    String name;
};
