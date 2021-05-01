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

        [[nodiscard]] float GetTotalHours() const;
		[[nodiscard]] float GetTotalMinutes() const;
		[[nodiscard]] float GetTotalSeconds() const;
		[[nodiscard]] int GetTotalMilliseconds() const;
	};

	DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond = 0);
	DateTime();

	[[nodiscard]] String ToString(String format = "%Y.%m.%d %H:%M:%S") const;

	static DateTime Now();
	static Interval EpochTime();

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