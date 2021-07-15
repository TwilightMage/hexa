#pragma once

#include "DatabaseBase.h"
#include "Map.h"
#include "Name.h"
#include "Pointers.h"
#include "String.h"

template<class T>
class Database : public DatabaseBase
{
public:
    explicit Database(const String& name);

    template<class SubT>
    Shared<const SubT> add(SubT* record);
    Shared<const T> get(const Name& key) const;
    const Map<Name, Shared<T>>& records() const;

private:
    Map<Name, Shared<T>> data_;
};

#include "Database.cpp"