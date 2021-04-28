#include "String.h"

#include "BasicTypes.h"
#include "framework.h"

String::String()
	: inner(new char('\0'))
    , length(0)
    , allocatedLength(1)
{
}

String::String(const String& src)
	: inner(new char[src.Length() + 1])
    , length(src.length)
    , allocatedLength(src.allocatedLength)
{
	memcpy(inner, src.inner, Length());
	inner[Length()] = '\0';
}

String::String(const std::string& str)
	: inner(new char[str.length() + 1])
    , length(static_cast<uint>(str.length()))
    , allocatedLength(static_cast<uint>(str.length() + 1))
{
	memcpy(inner, str.data(), Length());
	inner[Length()] = '\0';
}

String::String(char ch, uint count)
	: inner(new char[count + 1])
    , length(count)
    , allocatedLength(count + 1)
{
	for (uint i = 0; i < count; i++)
	{
		inner[i] = ch;
	}
	inner[length] = '\0';
}

String::String(const std::initializer_list<char>& il)
	: inner(new char[il.size() + 1])
	, length(static_cast<uint>(il.size()))
	, allocatedLength(static_cast<uint>(il.size() + 1))
{
	memcpy(inner, il.begin(), il.size());
	inner[length] = '\0';
}

String::String(const std::vector<char>& vec)
	: inner(new char[vec.size() + 1])
    , length(static_cast<uint>(vec.size()))
    , allocatedLength(static_cast<uint>(vec.size() + 1))
{
	memcpy(inner, vec.data(), vec.size());
	inner[length] = '\0';
}

String::String(const List<char>& list)
	: inner(new char[list.Length() + 1])
	, length(static_cast<uint>(list.Length()))
	, allocatedLength(static_cast<uint>(list.Length() + 1))
{
	memcpy(inner, list.GetData(), list.Length());
	inner[length] = '\0';
}

String::String(char ch)
	: inner(new char[2])
    , length(1)
    , allocatedLength(2)
{
	inner[0] = ch;
	inner[1] = '\0';
}

String::String(const char* cstr)
	: inner(new char[strlen(cstr) + 1])
    , length(static_cast<uint>(strlen(cstr)))
    , allocatedLength(static_cast<uint>(strlen(cstr) + 1))
{
	memcpy(inner, cstr, strlen(cstr));
	inner[length] = '\0';
}

String::String(const char* cstr, uint size)
	: inner(new char[size + 1])
	, length(size)
    , allocatedLength(size + 1)
{
	memcpy(inner, cstr, size);
	inner[length] = '\0';
}

String& String::operator=(const String& rhs)
{
	if (this == &rhs) return *this;

	delete[] inner;

	inner = new char[rhs.Length() + 1];
	memcpy(inner, rhs.inner, rhs.Length() + 1);
	length = rhs.length;
	allocatedLength = rhs.allocatedLength;

	return *this;
}

String::~String()
{
	delete[] inner;
}

std::string String::std() const
{
	return inner;
}

const char* String::c() const
{
	return inner;
}

char* String::c_copy() const
{
	char* result = new char[Length() + 1];
	memcpy(result, inner, Length() + 1);
	return result;
}

wchar_t* String::wc() const
{
	wchar_t* result = new wchar_t[length];

	mbstowcs_s(nullptr, result, length + 1, inner, length);

	return result;
}

std::vector<char> String::vec() const
{
	return std::vector<char>(inner, inner + length);
}

List<char> String::list() const
{
	return List<char>(inner, length);
}

uint String::Length() const
{
	return length;
}

uint String::AllocatedLength() const
{
	return allocatedLength;
}

int String::IndexOf(const String& substr) const
{
	if (substr.length > length) return -1;

	for (uint i = 0; i < length - substr.length + 1; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length; j++)
		{
		    if (inner[j + i] != substr.inner[j])
		    {
				flag = false;
				break;
		    }
		}

		if (flag)
		{
			return i;
		}
	}

	return -1;
}

