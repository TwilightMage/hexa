#include "String.h"

#include "BasicTypes.h"
#include "framework.h"

String::String()
	: inner_(new char('\0'))
    , length_(0)
    , allocated_length_(1)
{
}

String::String(const String& src)
	: inner_(new char[src.length_ + 1])
    , length_(src.length_)
    , allocated_length_(src.allocated_length_)
{
	memcpy(inner_, src.inner_, length_);
	inner_[length_] = '\0';
}

String::String(const std::string& str)
	: inner_(new char[str.length() + 1])
    , length_(static_cast<uint>(str.length()))
    , allocated_length_(static_cast<uint>(str.length() + 1))
{
	memcpy(inner_, str.data(), length_);
	inner_[length_] = '\0';
}

String::String(char ch, uint count)
	: inner_(new char[count + 1])
    , length_(count)
    , allocated_length_(count + 1)
{
	for (uint i = 0; i < count; i++)
	{
		inner_[i] = ch;
	}
	inner_[length_] = '\0';
}

String::String(const std::initializer_list<char>& il)
	: inner_(new char[il.size() + 1])
	, length_(static_cast<uint>(il.size()))
	, allocated_length_(static_cast<uint>(il.size() + 1))
{
	memcpy(inner_, il.begin(), il.size());
	inner_[length_] = '\0';
}

String::String(const std::vector<char>& vec)
	: inner_(new char[vec.size() + 1])
    , length_(static_cast<uint>(vec.size()))
    , allocated_length_(static_cast<uint>(vec.size() + 1))
{
	memcpy(inner_, vec.data(), vec.size());
	inner_[length_] = '\0';
}

String::String(const List<char>& list)
	: inner_(new char[list.length() + 1])
	, length_(list.length())
	, allocated_length_(list.length() + 1)
{
	memcpy(inner_, list.get_data(), list.length());
	inner_[length_] = '\0';
}

String::String(char ch)
	: inner_(new char[2])
    , length_(1)
    , allocated_length_(2)
{
	inner_[0] = ch;
	inner_[1] = '\0';
}

String::String(const char* c_str)
	: inner_(new char[strlen(c_str) + 1])
    , length_(static_cast<uint>(strlen(c_str)))
    , allocated_length_(static_cast<uint>(strlen(c_str) + 1))
{
	memcpy(inner_, c_str, strlen(c_str));
	inner_[length_] = '\0';
}

String::String(const char* c_str, uint size)
	: inner_(new char[size + 1])
	, length_(size)
    , allocated_length_(size + 1)
{
	memcpy(inner_, c_str, size);
	inner_[length_] = '\0';
}

String& String::operator=(const String& rhs)
{
	if (this == &rhs) return *this;

	delete[] inner_;

	inner_ = new char[rhs.length_ + 1];
	memcpy(inner_, rhs.inner_, rhs.length_ + 1);
	length_ = rhs.length_;
	allocated_length_ = rhs.allocated_length_;

	return *this;
}

String::~String()
{
	delete[] inner_;
}

std::string String::std() const
{
	return inner_;
}

const char* String::c() const
{
	return inner_;
}

char* String::c_copy() const
{
	char* result = new char[length_ + 1];
	memcpy(result, inner_, length_ + 1);
	return result;
}

wchar_t* String::wc() const
{
	wchar_t* result = new wchar_t[length_];

	mbstowcs_s(nullptr, result, length_ + 1, inner_, length_);

	return result;
}

std::vector<char> String::vec() const
{
	return std::vector<char>(inner_, inner_ + length_);
}

List<char> String::list() const
{
	return List<char>(inner_, length_);
}

uint String::length() const
{
	return length_;
}

uint String::allocated_length() const
{
	return allocated_length_;
}

