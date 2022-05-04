#include "HexaGame.h"


#include "Characters.h"
#include "DefaultWorldGeneratorInfo.h"
#include "HexaSaveGame.h"
#include "HexaSettings.h"
#include "InventoryItem.h"
#include "ItemLogic.h"
#include "TileLogic.h"
#include "WorldGenerator.h"
#include "Database/Items.h"
#include "Database/Tiles.h"
#include "Engine/Audio.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Logger.h"
#include "Entities/Characters/Slime.h"
#include "ui/TileDatabaseViewer.h"
#include "Worlds/GameWorld.h"
#include "Worlds/TilePresentationWorld.h"

HexaGame::HexaGame(int argc, char* argv[])
    : Game("hexa", argc, argv)
{
}

void HexaGame::register_world_generator(const Shared<WorldGeneratorInfo>& generator_info)
{
    cast<HexaGame>(get_instance())->generator_infos_[generator_info->get_name()] = generator_info;
}

Shared<HexaGame> HexaGame::get_instance()
{
    return cast<HexaGame>(Game::get_instance());
}

void HexaGame::register_item(const ModuleAssetID& key, const Compound::Object& new_item, bool force_replace)
{
    if (get_stage() == GameStage::Loading)
    {
        item_database_->add_record_compound(key, new_item, force_replace);
    }
    else
    {
        print_error("HexaGame", "Item registration allowed only during loading stage");
    }
}

Compound::Object* HexaGame::modify_item_data(const ModuleAssetID& key)
{
    if (get_stage() == GameStage::Loading)
    {
        return item_database_->get_record(key);
    }

    return nullptr;
}

ConstPtr<Compound::Object> HexaGame::get_item_data(const ModuleAssetID& key) const
{
    return item_database_->get_record(key);
}

const Map<ModuleAssetID, Compound::Object*>& HexaGame::get_all_items() const
{
    return item_database_->records();
}

void HexaGame::register_item_logic(const ModuleAssetID& key, const Shared<ItemLogic>& logic)
{
    if (get_stage() == GameStage::Loading)
    {
        item_logic_[key] = logic;
    }
}

Shared<ItemLogic> HexaGame::get_item_logic(const ModuleAssetID& key) const
{
    return item_logic_.find_or_default(key);
}

void HexaGame::register_tile(const ModuleAssetID& key, const TileInfo& new_item, bool force_replace)
{
    if (get_stage() == GameStage::Loading)
    {
        tile_table_->add_record(key, new_item, force_replace);
    }
    else
    {
        print_error("HexaGame", "Item registration allowed only during loading stage");
    }
}

TileInfo* HexaGame::modify_tile_data(const ModuleAssetID& key)
{
    if (get_stage() == GameStage::Loading)
    {
        return tile_table_->get_record(key);
    }

    return nullptr;
}

ConstPtr<TileInfo> HexaGame::get_tile_data(const ModuleAssetID& key) const
{
    return tile_table_->get_record(key);
}

void HexaGame::register_tile_logic(const ModuleAssetID& key, const Shared<TileLogic>& logic)
{
    if (get_stage() == GameStage::Loading)
    {
        tile_logic_[key] = logic;
    }
}

Shared<TileLogic> HexaGame::get_tile_logic(const ModuleAssetID& key) const
{
    return tile_logic_.find_or_default(key);
}

Shared<InventoryItem> HexaGame::create_item(const ModuleAssetID& key, uint count)
{
    if (auto item_data = get_item_data(key))
    {
        // TODO: Update item creation
        /*if (auto logic = item_logic_.find_or_default(Name(item_data->get_string("logic", "item"))))
        {
            auto data = *item_data;
            data.remove("logic");

            return MakeShared<InventoryItem>(key, data, logic, count);
        }*/
    }

    return nullptr;
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

void HexaGame::on_start()
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
        if (Utils::ask_yes_no("Start debug scene?"))
        {
            open_tile_test_world();
        }
        else
        {
            open_game_world();   
        }
    }
}

void HexaGame::on_tick(float delta_time)
{
    if (auto world = get_world())
    {
        //foliage_material->set_param_value("time", world->get_time());
        //foliage_material->set_param_value("ambient_light", world->ambient_color.to_vector3() * world->ambient_intensity);
        //foliage_material->set_param_value("sun_light", world->sun_color.to_vector3() * world->sun_intensity);
        //foliage_material->set_param_value("sun_dir", -world->sun_angle.forward());
    }
}

