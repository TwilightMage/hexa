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
	Path(const String& path_str);
	Path(const char* str);

    bool exists() const;
	bool is_global() const;
	void create();
	Path up(uint levels = 1) const;
	Path get_child(const String& child) const;
	Path get_absolute() const;
	List<Path> list() const;
	String get_extension() const;

	Path operator+(const Path& rhs) const;

	String to_string() const;
	String get_absolute_string() const;
	
	String parent;
	String filename;
	String extension;
	EPathType type;

private:
	bool is_global_;
};