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
	String(const char* c_str);
	String(const char* c_str, uint size);

	String& operator=(const String& rhs);

	~String();

	std::string std() const;
	const char* c() const;
	// return the cope of inner string
	// may cause memory leak if you are not careful
	char* c_copy() const;
	wchar_t* wc() const;
	std::vector<char> vec() const;
	List<char> list() const;

	char* begin();
	char* end();
	const char* begin() const;
	const char* end() const;

	uint length() const;
	uint allocated_length() const;
	int index_of(const String& substr) const;
	int last_index_of(const String& substr) const;
	String substring(uint start, uint num) const;
	String substring(uint start) const;
	String replace(const String& from, const String& to) const;
	String remove(const String& substring) const;
	String fit(int new_length, char filler = ' ') const;
	String to_lower() const;
	String to_upper() const;
	String trim_start(const String& chars_to_trim = " \r\n\t") const;
	String trim_end(const String& chars_to_trim = " \r\n\t") const;
	String trim(const String& chars_to_trim = " \r\n\t") const;
	bool starts_with(const String& val) const;
	bool ends_with(const String& val) const;
	bool contains(const String& substr) const;
	bool is_empty() const;
	List<String> split(const String& delimiter, bool remove_empty = false) const;
	List<uint> find(const String& substr) const;

	static bool replace_single(String& src, const String& from, const String& to);

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
	char* inner_;
	uint length_;
	uint allocated_length_;
};

inline String operator+(const char* const l, const String& r)
{
	return String(l) + r.std();
}

inline std::string operator+(const std::string& l, const String& r)
{
	return l + r.std();
}

template<class Elem, class Traits>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& o_stream, const String& str)
{
	o_stream << str.c();
	return (o_stream);
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
	if (items.length() == 0) return "";

	String result;

	for (uint i = 0; i < items.length(); i++)
	{
		if (i > 0) result += glue;
		result += items[i];
	}

	return result;
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
	return String(hexString).to_lower();
}