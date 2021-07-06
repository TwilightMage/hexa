﻿#include "TileDatabaseViewer.h"


#include "TileDatabaseRow.h"
#include "Engine/Database.h"
#include "HexaGame/TileInfo.h"

TileDatabaseViewer::TileDatabaseViewer(const Shared<Database<TileInfo>>& database)
    : DatabaseViewer(database)
{
}

List<Shared<UIElement>> TileDatabaseViewer::construct_all_rows()
{
    if (auto database = cast<Database<TileInfo>>(get_database()))
    {
        List<Shared<UIElement>> result;

        for (auto& record : database->records())
        {
            result.add(MakeShared<TileDatabaseRow>(record.value->key, record.value->texture));
        }

        return result;
    }

    return List<Shared<UIElement>>();
}
