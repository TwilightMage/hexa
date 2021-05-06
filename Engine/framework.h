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
    #define EXTERN extern "C"
    #define EXPORT __declspec(dllexport)
#else
    #define EXTERN
    #define EXPORT
#endif

// Platform-specific definitions
#if PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define VK_USE_PLATFORM_WIN32_KHR
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
FORCEINLINE To* cast(Shared<From>& obj)
{
	return dynamic_cast<To*>(obj.get());
}

template<typename To, typename From>
FORCEINLINE To* cast(Unique<From>& obj)
{
	return dynamic_cast<To*>(obj.get());
}

template<typename To, typename From>
FORCEINLINE To& cast_object(From& rhs)
{
	static_assert(sizeof(To) == sizeof(From));
	return *reinterpret_cast<To*>(&rhs);
}

template<typename To, typename From>
FORCEINLINE const To& cast_object(const From& rhs)
{
	static_assert(sizeof(To) == sizeof(From));
	return *reinterpret_cast<const To*>(&rhs);
}

// just for fun :)
// access class members on object, even private
template<typename MemberType, typename ObjectType>
FORCEINLINE MemberType* hack_member(ObjectType* obj, uint byte_offset)
{
	return reinterpret_cast<MemberType*>(reinterpret_cast<byte*>(obj) + byte_offset);
}

#define assert_error(cond, retval, category, message, ...) \
if (!(cond))\
{\
	print_error(category, message, __VA_ARGS__);\
	return retval;\
}\

#define assert_warning(cond, retval, category, message, ...) \
if (!(cond))\
{\
print_warning(category, message, __VA_ARGS__);\
return retval;\
}\



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
}																	\

#define IMPLEMENT_ENUM_FLAGS(type)													\
inline type operator|(type a, type b)												\
{																					\
	return static_cast<type>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));	\
}																					\
inline void operator|=(type& a, type b)												\
{																					\
	a = static_cast<type>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));		\
}																					\
inline type operator&(type a, type b)												\
{																					\
	return static_cast<type>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));	\
}																					\
inline void operator&=(type& a, type b)												\
{																					\
	a = static_cast<type>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));		\
}																					\
inline type operator~(type a)														\
{																					\
	return static_cast<type>(~static_cast<uint32_t>(a));							\
}																					\
