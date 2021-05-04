#pragma once

#include "String.h"

EXTERN struct EXPORT DateTime
{
	struct Interval
	{
		int day;
		int hour;
		int minute;
		int second;
		int millisecond;

        [[nodiscard]] float get_total_hours() const;
		[[nodiscard]] float get_total_minutes() const;
		[[nodiscard]] float get_total_seconds() const;
		[[nodiscard]] int get_total_milliseconds() const;

		[[nodiscard]] String to_string() const;
	};

	DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond = 0);
	DateTime();

	[[nodiscard]] String to_string(String format = "%Y.%m.%d %H:%M:%S") const;

	static DateTime now();
	static Interval epoch_time();

	operator String() const;
	Interval operator-(const DateTime& rhs) const;
	//DateTime operator+(const DateTime::Interval& rhs) const;

	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int millisecond;
};