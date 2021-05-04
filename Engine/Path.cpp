#include "Path.h"

#include <filesystem>


#include "Game.h"
#include "Regex.h"

Path::Path()
    : parent("")
    , filename(".")
    , extension("")
    , type(EPathType::Directory)
    , is_global_(false)
{
}

Path::Path(const String& path_str)
{
	std::filesystem::path path(path_str.replace('\\', '/').c());

	parent = path.parent_path().string();
	filename = path.stem().string();
	extension = path.extension().string();

	if (filename.is_empty() && extension.is_empty())
	{
		filename = ".";
	}

	is_global_ = Regex("^([A-Z]:/|/).*").check(parent);

	path = std::filesystem::path(get_absolute_string().c());

	if (is_block_file(path)) type = EPathType::Block;
	else if (is_character_file(path)) type = EPathType::Character;
	else if (is_directory(path)) type = EPathType::Directory;
	else if (is_fifo(path)) type = EPathType::Fifo;
	else if (is_other(path)) type = EPathType::Other;
	else if (is_regular_file(path)) type = EPathType::Regular;
	else if (is_socket(path)) type = EPathType::Socket;
	else if (is_symlink(path)) type = EPathType::Symlink;
}

Path::Path(const char* str)
	: Path(String(str))
{
}

bool Path::exists() const
{
	return std::filesystem::exists(get_absolute_string().c());
}

bool Path::is_global() const
{
	return is_global_;
}

void Path::create() const
{
	std::filesystem::create_directories(get_absolute_string().c());
}

Path Path::up(uint levels) const
{
	const auto separators = to_string().find('/');

	if (levels >= separators.length())
	{
		if (is_global_)
		{
			return Path(parent.substring(0, separators[0] + 1));
		}
		else
		{
			return Path(String("../") * (levels - static_cast<uint>(separators.length())));
		}
	}
	else
	{
		return Path(parent.substring(0, separators[separators.length() - levels] + 1));
	}
}

Path Path::get_child(const String& child) const
{
	return Path(to_string() + '/' + child);
}

Path Path::get_absolute() const
{
	return Path(get_absolute_string());
}

List<Path> Path::list() const
{
	std::vector<Path> result;

	if (exists())
	{
		for (const auto& entry : std::filesystem::directory_iterator(get_absolute_string().c()))
		{
			result.push_back(Path(entry.path().string()));
		}
	}

	return result;
}

String Path::get_extension() const
{
	return filename + extension;
}

Path Path::operator+(const Path& rhs) const
{
	return Path(to_string() + '/' + rhs.to_string());
}

String Path::to_string() const
{
	if (parent.is_empty())
	{
		return filename + extension;
	}
	else
	{
		return parent + '/' + filename + extension;
	}
}

String Path::get_absolute_string() const
{
	return !is_global_ && Game::is_app_path_set() ? (Game::get_app_path().parent + "/" + to_string()).std() : to_string().std();
}
