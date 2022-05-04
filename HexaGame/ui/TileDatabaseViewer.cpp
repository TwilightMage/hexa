#include "TileDatabaseViewer.h"


#include "TileDatabaseRow.h"
#include "Engine/Table.h"
#include "HexaGame/TileInfo.h"

TileDatabaseViewer::TileDatabaseViewer(const Shared<Table<TileInfo>>& database)
    : DatabaseViewer(database)
{
}

List<Shared<UIElement>> TileDatabaseViewer::construct_all_rows()
{
    if (auto database = cast<Table<TileInfo>>(get_database()))
    {
        List<Shared<UIElement>> result;

        for (auto& record : database->records())
        {
            //result.add(MakeShared<TileDatabaseRow>(record.value->key, record.value->texture));
        }

        return result;
    }

    return List<Shared<UIElement>>();
}
