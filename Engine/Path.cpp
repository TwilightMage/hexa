#include "Path.h"

#include <filesystem>


#include "Game.h"
#include "Regex.h"

Path::Path()
    : isGlobal(false)
    , parent("")
    , filename(".")
    , extension("")
    , type(EPathType::Directory)
{
}

Path::Path(const String& pathStr)
{
	std::filesystem::path path(pathStr.Replace('\\', '/').c());

	parent = path.parent_path().string();
	filename = path.stem().string();
	extension = path.extension().string();

	if (filename.IsEmpty() && extension.IsEmpty())
	{
		filename = ".";
	}

	isGlobal = Regex("^([A-Z]:/|/).*").Check(parent);

	if (!isGlobal && Game::IsAppPathSet()) path = std::filesystem::path((Game::GetAppPath().parent + "/" + ToString()).std());

	if (is_block_file(path)) type = EPathType::Block;
	else if (is_character_file(path)) type = EPathType::Character;
	else if (is_directory(path)) type = EPathType::Directory;
	else if (is_fifo(path)) type = EPathType::Fifo;
	else if (is_other(path)) type = EPathType::Other;
	else if (is_regular_file(path)) type = EPathType::Regular;
	else if (is_socket(path)) type = EPathType::Socket;
	else if (is_symlink(path)) type = EPathType::Symlink;
}

bool Path::Exists() const
{
	return std::filesystem::exists(!isGlobal && Game::IsAppPathSet() ? (Game::GetAppPath().parent + "/" + ToString()).std() : ToString().std());
}

bool Path::IsGlobal() const
{
	return isGlobal;
}

void Path::Create()
{
	std::filesystem::create_directories(!isGlobal && Game::IsAppPathSet() ? (Game::GetAppPath().parent + "/" + ToString()).std() : ToString().std());
}

Path Path::Up(uint levels) const
{
	const auto separators = ToString().Find('/');

	if (levels >= separators.Length())
	{
		if (isGlobal)
		{
			return Path(parent.Substring(0, separators[0] + 1));
		}
		else
		{
			return Path(String("../") * (levels - static_cast<uint>(separators.Length())));
		}
	}
	else
	{
		return Path(parent.Substring(0, separators[separators.Length() - levels] + 1));
	}
}

Path Path::GetChild(const String& child) const
{
	return Path(ToString() + '/' + child);
}

List<Path> Path::List() const
{
	std::vector<Path> result;

	if (Exists())
	{
		for (const auto& entry : std::filesystem::directory_iterator(!isGlobal && Game::IsAppPathSet() ? (Game::GetAppPath().parent + "/" + ToString()).std() : ToString().std()))
		{
			result.push_back(Path(entry.path().string()));
		}
	}

	return result;
}

String Path::GetFileExt() const
{
	return filename + extension;
}

Path Path::operator+(const Path& rhs) const
{
	return Path(ToString() + '/' + rhs.ToString());
}

String Path::ToString() const
{
	if (parent.IsEmpty())
	{
		return filename + extension;
	}
	else
	{
		return parent + '/' + filename + extension;
	}
}
