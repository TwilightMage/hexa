#pragma once

#include <chrono>

#include "Map.h"

template<typename... TimePointTypes>
FORCEINLINE void measure_time_all_impl(Map<uint, std::chrono::duration<long long, std::ratio<1, 10000000>>>& map, TimePointTypes... time_points)
{
    List<std::chrono::time_point<std::chrono::system_clock>> list = {time_points...};
    for (uint i = 0; i < sizeof...(time_points) - 1; i++)
    {
        map[i] = list[i + 1] - list[i];
    }
}

#define get_now(name) const std::chrono::time_point<std::chrono::system_clock> name = std::chrono::system_clock::now();
#define measure_time(name, a, b) const std::chrono::duration<long long, std::ratio<1, 10000000>> name = b - a;
#define measure_now(name, a) const std::chrono::duration<long long, std::ratio<1, 10000000>> name = std::chrono::system_clock::now() - a;
#define measure_time_all(name, ...) Map<uint, std::chrono::duration<long long, std::ratio<1, 10000000>>> name; measure_time_all_impl(name, __VA_ARGS__);
    
