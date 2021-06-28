#pragma once

#include "BasicTypes.h"
#include "Engine/Database.h"

#define DECLARE_DATABASE_ENTRY(type, name)            inline static Shared<const type> name = nullptr;
#define INIT_DATABASE_ENTRY(name, type, ...)          name = database->add(new type(#name, __VA_ARGS__));
#define DATABASE_INIT_FUNC(type)                      static void init(const Shared<Database<type>>& database)
#define DATABASE_INIT_FUNC_IMPL(container_type, type) void container_type::init(const Shared<Database<type>>& database)