int String::LastIndexOf(const String& substr) const
{
	if (substr.length > length) return -1;

	for (uint i = substr.length; i < length; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length; j++)
		{
			if (inner[length - i + j] != substr.inner[j])
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			return length - i;
		}
	}

	return -1;
}

String String::Substring(uint start, uint num) const
{
	const uint min = min(length, start + num);
	char* result = new char[min - start];

	for (uint i = start; i < min; i++)
	{
		result[i - start] = inner[i];
	}

	return String(result, min - start);
}

String String::Substring(uint start) const
{
	return Substring(start, length - start);
}

String String::Replace(const String& from, const String& to) const
{
	List<uint> positions = Find(from);
	const int offset = to.length - from.length;

	const uint newLength = static_cast<uint>(length + positions.Length() * offset);
	char* result = new char[newLength];

	uint counter = 0;
	for (uint i = 0; i < newLength; i++)
	{
		if (counter < positions.Length() && i - offset * counter == positions[counter])
		{
			memcpy(&result[i], to.inner, to.length);
			counter++;
			i += to.length - 1;
		}
		else
		{
			result[i] = inner[i - offset * counter];
		}
	}

	return String(result, newLength);
}

String String::Remove(const String& rmstr) const
{
	return Replace(rmstr, "");
}

String String::Fit(int newLength, char filler) const
{
	const bool keepLeft = newLength > 0;
	newLength = abs(newLength);

	if (newLength < static_cast<int>(length))
	{
		if (keepLeft) return Substring(0, newLength);
		return Substring(length - newLength);
	}

	if (static_cast<int>(length) < newLength)
	{
		char* result = new char[newLength];

		if (keepLeft)
		{
			memcpy(result, inner, length);
			memcpy(result + length, std::string(newLength - length, filler).c_str(), (newLength - length));
		}
		else
		{
			memcpy(result, std::string(newLength - length, filler).c_str(), (newLength - length));
			memcpy(result + (newLength - length), inner, length);
		}

		return String(result, newLength);
	}

	return *this;
}

String String::ToLower() const
{
	String result = *this;

	for (uint i = 0; i < length; i++)
	{
		result.inner[i] = std::tolower(result.inner[i]);
	}

	return result;
}

String String::ToUpper() const
{
	String result = *this;

	for (uint i = 0; i < length; i++)
	{
		result.inner[i] = std::toupper(result.inner[i]);
	}

	return result;
}

String String::TrimStart(const String& charsToTrim) const
{
	uint num = 0;

	while (num < length && charsToTrim.Contains(inner[num]))
	{
		num++;
	}

	return Substring(num);
}

String String::TrimEnd(const String& charsToTrim) const
{
	uint num = 0;

	while (num < length && charsToTrim.Contains(inner[length - 1 - num]))
	{
		num++;
	}

	return Substring(0, length - num);
}

String String::Trim(const String& charsToTrim) const
{
	return TrimStart(charsToTrim).TrimEnd(charsToTrim);
}

bool String::StartsWith(const String& val) const
{
	if (val.length > length) return false;

	for (uint i = 0; i < val.length; i++)
	{
		if (inner[i] != val.inner[i]) return false;
	}

	return true;
}

bool String::EndsWith(const String& val) const
{
	if (val.length > length) return false;

	for (uint i = 0; i < val.length; i++)
	{
		if (inner[length - val.length + i] != val.inner[i]) return false;
	}

	return true;
}

bool String::Contains(const String& substr) const
{
	return IndexOf(substr) >= 0;
}

bool String::IsEmpty() const
{
	return length == 0;
}

List<String> String::Split(const String& delimiter, bool removeEmpty) const
{
	if (length == 0) return List<String>();

	List<uint> positions = Find(delimiter);

	List<String> result;

	uint i = 0;
	for (uint c = 0; c < positions.Length(); c++)
	{
		const String substring = Substring(i, positions[c] - i);
		if (substring.length > 0 || !removeEmpty)
		{
			result.Add(substring);
		}
		i = positions[c] + delimiter.length;
	}

	if (positions.Length() > 0)
	{
		result.Add(Substring(positions[positions.Length() - 1] + delimiter.length));
	}
	else
	{
		return { *this };
	}

	return result;
}

