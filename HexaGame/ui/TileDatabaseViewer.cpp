#include "TileDatabaseViewer.h"


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
            result.Add(MakeShared<TileDatabaseRow>(record.second->key, record.second->texture));
        }

        return result;
    }

    return List<Shared<UIElement>>();
}
