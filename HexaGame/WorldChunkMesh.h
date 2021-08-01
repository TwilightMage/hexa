#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/List.h"
#include "Engine/Pointers.h"

class Texture;
class StaticMesh;

class EXPORT WorldChunkMesh
{
public:
    struct Domain
    {
        Shared<StaticMesh> mesh;
        Shared<Texture> texture;
    };

    List<Domain> domains;
};