int String::index_of(const String& substr) const
{
	if (substr.length_ > length_) return -1;

	for (uint i = 0; i < length_ - substr.length_ + 1; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length_; j++)
		{
		    if (inner_[j + i] != substr.inner_[j])
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

int String::last_index_of(const String& substr) const
{
	if (substr.length_ > length_) return -1;

	for (uint i = substr.length_; i < length_; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length_; j++)
		{
			if (inner_[length_ - i + j] != substr.inner_[j])
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			return length_ - i;
		}
	}

	return -1;
}

String String::substring(uint start, uint num) const
{
	const uint min = min(length_, start + num);
	char* result = new char[min - start];

	for (uint i = start; i < min; i++)
	{
		result[i - start] = inner_[i];
	}

	return String(result, min - start);
}

String String::substring(uint start) const
{
	return substring(start, length_ - start);
}

String String::replace(const String& from, const String& to) const
{
	List<uint> positions = find(from);
	const int offset = to.length_ - from.length_;

	const uint newLength = static_cast<uint>(length_ + positions.length() * offset);
	char* result = new char[newLength];

	uint counter = 0;
	for (uint i = 0; i < newLength; i++)
	{
		if (counter < positions.length() && i - offset * counter == positions[counter])
		{
			memcpy(&result[i], to.inner_, to.length_);
			counter++;
			i += to.length_ - 1;
		}
		else
		{
			result[i] = inner_[i - offset * counter];
		}
	}

	return String(result, newLength);
}

String String::remove(const String& substring) const
{
	return replace(substring, "");
}

String String::fit(int new_length, char filler) const
{
	const bool keepLeft = new_length > 0;
	new_length = abs(new_length);

	if (new_length < static_cast<int>(length_))
	{
		if (keepLeft) return substring(0, new_length);
		return substring(length_ - new_length);
	}

	if (static_cast<int>(length_) < new_length)
	{
		char* result = new char[new_length];

		if (keepLeft)
		{
			memcpy(result, inner_, length_);
			memcpy(result + length_, std::string(new_length - length_, filler).c_str(), (new_length - length_));
		}
		else
		{
			memcpy(result, std::string(new_length - length_, filler).c_str(), (new_length - length_));
			memcpy(result + (new_length - length_), inner_, length_);
		}

		return String(result, new_length);
	}

	return *this;
}

String String::to_lower() const
{
	String result = *this;

	for (uint i = 0; i < length_; i++)
	{
		result.inner_[i] = std::tolower(result.inner_[i]);
	}

	return result;
}

String String::to_upper() const
{
	String result = *this;

	for (uint i = 0; i < length_; i++)
	{
		result.inner_[i] = std::toupper(result.inner_[i]);
	}

	return result;
}

String String::trim_start(const String& chars_to_trim) const
{
	uint num = 0;

	while (num < length_ && chars_to_trim.contains(inner_[num]))
	{
		num++;
	}

	return substring(num);
}

String String::trim_end(const String& chars_to_trim) const
{
	uint num = 0;

	while (num < length_ && chars_to_trim.contains(inner_[length_ - 1 - num]))
	{
		num++;
	}

	return substring(0, length_ - num);
}

String String::trim(const String& chars_to_trim) const
{
	return trim_start(chars_to_trim).trim_end(chars_to_trim);
}

bool String::starts_with(const String& val) const
{
	if (val.length_ > length_) return false;

	for (uint i = 0; i < val.length_; i++)
	{
		if (inner_[i] != val.inner_[i]) return false;
	}

	return true;
}

bool String::ends_with(const String& val) const
{
	if (val.length_ > length_) return false;

	for (uint i = 0; i < val.length_; i++)
	{
		if (inner_[length_ - val.length_ + i] != val.inner_[i]) return false;
	}

	return true;
}

bool String::contains(const String& substr) const
{
	return index_of(substr) >= 0;
}

bool String::is_empty() const
{
	return length_ == 0;
}

List<String> String::split(const String& delimiter, bool remove_empty) const
{
	if (length_ == 0) return List<String>();

	auto positions = find(delimiter);

	List<String> result;

	uint i = 0;
	for (uint c = 0; c < positions.length(); c++)
	{
		const auto substr = substring(i, positions[c] - i);
		if (substr.length_ > 0 || !remove_empty)
		{
			result.Add(substr);
		}
		i = positions[c] + delimiter.length_;
	}

	if (positions.length() > 0)
	{
		result.Add(substring(positions[positions.length() - 1] + delimiter.length_));
	}
	else
	{
		return { *this };
	}

	return result;
}

List<uint> String::find(const String& substr) const
{
	List<uint> result;

	for (uint i = 0; i < length_ - substr.length_ + 1; i++)
	{
		bool flag = true;

		for (uint j = 0; j < substr.length_; j++)
		{
			if (inner_[i + j] != substr.inner_[j])
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			result.Add(i);
			i += substr.length_ - 1;
		}
	}

	return result;
}

bool String::replace_single(String& src, const String& from, const String& to)
{
	const int pos = src.index_of(from);
	if (pos != -1)
	{
		const int offset = to.length_ - from.length_;

		char* target = src.inner_;
		const bool extend = src.length_ + offset > src.allocated_length_ - 1;

		if (extend)
		{
			target = new char[src.length_ + offset + 1];
			memcpy(target, src.inner_, pos);
		}

		if (offset > 0)
		{
			memcpy_b(target + (pos + to.length_), src.inner_ + (pos + from.length_), src.length_ - from.length_ - pos);
		}
		else if (offset < 0)
		{
			memcpy(target + (pos + to.length_), src.inner_ + (pos + from.length_), src.length_ - from.length_ - pos);
		}

		memcpy(target + pos, to.inner_, to.length_);

		src.length_ += offset;
		target[src.length_] = '\0';

		if (extend)
		{
			delete[] src.inner_;
			src.inner_ = target;
			src.allocated_length_ = src.length_ + 1;
		}

		return true;
	}

	return false;
}

String String::operator*(uint rhs) const
{
	if (rhs == 0) return "";

	char* result = new char[length_ * rhs];

	for (uint i = 0; i < rhs; i++)
	{
		for (uint j = 0; j < length_; j++)
		{
			result[i * length_ + j] = inner_[j];
		}
	}

	return String(result, length_ * rhs);
}

String String::operator+(const String& rhs) const
{
	char* result = new char[length_ + rhs.length_];

	memcpy(result, inner_, length_);
	memcpy(result + length_, rhs.inner_ , rhs.length_);

	return String(result, length_ + rhs.length_);
}

void String::operator+=(const String& rhs)
{
	const bool extend = length_ + rhs.length_ > allocated_length_;

	char* target = inner_;
	if (extend)
	{
		target = new char[length_ + rhs.length_];
		memcpy(target, inner_, length_);
	}

	memcpy(target + length_, rhs.inner_, rhs.length_);

	length_ += rhs.length_;

	if (extend)
	{
		delete[] inner_;
		inner_ = target;
		allocated_length_ = length_;
	}
}

bool String::operator==(const String& rhs) const
{
	if (length_ != rhs.length_) return false;

	for (uint i = 0; i < length_; i++)
	{
		if (inner_[i] != rhs.inner_[i]) return false;
	}

	return true;
}

bool String::operator!=(const String& rhs) const
{
	return !operator==(rhs);
}

bool String::operator<(const String& rhs) const
{
	for (uint i = 0; i < min(length_, rhs.length_); i++)
	{
		if (inner_[i] < rhs.inner_[i]) return true;
		if (inner_[i] > rhs.inner_[i]) return false;
	}

	return length_ < rhs.length_;
}

bool String::operator<=(const String& rhs) const
{
	for (uint i = 0; i < min(length_, rhs.length_); i++)
	{
		if (inner_[i] < rhs.inner_[i]) return true;
		if (inner_[i] > rhs.inner_[i]) return false;
	}

	return length_ <= rhs.length_;
}

bool String::operator>(const String& rhs) const
{
	for (uint i = 0; i < min(length_, rhs.length_); i++)
	{
		if (inner_[i] > rhs.inner_[i]) return true;
		if (inner_[i] < rhs.inner_[i]) return false;
	}

	return length_ > rhs.length_;
}

bool String::operator>=(const String& rhs) const
{
	for (uint i = 0; i < min(length_, rhs.length_); i++)
	{
		if (inner_[i] > rhs.inner_[i]) return true;
		if (inner_[i] < rhs.inner_[i]) return false;
	}

	return length_ >= rhs.length_;
}

char& String::operator[](uint index)
{
	if (index >= length_)
	{
		throw new std::out_of_range(StringFormat("Parameter \"index\" is greater than last character index - %i", length_ - 1).c());
	}

	return inner_[index];
}

const char& String::operator[](uint index) const
{
	if (index >= length_)
	{
		throw new std::out_of_range(StringFormat("Parameter \"index\" is greater than last character index - %i", length_ - 1).c());
	}

	return inner_[index];
}
