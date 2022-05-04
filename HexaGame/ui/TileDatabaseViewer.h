#pragma once

#include "Engine/Table.h"
#include "Engine/ui/DatabaseViewer.h"
#include "HexaGame/TileInfo.h"

class EXPORT TileDatabaseViewer : public DatabaseViewer
{
public:
    explicit TileDatabaseViewer(const Shared<Table<TileInfo>>& database);

protected:
    List<Shared<UIElement>> construct_all_rows() override;
};
