#include "HexaSettings.h"

#include "Engine/Math.h"

void HexaSettings::read_settings(const Compound::Object& json)
{
    Settings::read_settings(json);

    const auto audio = json.get_object("audio");
    audio_music = Math::clamp(audio.get_float("music", 1.0f), 0.0f, 1.0f);
    audio_ambient = Math::clamp(audio.get_float("ambient", 1.0f), 0.0f, 1.0f);
    audio_effects = Math::clamp(audio.get_float("effects", 1.0f), 0.0f, 1.0f);

    const auto game = json.get_object("game");
    chunk_load_distance = game.get_int32("chunk load distance", 9);
}

Compound::Object HexaSettings::write_settings()
{
    auto obj = Settings::write_settings();
    obj["audio"]["music"] = audio_music;
    obj["audio"]["ambient"] = audio_ambient;
    obj["audio"]["effects"] = audio_effects;
    obj["game"]["chunk load distance"] = (int32)chunk_load_distance;

    return obj;
}
