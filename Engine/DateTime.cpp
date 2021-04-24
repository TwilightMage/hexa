#include "DateTime.h"
#include <chrono>
#include <time.h>

tm ToTm(const DateTime& inTime)
{
	tm t;
	t.tm_year = inTime.year - 1900;
	t.tm_mon = inTime.month - 1;
	t.tm_mday = inTime.day;
	t.tm_hour = inTime.hour;
	t.tm_min = inTime.minute;
	t.tm_sec = inTime.second;

	return t;
}

std::chrono::system_clock::time_point ToTimePoint(const DateTime& inTime)
{
	auto tm = ToTm(inTime);
	auto result = std::chrono::system_clock::from_time_t(mktime(&tm));
	result += std::chrono::milliseconds(inTime.millisecond);

	return result;
}

float DateTime::Interval::GetTotalHours() const
{
	return GetTotalMinutes() / 60.0f;
}

float DateTime::Interval::GetTotalMinutes() const
{
	return GetTotalSeconds() / 60.0f;
}

float DateTime::Interval::GetTotalSeconds() const
{
	return GetTotalMilliseconds() / 1000.0f;
}

int DateTime::Interval::GetTotalMilliseconds() const
{
	return millisecond + (second + (minute + (hour * day * 24) * 60) * 60) * 1000;
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	: year(year)
	, month(month)
	, day(day)
	, hour(hour)
	, minute(minute)
	, second(second)
	, millisecond(millisecond)
{
}

DateTime::DateTime()
	: year(0)
	, month(0)
	, day(0)
	, hour(0)
	, minute(0)
	, second(0)
	, millisecond(0)
{
}

String DateTime::ToString(String format) const
{
    auto t = ToTm(*this);

	char timeStrBuf[26];
    const uint timeStrBufSize = static_cast<uint>(strftime(timeStrBuf, sizeof(timeStrBuf), format.c(), &t));
	return String(timeStrBuf, timeStrBufSize);
}

DateTime DateTime::Now()
{
	time_t timeNow = time(0);
	tm* localTimeNow = new tm();
	localtime_s(localTimeNow, &timeNow);

	auto a = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	return {
		localTimeNow->tm_year + 1900,
		localTimeNow->tm_mon + 1,
		localTimeNow->tm_mday,
		localTimeNow->tm_hour,
		localTimeNow->tm_min,
		localTimeNow->tm_sec,
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000
	};
}

DateTime::Interval DateTime::EpochTime()
{
	return Now() - DateTime{ 1970, 1, 1, 0, 0, 0, 0 };
}

DateTime::operator String() const
{
	return ToString();
}

DateTime::Interval DateTime::operator-(const DateTime& rhs) const
{
	auto myPoint = ToTimePoint(*this);
	auto rhsPoint = ToTimePoint(rhs);

	auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(myPoint - rhsPoint).count();

	const long long sDiv = 1000;
	const long long mDiv = sDiv * 60;
	const long long hDiv = mDiv * 60;
	const long long dDiv = hDiv * 24;

	int day = static_cast<int>((interval) / dDiv);
	int hour = static_cast<int>((interval - (day * dDiv)) / hDiv);
	int minute = static_cast<int>((interval - (day * dDiv) - (hour * hDiv)) / mDiv);
	int second = static_cast<int>((interval - (day * dDiv) - (hour * hDiv) - (minute * mDiv)) / sDiv);
	int millisecond = static_cast<int>((interval - (day * dDiv) - (hour * hDiv) - (minute * mDiv) - (second * sDiv)));

	return { day, hour, minute, second, millisecond };
}