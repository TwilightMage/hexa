#pragma once

#include <rapidjson/document.h>

#include "String.h"

EXTERN class EXPORT JSON
{
public:
    JSON();
    JSON(const JSON& rhs);

    JSON& operator=(const JSON& rhs);
    

    // String conversion interface
    
    static JSON parse(const String& string);
    static Shared<JSON> try_parse(const String& string);
    String to_string() const;
    

    // Member accessing interface
    // get - get optional value if it exists and it's type matches, returns default_value otherwise
    // request - use this if you want to check whether value exists or not without halting
    // set - set value
    // with - set value and get reference to this JSON. Used for chaining

    // bool
    bool get_bool(const String& name, bool default_value = false) const;
    Shared<bool> request_bool(const String& name) const;
    FORCEINLINE void set_bool(const String& name, bool value);
    JSON& with_bool(const String& name, bool value);

    // int
    int get_int(const String& name, int default_value = 0) const;
    Shared<int> request_int(const String& name) const;
    FORCEINLINE void set_int(const String& name, int value);
    JSON& with_int(const String& name, int value);

    // int64
    int64 get_int64(const String& name, int64 default_value = 0) const;
    Shared<int64> request_int64(const String& name) const;
    FORCEINLINE void set_int64(const String& name, int64 value);
    JSON& with_int64(const String& name, int64 value);

    // float
    float get_float(const String& name, float default_value = 0.0f) const;
    Shared<float> request_float(const String& name) const;
    FORCEINLINE void set_float(const String& name, float value);
    JSON& with_float(const String& name, float value);

    // String
    String get_string(const String& name, const String& default_value = "") const;
    Shared<String> request_string(const String& name) const;
    FORCEINLINE void set_string(const String& name, const String& value);
    JSON& with_string(const String& name, const String& value);

    // JSON
    JSON get_object(const String& name, const JSON& default_value = JSON()) const;
    Shared<JSON> request_object(const String& name) const;
    FORCEINLINE void set_object(const String& name, const JSON& value);
    JSON& with_object(const String& name, const JSON& value);

    // List<bool>
    List<bool> get_bool_array(const String& name, const List<bool>& default_value = {}) const;
    Shared<List<bool>> request_bool_array(const String& name) const;
    void set_bool_array(const String& name, const List<bool>& value);
    JSON& with_bool_array(const String& name, const List<bool>& value);

    // List<int>
    List<int> get_int_array(const String& name, const List<int>& default_value = {}) const;
    Shared<List<int>> request_int_array(const String& name) const;
    void set_int_array(const String& name, const List<int>& value);
    JSON& with_int_array(const String& name, const List<int>& value);

    // List<int64>
    List<int64> get_int64_array(const String& name, const List<int64>& default_value = {}) const;
    Shared<List<int64>> request_int64_array(const String& name) const;
    void set_int64_array(const String& name, const List<int64>& value);
    JSON& with_int64_array(const String& name, const List<int64>& value);

    // List<float>
    List<float> get_float_array(const String& name, const List<float>& default_value = {}) const;
    Shared<List<float>> request_float_array(const String& name) const;
    void set_float_array(const String& name, const List<float>& value);
    JSON& with_float_array(const String& name, const List<float>& value);

    // List<String>
    List<String> get_string_array(const String& name, const List<String>& default_value = {}) const;
    Shared<List<String>> request_string_array(const String& name) const;
    void set_string_array(const String& name, const List<String>& value);
    JSON& with_string_array(const String& name, const List<String>& value);

    // List<JSON>
    List<JSON> get_object_array(const String& name, const List<JSON>& default_value = {}) const;
    Shared<List<JSON>> request_object_array(const String& name) const;
    void set_object_array(const String& name, const List<JSON>& value);
    JSON& with_object_array(const String& name, const List<JSON>& value);

private:
    Unique<rapidjson::Document> document_;
};
