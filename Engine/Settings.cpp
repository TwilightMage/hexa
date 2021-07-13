#include "Settings.h"

#include "Math.h"

void Settings::read_settings(const JSON& json)
{
    const auto graphics = json.get_object("graphics");
    fps_limit = Math::clamp(graphics.get_int("fps cap", 60), 1, 10000);

    const auto audio = json.get_object("audio");
    audio_general = Math::clamp(audio.get_float("general", 1.0f), 0.0f, 1.0f);
}

void Settings::write_settings(JSON& json)
{   
    json
    .with_object("graphics", JSON()
        .with_int("fps cap", fps_limit))
    .with_object("audio", JSON()
        .with_float("general", audio_general));
}
