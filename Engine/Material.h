#pragma once

#include "Pointers.h"

class TileInfo;
class MeshComponent;
class Module;

namespace Ogre {
    class Material;
}

class Material
{
    friend Module;
    friend MeshComponent;
    friend TileInfo;
public:

private:
    Shared<Ogre::Material> ogre_material_;
};
