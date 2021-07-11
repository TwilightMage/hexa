#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/List.h"
#include "Engine/Pointers.h"

class Texture;
class Mesh;

class EXPORT WorldChunkMesh
{
public:
    struct Domain
    {
        Shared<Mesh> mesh;
        Shared<Texture> texture;
    };

    List<Domain> domains;
};
