#include "JSON.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "Logger.h"

using namespace rapidjson;

#ifdef GetObject
#pragma push_macro("GetObject")
#define GetObject_APPLIED
#undef GetObject
#endif

FORCEINLINE GenericStringRef<char> str(const String& string)
{
    return StringRef(string.c_copy(), string.length());
}

JSON::JSON()
    : document_(new Document())
{
    document_->Parse("{}");
}

JSON::JSON(const JSON& rhs)
{
    document_.reset(new Document());
    document_->CopyFrom(*rhs.document_, document_->GetAllocator(), true);
}

JSON& JSON::operator=(const JSON& rhs)
{
    if (this == &rhs) return *this;
    
    document_->CopyFrom(*rhs.document_, document_->GetAllocator(), true);
    return *this;
}

JSON JSON::parse(const String& string)
{
    JSON result;
    result.document_->Parse(string.c());
    
    return result;
}

Shared<JSON> JSON::try_parse(const String& string)
{
    Shared<JSON> result = MakeShared<JSON>();
    result->document_->Parse(string.c());
    if (result->document_->HasParseError())
    {
        print_warning("JSON Parse Error", GetParseError_En(result->document_->GetParseError()));
        return nullptr;
    }
    
    return result;
}

String JSON::to_string() const
{
    StringBuffer buffer;
    Writer writer(buffer);
    document_->Accept(writer);
    return buffer.GetString();
}

bool JSON::get_bool(const String& name, bool default_value) const
{
    const auto member = document_->FindMember(name.c());
    return member != document_->MemberEnd() && member->value.IsBool() ? member->value.GetBool() : default_value;
}

Shared<bool> JSON::request_bool(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsBool())
    {
        return MakeShared<bool>(member->value.GetBool());
    }

    return nullptr;
}

void JSON::set_bool(const String& name, bool value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsBool())
        {
            member->value.SetBool(value);
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), value, document_->GetAllocator());
}

JSON& JSON::with_bool(const String& name, bool value)
{
    set_bool(name, value);
    return *this;
}

int JSON::get_int(const String& name, int default_value) const
{
    const auto member = document_->FindMember(name.c());
    return member != document_->MemberEnd() && member->value.IsInt() ? member->value.GetInt() : default_value;
}

Shared<int> JSON::request_int(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsInt())
    {
        return MakeShared<int>(member->value.GetInt());
    }

    return nullptr;
}

void JSON::set_int(const String& name, int value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsInt())
        {
            member->value.SetInt(value);
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), value, document_->GetAllocator());
}

JSON& JSON::with_int(const String& name, int value)
{
    set_int(name, value);
    return *this;
}

int64 JSON::get_int64(const String& name, int64 default_value) const
{
    const auto member = document_->FindMember(name.c());
    return member != document_->MemberEnd() && (member->value.IsInt64() || member->value.IsInt()) ? member->value.GetInt64() : default_value;
}

Shared<int64> JSON::request_int64(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsInt64() || member->value.IsInt())
    {
        return MakeShared<int64>(member->value.GetInt64());
    }

    return nullptr;
}

void JSON::set_int64(const String& name, int64 value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsInt64())
        {
            member->value.SetInt64(value);
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), value, document_->GetAllocator());
}

JSON& JSON::with_int64(const String& name, int64 value)
{
    set_int64(name, value);
    return *this;
}

float JSON::get_float(const String& name, float default_value) const
{
    const auto member = document_->FindMember(name.c());
    return member != document_->MemberEnd() && (member->value.IsFloat() || member->value.IsInt()) ? member->value.GetFloat() : default_value;
}

Shared<float> JSON::request_float(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && (member->value.IsFloat() || member->value.IsInt()))
    {
        return MakeShared<float>(member->value.GetFloat());
    }

    return nullptr;
}

void JSON::set_float(const String& name, float value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsFloat())
        {
            member->value.SetFloat(value);
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), value, document_->GetAllocator());
}

JSON& JSON::with_float(const String& name, float value)
{
    set_float(name, value);
    return *this;
}

