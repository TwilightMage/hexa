#include "HexaGame.h"


#include "Characters.h"
#include "DefaultWorldGeneratorInfo.h"
#include "HexaSaveGame.h"
#include "HexaSettings.h"
#include "Items.h"
#include "Tiles.h"
#include "WorldGenerator.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Logger.h"
#include "Engine/Material.h"
#include "Engine/Material3D.h"
#include "Engine/AnimationEditor/AnimationEditorWorld.h"
#include "Entities/Characters/Slime.h"
#include "ui/TileDatabaseViewer.h"
#include "Worlds/GameWorld.h"
#include "Worlds/TilePresentationWorld.h"

Shared<Shader> HexaGame::tile_cap_shader = nullptr;

Shared<Material3D> HexaGame::tile_cap_material = nullptr;

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
    if (get_args().length() >= 2 && get_args()[1] == "animations")
    {
        if (get_args().length() >= 4)
        {
            if (!open_animation_editor(get_args()[2], get_args()[3]))
            {
                exit(1);
            }
        }
        else
        {
            print_error("Hexa", "Failed to start animation editor. Invalid parameter count");
            exit(1);
        }
    }
    else
    {
        open_game_world();
        //open_tile_test_world();
    }
}

void HexaGame::loading_stage()
{
    tile_cap_shader = Shader::compile("tile cap", {
        RESOURCES_ENGINE_SHADERS + "basic_3d.vert",
        RESOURCES_HEXA_SHADERS + "tile_cap.frag"
    });

    tile_cap_material = MakeShared<Material3D>();
    tile_cap_material->init(tile_cap_shader, 0);
    
    Tiles::init(tile_database);

    Items::init(item_database);

    Characters::init(character_database);
}

bool HexaGame::open_animation_editor(const String& entity_type, const String& entity_name)
{
    auto editor = MakeShared<AnimationEditorWorld>();
    open_world(editor);
            
    if (entity_type == "character")
    {
        if (const auto character_info = character_database->get(entity_name))
        {
            editor->open(character_info->character_supplier());
            return true;
        }
        else
        {
            print_error("Hexa", "Unable to find character %s", entity_name.c());
        }
    }
    else
    {
        print_error("Hexa", "Unknown entity type %s", entity_type.c());
    }

    return false;
}

void HexaGame::open_game_world()
{
    const auto default_world_generator_info = MakeShared<DefaultWorldGeneratorInfo>();
    register_world_generator(default_world_generator_info);

    if (const auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
    {
        if (const auto world_settings = save_game->get_world_settings())
        {
            const auto generator_settings = world_settings->get_object("generator");
            const auto generator_name = generator_settings.get_string("name", "default");

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

void HexaGame::open_tile_test_world()
{
    const auto world = MakeShared<TilePresentationWorld>();
    open_world(world);
}
