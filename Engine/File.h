#pragma once

#include "pch.h"
#include "String.h"
#include <fstream>

class File
{
public:
	class Reader
	{
	public:
		static Shared<Reader> Open(const String& path);

		String ReadAll();
		String ReadLine();
		bool IsEndOfFile() const;
		int GetSize() const;
		int GetPosition();

	private:
		Reader() {};

		std::ifstream stream;
		size_t size;
	};

	class Writer
	{
	public:
		static Shared<Writer> Open(const String& path);

		void Write(const String& data);

		void Close();

	private:
		Writer() {};

		std::ofstream stream;
	};

	virtual ~File() = delete;

	static String ReadFile(const String& path);
	static void WriteFile(const String& path, const String& data = "");
	static void AppendFile(const String& path, const String& data = "");

	static bool Exists(const String& path);
};