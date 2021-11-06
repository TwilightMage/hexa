#pragma once

#include <chrono>

#include "Map.h"

typedef std::chrono::duration<long long, std::ratio<1, 1000000000>> dur_nano;

template<typename... TimePointTypes>
FORCEINLINE void measure_time_all_impl(List<dur_nano>& intervals, TimePointTypes... time_points)
{
    List<std::chrono::time_point<std::chrono::system_clock>> time_points_list = {time_points...};
    for (uint i = 0; i < sizeof...(time_points) - 1; i++)
    {
        intervals.add(time_points_list[i + 1] - time_points_list[i]);
    }
}

#define get_now(name) const std::chrono::time_point<std::chrono::system_clock> name = std::chrono::system_clock::now();
#define measure_time(name, a, b) const dur_nano name = b - a;
#define measure_now(name, a) const dur_nano name = std::chrono::system_clock::now() - a;
#define measure_time_all(name, ...) List<dur_nano> name; measure_time_all_impl(name, __VA_ARGS__);
    
