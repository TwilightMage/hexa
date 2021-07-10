#pragma once

#include "Engine/BasicTypes.h"
#include "HexaGame/ComplexTileCustomData.h"

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
};
