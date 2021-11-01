﻿#pragma once

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

    inline static Shared<Material3D> tile_cap_material = nullptr;
    inline static Shared<Material3D> skybox_material = nullptr;
    
    inline static Shared<Material> tall_grass_material = nullptr;
    inline static Shared<Material> blue_roses_material = nullptr;
    inline static Shared<Material> red_roses_material = nullptr;
    inline static Shared<Material> chamomile_material = nullptr;

    inline static Shared<Audio> plains_music = nullptr;

    inline static Shared<Audio> wood_chop_sound = nullptr;

    inline static Shared<Audio> wind_sound = nullptr;
    inline static Shared<Audio> drop_sound = nullptr;
    
    FORCEINLINE static const Shared<AudioChannel>& get_music_channel() { return music_channel_; }
    FORCEINLINE static const Shared<AudioChannel>& get_ambient_channel() { return ambient_channel_; }
    FORCEINLINE static const Shared<AudioChannel>& get_effects_channel() { return effects_channel_; }
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    Shared<Settings> generate_settings_object() override;
    Shared<SaveGame> generate_save_game_object(const String& profile_name) override;
    void on_start() override;
    void on_tick(float delta_time) override;
    void on_loading_stage() override;
    void on_unloading_stage() override;

private:
    bool open_animation_editor(const String& entity_type, const String& entity_name) const;
    void open_game_world();
    void open_tile_test_world();
    
    Map<String, Shared<WorldGeneratorInfo>> generator_infos_;

    inline static Shared<AudioChannel> music_channel_ = nullptr;
    inline static Shared<AudioChannel> ambient_channel_ = nullptr;
    inline static Shared<AudioChannel> effects_channel_ = nullptr;
};
