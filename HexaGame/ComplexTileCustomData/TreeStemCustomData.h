#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/List.h"
#include "Engine/Pointers.h"
#include "HexaGame/ComplexTileCustomData.h"

class Entity;

class TreeStemCustomData : public ComplexTileCustomData
{
public:
    enum class Type
    {
        Basic,
        Roots,
        Top,
        Branched
    };

    Type type = Type::Basic;

    uint tree_seed;
    uint cell_index;

    List<Shared<Entity>> tree_sub_parts;
};
