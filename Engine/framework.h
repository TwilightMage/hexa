#pragma once

#pragma warning( disable : 4251 )

// ------------------------ Defines ------------------------
// Platform definitions
#define PLATFORM_WINDOWS 0

#if defined(_WINDOWS)
#undef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS 1
#endif

// Debug definitions
#define DEBUG 0

#if defined(_DEBUG)
#undef DEBUG
#define DEBUG 1
#endif

// Misc definitions
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

#define DLL 1

#if DLL
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#define _GLFW_BUILD_DLL

// Platform-specific definitions
#if PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define VK_USE_PLATFORM_WIN32_KHR
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif



// ------------------------- Libs --------------------------
//#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glfw3_mt.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "reactphysics3d.lib")

// Platform-specific libs
#if PLATFORM_WINDOWS
    #pragma comment(lib, "DbgHelp.lib")
#endif



// ----------------------- Includes ------------------------
// Platform-specific includes
#if PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN 
    #include <windows.h>
#endif

#include "BasicTypes.h"



// -------------------- Functionality ----------------------
// memcpy   - when you move data backward: 0000xxxx0000 -> 00xxxx000000
// memcpy_b - when you move data forward:  0000xxxx0000 -> 000000xxxx00
// Same as memcpy, but works in reverse order
FORCEINLINE void memcpy_b(void* dst, void* src, size_t size)
{
    auto _dst = static_cast<byte*>(dst);
    auto _src = static_cast<byte*>(src);

    for (size_t i = 1; i <= size; i++)
    {
        _dst[size - i] = _src[size - i];
    }
}

template<typename To, typename From>
FORCEINLINE To* cast(From* obj)
{
	return dynamic_cast<To*>(obj);
}

template<typename To, typename From>
FORCEINLINE Shared<To> cast(const Shared<From>& obj)
{
	return std::dynamic_pointer_cast<To>(obj);
}

template<typename To, typename From>
FORCEINLINE Shared<To> cast(const Weak<From>& obj)
{
	return std::dynamic_pointer_cast<To>(obj.lock());
}

template<typename To, typename From>
// use when you know what you're doing
FORCEINLINE To& cast_object(From& rhs)
{
	static_assert(sizeof(To) == sizeof(From));
	return *reinterpret_cast<To*>(&rhs);
}

template<typename To, typename From>
// use when you know what you're doing
FORCEINLINE const To& cast_object(const From& rhs)
{
	static_assert(sizeof(To) == sizeof(From));
	return *reinterpret_cast<const To*>(&rhs);
}

template<typename To, typename From>
// use when you really know what you're doing
FORCEINLINE To& cast_object_unsafe(From& rhs)
{
	return *reinterpret_cast<To*>(&rhs);
}

template<typename To, typename From>
// use when you really know what you're doing
FORCEINLINE const To& cast_object_unsafe(const From& rhs)
{
	return *reinterpret_cast<const To*>(&rhs);
}

// just for fun :)
// access class members on object, even private
template<typename MemberType, typename ObjectType>
FORCEINLINE MemberType* hack_member(ObjectType* obj, uint byte_offset)
{
	return reinterpret_cast<MemberType*>(reinterpret_cast<byte*>(obj) + byte_offset);
}



// --------------------- Enum macros -----------------------
#define IMPLEMENT_ENUM(type)										\
inline type& operator++(type& in) /*prefix ++*/						\
{																	\
	in = static_cast<type>(static_cast<uint32_t>(in) + 1);			\
	return in;														\
}																	\
inline type& operator--(type& in) /*prefix --*/						\
{																	\
	in = static_cast<type>(static_cast<uint32_t>(in) - 1);			\
	return in;														\
}																	\
inline type operator++(type& in, int) /*postfix ++*/				\
{																	\
	type tmp = in;													\
	++in;															\
	return tmp;														\
}																	\
inline type operator--(type& in, int) /*postfix --*/				\
{																	\
	type tmp = in;													\
	--in;															\
	return tmp;														\
}																	\
inline type operator+(type in, int delta)							\
{																	\
	return static_cast<type>(static_cast<uint32_t>(in) + delta);	\
}																	\
inline type operator-(type in, int delta)							\
{																	\
	return static_cast<type>(static_cast<uint32_t>(in) - delta);	\
}

#define IMPLEMENT_ENUM_FLAGS(Type)																											 \
inline           Type& operator|=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); } \
inline           Type& operator&=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); } \
inline           Type& operator^=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs ^ (__underlying_type(Type))rhs); } \
inline constexpr Type  operator| (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); }       \
inline constexpr Type  operator& (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); }       \
inline constexpr Type  operator^ (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs ^ (__underlying_type(Type))rhs); }       \
inline constexpr bool  operator! (Type  e)               { return !(__underlying_type(Type))e; }                                               \
inline constexpr Type  operator~ (Type  e)               { return (Type)~(__underlying_type(Type))e; }                                         \
inline constexpr Type operator<< (Type  lhs, int offset) { return (Type)((__underlying_type(Type))lhs << offset); }                            \
inline constexpr Type operator>> (Type  lhs, int offset) { return (Type)((__underlying_type(Type))lhs >> offset); }