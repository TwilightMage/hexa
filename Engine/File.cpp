#include "File.h"

#include <glm/ext/quaternion_geometric.hpp>

String File::ReadFile(const Path& path)
{
	std::ifstream file(path.get_absolute().to_string().std(), std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void File::WriteFile(const Path& path, const String& data)
{
	std::ofstream file(path.get_absolute().to_string().std());

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	file << data;

	file.close();
}

void File::AppendFile(const Path& path, const String& data)
{
	std::ofstream file(path.get_absolute().to_string().std(), std::ios::app);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	file << data;

	file.close();
}

bool File::Exists(const String& path)
{
	struct stat buffer;
	return (stat(path.c(), &buffer) == 0);
}

Shared<File::Reader> File::Reader::Open(const Path& path)
{
	Shared<File::Reader> result = MakeSharedInternal(File::Reader);

	result->stream = std::ifstream(path.get_absolute().to_string().std(), std::ios::ate);

	if (!result->stream.is_open())
	{
		result = nullptr;
	}

	result->size = result->stream.tellg();
	result->stream.seekg(0);

	return result;
}

String File::Reader::Read(uint lenght)
{
	const auto buff_size = std::min(size - stream.tellg(), static_cast<uint64>(lenght));
	const auto buff = new char[buff_size];
	stream.read(buff, buff_size);
	return String(buff, static_cast<uint>(buff_size));
}

String File::Reader::ReadAll()
{
	const auto buff_size = size - stream.tellg();
	const auto buff = new char[buff_size];
	stream.read(buff, buff_size);
	return String(buff, static_cast<uint>(buff_size));
}

String File::Reader::ReadLine()
{
	std::string line;
	if (!stream.eof() && std::getline(stream, line))
	{
		return String(line);
	}

	return "";
}

bool File::Reader::IsEndOfFile() const
{
	return stream.eof();
}

uint File::Reader::GetSize() const
{
	return static_cast<uint>(size);
}

uint File::Reader::GetPosition()
{
	return static_cast<uint>(stream.tellg());
}

Shared<File::Writer> File::Writer::Open(const Path& path)
{
	auto result = MakeSharedInternal(File::Writer);

	result->stream = std::ofstream(path.get_absolute().to_string().std());

	if (!result->stream.is_open())
	{
		result = nullptr;
	}

	return result;
}

void File::Writer::Write(const String& data)
{
	stream << data;
}

void File::Writer::Close()
{
	stream.close();
}