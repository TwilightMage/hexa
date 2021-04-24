#pragma once

#include <vector>
#include <cstdint>
#include <memory>

typedef unsigned int uint;
typedef int64_t int64;
typedef uint64_t uint64;

typedef uint8_t byte;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<class T, class... Types>
Shared<T> MakeShared(Types&&... Args)
{
	return std::make_shared<T, Types...>(std::forward<Types>(Args)...);
}

#define MakeSharedInternal(T, ...)		(Shared<T>(new T(__VA_ARGS__)))

template<typename To, typename From>
To* Cast(From* obj)
{
	return dynamic_cast<To*>(obj);
}

template<typename To, typename From>
To* Cast(Shared<From>& obj)
{
	return dynamic_cast<To*>(obj.get());
}

template<typename To, typename From>
To* Cast(Unique<From>& obj)
{
	return dynamic_cast<To*>(obj.get());
}