bool String::ReplaceSingle(String& src, const String& from, const String& to)
{
	const int pos = src.IndexOf(from);
	if (pos != -1)
	{
		const int offset = to.length - from.length;

		char* target = src.inner;
		const bool extend = src.length + offset > src.allocatedLength - 1;

		if (extend)
		{
			target = new char[src.length + offset + 1];
			memcpy(target, src.inner, pos);
		}

		if (offset > 0)
		{
			memcpy_b(target + (pos + to.length), src.inner + (pos + from.length), src.length - from.length - pos);
		}
		else if (offset < 0)
		{
			memcpy(target + (pos + to.length), src.inner + (pos + from.length), src.length - from.length - pos);
		}

		memcpy(target + pos, to.inner, to.length);

		src.length += offset;
		target[src.length] = '\0';

		if (extend)
		{
			delete[] src.inner;
			src.inner = target;
			src.allocatedLength = src.length + 1;
		}

		return true;
	}

	return false;
}

String String::operator*(uint rhs) const
{
	if (rhs == 0) return "";

	char* result = new char[length * rhs];

	for (uint i = 0; i < rhs; i++)
	{
		for (uint j = 0; j < length; j++)
		{
			result[i * length + j] = inner[j];
		}
	}

	return String(result, length * rhs);
}

String String::operator+(const String& rhs) const
{
	char* result = new char[length + rhs.length];

	memcpy(result, inner, length);
	memcpy(result + length, rhs.inner , rhs.length);

	return String(result, length + rhs.length);
}

void String::operator+=(const String& rhs)
{
	const bool extend = length + rhs.length > allocatedLength;

	char* target = inner;
	if (extend)
	{
		target = new char[length + rhs.length];
		memcpy(target, inner, length);
	}

	memcpy(target + length, rhs.inner, rhs.length);

	length += rhs.length;

	if (extend)
	{
		delete[] inner;
		inner = target;
		allocatedLength = length;
	}
}

bool String::operator==(const String& rhs) const
{
	if (length != rhs.length) return false;

	for (uint i = 0; i < length; i++)
	{
		if (inner[i] != rhs.inner[i]) return false;
	}

	return true;
}

bool String::operator!=(const String& rhs) const
{
	return !operator==(rhs);
}

bool String::operator<(const String& rhs) const
{
	for (uint i = 0; i < min(length, rhs.length); i++)
	{
		if (inner[i] < rhs.inner[i]) return true;
		if (inner[i] > rhs.inner[i]) return false;
	}

	return length < rhs.length;
}

bool String::operator<=(const String& rhs) const
{
	for (uint i = 0; i < min(length, rhs.length); i++)
	{
		if (inner[i] < rhs.inner[i]) return true;
		if (inner[i] > rhs.inner[i]) return false;
	}

	return length <= rhs.length;
}

bool String::operator>(const String& rhs) const
{
	for (uint i = 0; i < min(length, rhs.length); i++)
	{
		if (inner[i] > rhs.inner[i]) return true;
		if (inner[i] < rhs.inner[i]) return false;
	}

	return length > rhs.length;
}

bool String::operator>=(const String& rhs) const
{
	for (uint i = 0; i < min(length, rhs.length); i++)
	{
		if (inner[i] > rhs.inner[i]) return true;
		if (inner[i] < rhs.inner[i]) return false;
	}

	return length >= rhs.length;
}

char& String::operator[](uint index)
{
	if (index >= length)
	{
		throw new std::out_of_range(StringFormat("Parameter \"index\" is greater than last character index - %i", length - 1).c());
	}

	return inner[index];
}

const char& String::operator[](uint index) const
{
	if (index >= length)
	{
		throw new std::out_of_range(StringFormat("Parameter \"index\" is greater than last character index - %i", length - 1).c());
	}

	return inner[index];
}
