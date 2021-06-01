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
    const auto default_world_generator_info = MakeShared<DefaultWorldGeneratorInfo>();
    register_world_generator(default_world_generator_info);

    if (const auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
    {
        if (auto world_settings = save_game->get_world_settings())
        {
            auto generator_settings = world_settings->get_object("generator");
            auto generator_name = generator_settings.get_string("name", "default");

            if (generator_infos_.contains(generator_name))
            {
                auto generator = generator_infos_[generator_name]->create_generator();
                generator->init(generator_settings.get_int("seed", 0));
                generator->read_settings(generator_settings.get_object("settings"));

                const auto world = MakeShared<GameWorld>(generator);
                open_world(world);
            }
        }
    }
}

void HexaGame::loading_stage()
{
    Tiles::init(tile_database);
}
