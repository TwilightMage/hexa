#include "LogStream.h"

#include <iostream>

#include "Path.h"

LogStream::LogStream(const DateTime& LogStartTime, const char* root)
	: std::ostream(&tbuf)
{
	Path logs = Path(root).Up().GetChild("logs");

	logs.Create();
	timedLogFile = std::ofstream(logs.GetChild(LogStartTime.ToString("%Y_%m_%d_%H_%M_%S") + ".log").ToString().c());
	latestLogFile = std::ofstream(logs.GetChild("latest.log").ToString().c());

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