String JSON::get_string(const String& name, const String& default_value) const
{
    const auto member = document_->FindMember(name.c());
    return member != document_->MemberEnd() && member->value.IsString() ? member->value.GetString() : default_value;
}

Shared<String> JSON::request_string(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsString())
    {
        return MakeShared<String>(member->value.GetString());
    }

    return nullptr;
}

void JSON::set_string(const String& name, const String& value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsString())
        {
            member->value.SetString(str(value));
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), str(value), document_->GetAllocator());
}

JSON& JSON::with_string(const String& name, const String& value)
{
    set_string(name, value);
    return *this;
}

JSON JSON::get_object(const String& name, const JSON& default_value) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsObject())
    {
        JSON result;
        result.document_->CopyFrom(member->value, result.document_->GetAllocator());
        return result;
    }
    
    return default_value;
}

Shared<JSON> JSON::request_object(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsObject())
    {
        auto result = MakeShared<JSON>();
        result->document_->CopyFrom(member->value, result->document_->GetAllocator());
        return result;
    }

    return nullptr;
}

void JSON::set_object(const String& name, const JSON& value)
{
    auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd())
    {
        if (member->value.IsObject())
        {
            member->value = value.document_->GetObject();
            return;
        }
        document_->RemoveMember(name.c());
    }
    document_->AddMember(str(name), value.document_->GetObject(), document_->GetAllocator());
}

JSON& JSON::with_object(const String& name, const JSON& value)
{
    set_object(name, value);
    return *this;
}

#define get_array(type, Type)                                   \
const auto member = document_->FindMember(name.c());            \
if (member != document_->MemberEnd() && member->value.IsArray())\
{                                                               \
    const auto arr = member->value.GetArray();                  \
    if (arr.Size() > 0)                                         \
    {                                                           \
        if (!arr[0].Is##Type()) return {};                      \
                                                                \
        List<type> result(arr.Size());                          \
        for (uint i = 0; i < arr.Size(); i++)                   \
        {                                                       \
            result[i] = arr[i].Get##Type();                     \
        }                                                       \
        return result;                                          \
    }                                                           \
                                                                \
    return {};                                                  \
}                                                               \
                                                                \
return default_value;                                           \

#define request_array(type, Type)                                       \
const auto member = document_->FindMember(name.c());                    \
if (member != document_->MemberEnd() && member->value.IsArray())        \
{                                                                       \
    const auto arr = member->value.GetArray();                          \
    if (arr.Size() > 0)                                                 \
    {                                                                   \
        if (!arr[0].Is##Type()) return {};                              \
                                                                        \
        Shared<List<type>> result = MakeShared<List<type>>(arr.Size()); \
        for (uint i = 0; i < arr.Size(); i++)                           \
        {                                                               \
            result->operator[](i) = arr[i].Get##Type();                 \
        }                                                               \
        return result;                                                  \
    }                                                                   \
                                                                        \
    return {};                                                          \
}                                                                       \
                                                                        \
return nullptr;                                                         \

#define set_array()                                 \
auto& val = document_->operator[](name.c());        \
val.SetArray();                                     \
val.Clear();                                        \
for (auto& entry : value)                           \
{                                                   \
    val.PushBack(entry, document_->GetAllocator()); \
}                                                   \

#define with_array(type)        \
set_##type##_array(name, value);\
return *this;                   \
    

List<bool> JSON::get_bool_array(const String& name, const List<bool>& default_value) const
{
    get_array(bool, Bool);
}

Shared<List<bool>> JSON::request_bool_array(const String& name) const
{
    request_array(bool, Bool);
}

void JSON::set_bool_array(const String& name, const List<bool>& value)
{
    set_array();
}

JSON& JSON::with_bool_array(const String& name, const List<bool>& value)
{
    with_array(bool);
}

List<int> JSON::get_int_array(const String& name, const List<int>& default_value) const
{
    get_array(int, Int);
}

Shared<List<int>> JSON::request_int_array(const String& name) const
{
    request_array(int, Int);
}

void JSON::set_int_array(const String& name, const List<int>& value)
{
    set_array();
}

JSON& JSON::with_int_array(const String& name, const List<int>& value)
{
    with_array(int);
}

List<int64> JSON::get_int64_array(const String& name, const List<int64>& default_value) const
{
    get_array(int64, Int64);
}

Shared<List<int64>> JSON::request_int64_array(const String& name) const
{
    request_array(int64, Int64);
}

void JSON::set_int64_array(const String& name, const List<int64>& value)
{
    set_array();
}

JSON& JSON::with_int64_array(const String& name, const List<int64>& value)
{
    with_array(int64);
}

List<float> JSON::get_float_array(const String& name, const List<float>& default_value) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsArray())
    {
        const auto arr = member->value.GetArray();
        if (arr.Size() > 0)
        {
            if (!arr[0].IsFloat() && !arr[0].IsInt()) return {};
            List<float> result(arr.Size());
            for (uint i = 0; i < arr.Size(); i++)
            {
                result[i] = arr[i].GetFloat();
            }
            return result;
        }
        return {};
    }
    return default_value;;
}

