#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/List.h"

class Texture;
class Mesh;

EXTERN class EXPORT WorldChunkMesh
{
public:
    struct Domain
    {
        Shared<Mesh> mesh;
        Shared<Texture> texture;
    };

    List<Domain> domains;
};
