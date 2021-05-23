#pragma once

#include "Engine/Database.h"
#include "Engine/ui/DatabaseViewer.h"
#include "HexaGame/TileInfo.h"

EXTERN class EXPORT TileDatabaseViewer : public DatabaseViewer
{
public:
    explicit TileDatabaseViewer(const Shared<Database<TileInfo>>& database);

protected:
    List<Shared<UIElement>> construct_all_rows() override;
};
