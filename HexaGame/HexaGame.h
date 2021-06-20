#pragma once

#include "TileInfo.h"
#include "Engine/Database.h"
#include "Engine/Game.h"

class DefaultWorldGenerator;
class WorldGeneratorInfo;

class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

    static Shared<Database<TileInfo>> tile_database;

    static void register_world_generator(const Shared<WorldGeneratorInfo>& generator_info);
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    Shared<Settings> generate_settings_object() override;
    Shared<SaveGame> generate_save_game_object(const String& profile_name) override;
    void start() override;
    void loading_stage() override;

private:
    Map<String, Shared<WorldGeneratorInfo>> generator_infos_;
};
