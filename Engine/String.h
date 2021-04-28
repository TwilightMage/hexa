#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "framework.h"
#include "List.h"

EXTERN class EXPORT String
{
public:
	String();
	String(const String& src);
	String(const std::string& str);
	String(char ch, uint count);
	String(const std::initializer_list<char>& il);
	String(const std::vector<char>& vec);
	String(const List<char>& list);
	String(char ch);
	String(const char* cstr);
	String(const char* cstr, uint size);

	String& operator=(const String& rhs);

	~String();

	std::string std() const;
	const char* c() const;
	/*
	 * return the cope of inner string
	 * may cause memory leak if you are not careful
	*/
	char* c_copy() const;
	wchar_t* wc() const;
	std::vector<char> vec() const;
	List<char> list() const;

	uint Length() const;
	uint AllocatedLength() const;
	int IndexOf(const String& substr) const;
	int LastIndexOf(const String& substr) const;
	String Substring(uint start, uint num) const;
	String Substring(uint start) const;
	String Replace(const String& from, const String& to) const;
	String Remove(const String& rmstr) const;
	String Fit(int newLength, char filler = ' ') const;
	String ToLower() const;
	String ToUpper() const;
	String TrimStart(const String& charsToTrim = " \r\n\t") const;
	String TrimEnd(const String& charsToTrim = " \r\n\t") const;
	String Trim(const String& charsToTrim = " \r\n\t") const;
	bool StartsWith(const String& val) const;
	bool EndsWith(const String& val) const;
	bool Contains(const String& substr) const;
	bool IsEmpty() const;
	List<String> Split(const String& delimiter, bool removeEmpty = false) const;
	List<uint> Find(const String& substr) const;

	static bool ReplaceSingle(String& src, const String& from, const String& to);

	String operator*(uint rhs) const;

	String operator+(const String& rhs) const;
	void operator+=(const String& rhs);
	bool operator==(const String& rhs) const;
	bool operator!=(const String& rhs) const;
	bool operator<(const String& rhs) const;
	bool operator<=(const String& rhs) const;
	bool operator>(const String& rhs) const;
	bool operator>=(const String& rhs) const;

	char& operator[](uint index);
	const char& operator[](uint index) const;

private:
	char* inner;
	uint length;
	uint allocatedLength;
};

inline List<uint> String::Find(const String& substr) const
{
	List<uint> result;

	for (uint i = 0; i < length - substr.length + 1; i++)
	{
		bool flag = true;

	    for (uint j = 0; j < substr.length; j++)
	    {
	        if (inner[i + j] != substr.inner[j])
	        {
				flag = false;
				break;
	        }
	    }

		if (flag)
		{
			result.Add(i);
			i += substr.length - 1;
		}
	}

	return result;
}

inline std::string operator+(const char* const l, const String& r)
{
	return std::string(l) + r.std();
}

inline std::string operator+(const std::string& l, const String& r)
{
	return l + r.std();
}

template<class Elem, class Traits>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& Ostr, const String& Str)
{
	Ostr << "\"" << Str.c() << "\"";
	return (Ostr);
}


// GLOBAL functions

template<typename T>
static String StringJoin(List<T> items, const String& glue)
{
	if (items.Length() == 0) return "";

	std::stringstream str;

	for (uint i = 0; i < items.Length(); i++)
	{
		if (i > 0) str << glue.std();
		str << items[i];
	}

	return str.str();
}

static String StringJoin(List<String> items, const String& glue)
{
	if (items.Length() == 0) return "";

	std::stringstream str;

	for (uint i = 0; i < items.Length(); i++)
	{
		if (i > 0) str << glue.std();
		str << items[i].c();
	}

	return str.str();
}

template<typename T>
static String StringMake(T val)
{
	std::stringstream str;
	str << val;
	return str.str();
}

template<typename... Args>
static String StringFormat(const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

	return String(buffer, size);
}

template<typename T>
T StringParse(const String& string)
{
	std::stringstream str;
	str << string.c();
	T result;
	str >> result;
	return result;
}

/*template<>
bool StringParse() const
{
	return ToLower() == "true";
}*/

template<class C>
static String StringFromPtr(C* ptr)
{
	char hexString[20];
	sprintf_s(hexString, 20, "0x%p", ptr);
	return String(hexString).ToLower();
}