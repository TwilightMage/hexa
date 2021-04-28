#pragma once

#include "String.h"

enum class EPathType
{
    Block,
	Character,
	Directory,
	Fifo,
	Other,
	Regular,
	Socket,
	Symlink
};

IMPLEMENT_ENUM(EPathType)

EXTERN class EXPORT Path
{
public:
	Path();
	Path(const String& pathStr);
	Path(const char* str);

    bool Exists() const;
	bool IsGlobal() const;
	void Create();
	Path Up(uint levels = 1) const;
	Path GetChild(const String& child) const;
	Path get_absolute() const;
	List<Path> List() const;
	String GetFileExt() const;

	Path operator+(const Path& rhs) const;

	String ToString() const;

	String parent;
	String filename;
	String extension;
	EPathType type;

private:
	bool isGlobal;
	String get_absolute_string() const;
};