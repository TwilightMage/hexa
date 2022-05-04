#pragma once

#include "CharacterInfo.h"
#include "TileInfo.h"
#include "Engine/AudioChannel.h"
#include "Engine/Table.h"
#include "Engine/Game.h"
#include "HexaGame/Database/items/ItemInfo.h"

class CharacterLogic;
class InventoryItem;
class ItemLogic;
class DefaultWorldGenerator;
class WorldGeneratorInfo;

class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

    static void register_world_generator(const Shared<WorldGeneratorInfo>& generator_info);

    static Shared<HexaGame> get_instance();

    inline static Map<Name, Shared<CharacterLogic>> character_logic_;

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
    

    void register_item(const ModuleAssetID& key, const Compound::Object& new_item, bool force_replace = false);
    Compound::Object* modify_item_data(const ModuleAssetID& key);
    ConstPtr<Compound::Object> get_item_data(const ModuleAssetID& key) const;
    const Map<ModuleAssetID, Compound::Object*>& get_all_items() const;
    void register_item_logic(const ModuleAssetID& key, const Shared<ItemLogic>& logic);
    Shared<ItemLogic> get_item_logic(const ModuleAssetID& key) const;

    void register_tile(const ModuleAssetID& key, const TileInfo& new_item, bool force_replace = false);
    TileInfo* modify_tile_data(const ModuleAssetID& key);
    ConstPtr<TileInfo> get_tile_data(const ModuleAssetID& key) const;
    void register_tile_logic(const ModuleAssetID& key, const Shared<TileLogic>& logic);
    Shared<TileLogic> get_tile_logic(const ModuleAssetID& key) const;

    Shared<InventoryItem> create_item(const ModuleAssetID& key, uint count = 1);
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    Shared<Settings> generate_settings_object() override;
    Shared<SaveGame> generate_save_game_object(const String& profile_name) override;
    void on_start() override;
    void on_tick(float delta_time) override;
    void on_init() override;
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

    inline static Shared<Table<Compound::Object>> item_database_ = nullptr;
    inline static Shared<Table<TileInfo>> tile_table_ = nullptr;
    inline static Shared<Table<Compound::Object>> character_table_ = nullptr;
    
    inline static Map<ModuleAssetID, Shared<ItemLogic>> item_logic_;
    inline static Map<ModuleAssetID, Shared<TileLogic>> tile_logic_;
};
