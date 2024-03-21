
#include <iostream>
#include <mld/logging/Logger.hxx>

using namespace std;
using namespace Teamcenter;
using namespace Teamcenter::Logging;

static void writeLoggingLevelsToSyslog(Logger *logger)
{
	if (logger->isTraceEnabled())
	{
		logger->info("Trace logging is enabled");
	}
	else
	{
		logger->info("Trace logging is disabled");
	}

	if (logger->isDebugEnabled())
	{
		logger->info("Debug logging is enabled");
	}
	else
	{
		logger->info("Debug logging is disabled");
	}
	
	if (logger->isInfoEnabled())
	{
		logger->info("Info logging is enabled");
	}
	else
	{
		logger->info("Info logging is disabled");
	}
	
	if (logger->isErrorEnabled())
	{
		logger->info("Error logging is enabled");
	}
	else
	{
		logger->info("Error logging is disabled");
	}
	
	if (logger->isFatalEnabled())
	{
		logger->info("Fatal logging is enabled");
	}
	else
	{
		logger->info("Fatal logging is disabled");
	}
}

static void do_it(void)
{
	std::string loggerName = "Teamcenter.GTAC.MyCode";
	Logger *logger = Logger::getLogger(loggerName);
	cout << "Using Logger " + logger->getLoggerName() << endl;
	writeLoggingLevelsToSyslog(logger);
}