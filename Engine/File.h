#pragma once

#include "pch.h"
#include "String.h"
#include <fstream>

#include "Path.h"

class File
{
public:
	class Reader
	{
	public:
		static Shared<Reader> Open(const Path& path);

		String Read(uint lenght);
		String ReadAll();
		String ReadLine();
		bool IsEndOfFile() const;
		uint GetSize() const;
		uint GetPosition();

	private:
		Reader() {};

		std::ifstream stream;
		uint64 size;
	};

	class Writer
	{
	public:
		static Shared<Writer> Open(const Path& path);

		void Write(const String& data);

		void Close();

	private:
		Writer() {};

		std::ofstream stream;
	};

	virtual ~File() = delete;

	static String ReadFile(const Path& path);
	static void WriteFile(const Path& path, const String& data = "");
	static void AppendFile(const Path& path, const String& data = "");

	static bool Exists(const String& path);
};
