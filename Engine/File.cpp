#include "File.h"

String File::ReadFile(const Path& path)
{
	std::ifstream file(path.get_absolute().ToString().std(), std::ios::ate | std::ios::binary);

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
	std::ofstream file(path.get_absolute().ToString().std());

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	file << data;

	file.close();
}

void File::AppendFile(const Path& path, const String& data)
{
	std::ofstream file(path.get_absolute().ToString().std(), std::ios::app);

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

	result->stream = std::ifstream(path.get_absolute().ToString().std(), std::ios::ate);

	if (!result->stream.is_open())
	{
		result = nullptr;
	}

	result->size = result->stream.tellg();
	result->stream.seekg(0);

	return result;
}

String File::Reader::ReadAll()
{
	size_t buffSize = size - stream.tellg();
	char* buff = new char[buffSize];
	stream.read(buff, buffSize);
	return String(buff, static_cast<uint>(buffSize));
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

int File::Reader::GetSize() const
{
	return static_cast<int>(size);
}

int File::Reader::GetPosition()
{
	return static_cast<int>(stream.tellg());
}

Shared<File::Writer> File::Writer::Open(const Path& path)
{
	Shared<File::Writer> result = MakeSharedInternal(File::Writer);

	result->stream = std::ofstream(path.get_absolute().ToString().std());

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