Shared<List<float>> JSON::request_float_array(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsArray())
    {
        const auto arr = member->value.GetArray();
        if (arr.Size() > 0)
        {
            if (!arr[0].IsFloat() && !arr[0].IsInt()) return {};
            Shared<List<float>> result = MakeShared<List<float>>(arr.Size());
            for (uint i = 0; i < arr.Size(); i++)
            {
                result->operator[](i) = arr[i].GetFloat();
            }
            return result;
        }
        return {};
    }
    return nullptr;;
}

void JSON::set_float_array(const String& name, const List<float>& value)
{
    set_array();
}

JSON& JSON::with_float_array(const String& name, const List<float>& value)
{
    with_array(float);
}

List<String> JSON::get_string_array(const String& name, const List<String>& default_value) const
{
    get_array(String, String);
}

Shared<List<String>> JSON::request_string_array(const String& name) const
{
    request_array(String, String);
}

void JSON::set_string_array(const String& name, const List<String>& value)
{
    auto& val = document_->operator[](name.c());
    val.SetArray();
    for (auto& entry : value)
    {
        val.PushBack(str(entry), document_->GetAllocator());
    }
}

JSON& JSON::with_string_array(const String& name, const List<String>& value)
{
    with_array(string);
}

List<JSON> JSON::get_object_array(const String& name, const List<JSON>& default_value) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsArray())
    {
        const auto arr = member->value.GetArray();
        if (arr.Size() > 0)
        {
            if (!arr[0].IsObject()) return {};
            List<JSON> result(arr.Size());
            for (uint i = 0; i < arr.Size(); i++)
            {
                result[i].document_->CopyFrom(arr[i], result[i].document_->GetAllocator());
            }
            return result;
        }
        return {};
    }
    return default_value;;
}

Shared<List<JSON>> JSON::request_object_array(const String& name) const
{
    const auto member = document_->FindMember(name.c());
    if (member != document_->MemberEnd() && member->value.IsArray())
    {
        const auto arr = member->value.GetArray();
        if (arr.Size() > 0)
        {
            if (!arr[0].IsObject()) return {};
            Shared<List<JSON>> result = MakeShared<List<JSON>>(arr.Size());
            for (uint i = 0; i < arr.Size(); i++)
            {
                result->operator[](i).document_->CopyFrom(arr[i], result->operator[](i).document_->GetAllocator());
            }
            return result;
        }
        return {};
    }
    return nullptr;;
}

void JSON::set_object_array(const String& name, const List<JSON>& value)
{
    Value arr = Value(kArrayType);
    for (auto& entry : value)
    {
        arr.PushBack(entry.document_->GetObject(), document_->GetAllocator());
    }
    document_->AddMember(str(name), arr, document_->GetAllocator());
}

JSON& JSON::with_object_array(const String& name, const List<JSON>& value)
{
    with_array(object);
}

#ifdef GetObject_APPLIED
#pragma pop_macro("GetObject")
#undef GetObject_APPLIED
#endif