#pragma once

#include "CharacterInfo.h"
#include "ItemInfo.h"
#include "TileInfo.h"
#include "Engine/Database.h"
#include "Engine/Game.h"

class DefaultWorldGenerator;
class WorldGeneratorInfo;

class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

    inline const static Shared<Database<TileInfo>> tile_database = MakeShared<Database<TileInfo>>("Tile Database");
    inline const static Shared<Database<ItemInfo>> item_database = MakeShared<Database<ItemInfo>>("Item Database");
    inline const static Shared<Database<CharacterInfo>> character_database = MakeShared<Database<CharacterInfo>>("Character Database");

    static void register_world_generator(const Shared<WorldGeneratorInfo>& generator_info);

    static Shared<Shader> tile_cap_shader;

    static Shared<Material3D> tile_cap_material;
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    Shared<Settings> generate_settings_object() override;
    Shared<SaveGame> generate_save_game_object(const String& profile_name) override;
    void start() override;
    void loading_stage() override;

private:
    bool open_animation_editor(const String& entity_type, const String& entity_name);
    void open_game_world();
    void open_tile_test_world();
    
    Map<String, Shared<WorldGeneratorInfo>> generator_infos_;
};
