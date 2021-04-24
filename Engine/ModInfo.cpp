#include "ModInfo.h"

#include "File.h"

String ReadLine(Shared<File::Reader> reader)
{
    String line = reader->ReadLine();
    uint pos = line.IndexOf('#');
    if (pos >= 0)
    {
        line = line.Substring(0, pos);
    }

    line = line.Trim();

    return line;
}

void ModInfo::ReadFrom(const String& path)
{
    if (auto reader = File::Reader::Open(path))
    {
        name = ReadLine(reader);

        display_name = ReadLine(reader);

        List<String> modVersionParts = ReadLine(reader).Split('.');
        mod_version = Version(StringParse<uint>(modVersionParts[0]), StringParse<uint>(modVersionParts[1]), StringParse<uint>(modVersionParts[2]));

        List<String> targetGameVersionParts = ReadLine(reader).Split('.');
        target_game_version = Version(StringParse<uint>(modVersionParts[0]), StringParse<uint>(modVersionParts[1]), StringParse<uint>(modVersionParts[2]));
    }
}
