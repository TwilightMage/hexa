#pragma once

#include "CharacterInfo.h"
#include "TileInfo.h"
#include "Engine/AudioChannel.h"
#include "Engine/Database.h"
#include "Engine/Game.h"
#include "HexaGame/Database/items/ItemInfo.h"

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

    inline static Shared<Shader> tile_cap_shader = nullptr;
    inline static Shared<Shader> skybox_shader = nullptr;
    inline static Shared<Shader> foliage_shader = nullptr;

    inline static Shared<Material3D> tile_cap_material = nullptr;
    inline static Shared<Material3D> skybox_material = nullptr;
    inline static Shared<Material3D> foliage_material = nullptr;

    inline static Shared<Audio> plains_music = nullptr;

    FORCEINLINE static const Shared<AudioChannel>& get_music_channel() { return music_channel_; }
    FORCEINLINE static const Shared<AudioChannel>& get_ambient_channel() { return ambient_channel_; }
    FORCEINLINE static const Shared<AudioChannel>& get_effects_channel() { return effects_channel_; }
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    Shared<Settings> generate_settings_object() override;
    Shared<SaveGame> generate_save_game_object(const String& profile_name) override;
    void start() override;
    void tick(float delta_time) override;
    void loading_stage() override;
    void unloading_stage() override;

private:
    bool open_animation_editor(const String& entity_type, const String& entity_name) const;
    void open_game_world();
    void open_tile_test_world();
    
    Map<String, Shared<WorldGeneratorInfo>> generator_infos_;

    inline static Shared<AudioChannel> general_channel_ = nullptr;
    inline static Shared<AudioChannel> music_channel_ = nullptr;
    inline static Shared<AudioChannel> ambient_channel_ = nullptr;
    inline static Shared<AudioChannel> effects_channel_ = nullptr;
};
