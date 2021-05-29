#include "HexaGame.h"



#include "DefaultWorldGenerator.h"
#include "DefaultWorldGeneratorInfo.h"
#include "HexaSaveGame.h"
#include "HexaSettings.h"
#include "Tiles.h"
#include "WorldGenerator.h"
#include "Engine/GeometryEditor.h"
#include "ui/TileDatabaseViewer.h"
#include "Worlds/GameWorld.h"

Shared<Database<TileInfo>> HexaGame::tile_database = MakeShared<Database<TileInfo>>("Tile Database");

HexaGame::HexaGame(int argc, char* argv[])
    : Game(argc, argv)
{
}

void HexaGame::register_world_generator(const Shared<WorldGeneratorInfo>& generator_info)
{
    cast<HexaGame>(get_instance())->generator_infos_[generator_info->get_name()] = generator_info;
}

void HexaGame::init_game_info(GameInfo& outInfo)
{
    outInfo.title = "Hexa";
}

Shared<Settings> HexaGame::generate_settings_object()
{
    return MakeShared<HexaSettings>();
}

Shared<SaveGame> HexaGame::generate_save_game_object(const String& profile_name)
{
    return MakeShared<HexaSaveGame>(profile_name);
}

void HexaGame::start()
{
    auto defaut_world_generator_info = MakeShared<DefaultWorldGeneratorInfo>();
    register_world_generator(defaut_world_generator_info);
    
    default_world_generator_ = cast<DefaultWorldGenerator>(defaut_world_generator_info->create_generator());
    default_world_generator_->init(0);

    const auto world = MakeShared<GameWorld>(default_world_generator_);
    open_world(world);
}

void HexaGame::loading_stage()
{
    Tiles::init(tile_database);
}