void HexaGame::on_init()
{
    item_database_ = create_table<Compound::Object>("items");
    tile_table_ = create_table<TileInfo>("tiles");
    character_table_ = create_table<Compound::Object>("characters");
}

void HexaGame::on_loading_stage()
{
    //set_cursor_texture(Texture::load_png(RESOURCES_TEXTURES_UI + "cursor.png"), 0, 0);
    
    /*tile_cap_shader = Shader::compile("tile cap", {
        RESOURCES_ENGINE_SHADERS + "basic_3d.vert",
        RESOURCES_HEXA_SHADERS + "tile_cap.frag"
    });

    skybox_shader = Shader::compile("skybox", {
        RESOURCES_ENGINE_SHADERS + "basic_3d.vert",
        RESOURCES_HEXA_SHADERS + "skybox.frag"
    });

    foliage_shader = Shader::compile("foliage", {
        RESOURCES_HEXA_SHADERS + "foliage.vert",
        RESOURCES_HEXA_SHADERS + "foliage.frag"
    });

    tile_cap_material = MakeShared<Material3D>();
    tile_cap_material->init(tile_cap_shader, 0);
    //tile_cap_material->set_param_value("void_skybox", Cubemap::load_png(RESOURCES_HEXA_TEXTURES_TILES + "cap.png"));

    skybox_material = MakeShared<Material3D>();
    skybox_material->init(skybox_shader, -1);
    skybox_material->set_param_value("sky_top", Color(23, 23, 255).to_quaternion());
    skybox_material->set_param_value("sky_horizon", Color::white().to_quaternion());
    skybox_material->set_param_value("sky_down", Color::black().to_quaternion());

    foliage_material = MakeShared<Material3D>();
    foliage_material->init(foliage_shader, 0);
    foliage_material->cast_shadows = true;*/

    WorldGenerator::init_tile_variations();

    plains_music = Audio::load(RESOURCES_AUDIO_MUSIC + "plains.ogg");
    plains_music->set_looped(true);

    wood_chop_sound = Audio::load(RESOURCES_AUDIO_EFFECTS + "wood_chop.ogg");
    wood_chop_sound->set_default_volume(2.0f);

    wind_sound = Audio::load(RESOURCES_AUDIO_AMBIENT + "wind.ogg");
    wind_sound->set_looped(true);
    wind_sound->set_default_volume(0.3f);
    
    drop_sound = Audio::load(RESOURCES_AUDIO_AMBIENT + "drop.ogg");
    drop_sound->set_default_volume(0.5f);

    music_channel_ = AudioChannel::create(get_general_channel());
    music_channel_->set_volume(get_settings<HexaSettings>()->audio_music);
    ambient_channel_ = AudioChannel::create(get_general_channel());
    ambient_channel_->set_volume(get_settings<HexaSettings>()->audio_ambient);
    effects_channel_ = AudioChannel::create(get_general_channel());
    effects_channel_->set_volume(get_settings<HexaSettings>()->audio_effects);

    tile_logic_[get_asset_id("tile")] = MakeShared<TileLogic>();
    
    item_logic_[get_asset_id("item")] = MakeShared<ItemLogic>();
}

void HexaGame::on_unloading_stage()
{
    music_channel_.reset();
    ambient_channel_.reset();
}

bool HexaGame::open_animation_editor(const String& entity_type, const String& entity_name) const
{
    /*auto editor = MakeShared<AnimationEditorWorld>();
    open_world(editor);
            
    if (entity_type == "character")
    {
        if (const auto character_info = character_database->get(Name(entity_name)))
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
*/
    return false;
}

void HexaGame::open_game_world()
{
    const auto default_world_generator_info = MakeShared<DefaultWorldGeneratorInfo>();
    register_world_generator(default_world_generator_info);

    if (const auto save_game = cast<HexaSaveGame>(get_save_game()))
    {
        if (const auto world_settings = save_game->get_world_settings())
        {
            const auto generator_settings = world_settings->get_object("generator");
            const auto generator_name = generator_settings.get_string("name", "default");

            if (generator_infos_.contains(generator_name))
            {
                auto generator = generator_infos_[generator_name]->create_generator();
                generator->allocate_thread_pool();
                generator->init(generator_settings.get_int32("seed", 0));
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
