#pragma once

#include "Engine/framework.h"

enum class TileType
{
    None    = 0,
    Air     = 1,
    Liquid  = 2,
    Complex = 4,
    Solid   = 8
};
inline TileType& operator|=(TileType& Lhs, TileType rhs) { return Lhs = (TileType)((__underlying_type(TileType))Lhs | (__underlying_type(TileType))rhs); }
inline TileType& operator&=(TileType& Lhs, TileType rhs) { return Lhs = (TileType)((__underlying_type(TileType))Lhs & (__underlying_type(TileType))rhs); }
inline TileType& operator^=(TileType& Lhs, TileType rhs) { return Lhs = (TileType)((__underlying_type(TileType))Lhs ^ (__underlying_type(TileType))rhs); }
inline constexpr TileType operator| (TileType Lhs, TileType rhs) { return (TileType)((__underlying_type(TileType))Lhs | (__underlying_type(TileType))rhs); }
inline constexpr TileType operator& (TileType Lhs, TileType rhs) { return (TileType)((__underlying_type(TileType))Lhs & (__underlying_type(TileType))rhs); }
inline constexpr TileType operator^ (TileType Lhs, TileType rhs) { return (TileType)((__underlying_type(TileType))Lhs ^ (__underlying_type(TileType))rhs); }
inline constexpr bool operator! (TileType e) { return !(__underlying_type(TileType))e; }
inline constexpr TileType operator~ (TileType e) { return (TileType)~(__underlying_type(TileType))e; };