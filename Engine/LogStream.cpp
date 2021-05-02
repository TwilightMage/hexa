#include "LogStream.h"

#include <iostream>

#include "Path.h"

LogStream::LogStream(const DateTime& LogStartTime, const char* root)
	: std::ostream(&tbuf)
{
	Path logs = Path(root).up().get_child("logs");

	logs.create();
	timedLogFile = std::ofstream(logs.get_child(LogStartTime.ToString("%Y_%m_%d_%H_%M_%S") + ".log").to_string().c());
	latestLogFile = std::ofstream(logs.get_child("latest.log").to_string().c());

	auto a = timedLogFile.is_open();
	auto b = latestLogFile.is_open();

	tbuf = TeeBuf({ std::cout.rdbuf(), timedLogFile.rdbuf(), latestLogFile.rdbuf() });
}

LogStream::~LogStream()
{
	timedLogFile << "\n";
	timedLogFile.close();

	latestLogFile << "\n";
	latestLogFile.close();
}