#include "HexaSettings.h"

#include "Engine/Math.h"

void HexaSettings::read_settings(const JSON& json)
{
    Settings::read_settings(json);

    const auto audio = json.get_object("audio");
    audio_music = Math::clamp(audio.get_float("music", 1.0f), 0.0f, 1.0f);
    audio_ambient = Math::clamp(audio.get_float("ambient", 1.0f), 0.0f, 1.0f);
    audio_effects = Math::clamp(audio.get_float("effects", 1.0f), 0.0f, 1.0f);
}

void HexaSettings::write_settings(JSON& json)
{
    Settings::write_settings(json);

    json
    .with_object("audio", json.get_object("audio")
        .with_float("music", audio_music)
        .with_float("ambient", audio_ambient)
        .with_float("effects", audio_effects));
}
