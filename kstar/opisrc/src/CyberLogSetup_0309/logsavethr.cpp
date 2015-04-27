#include "logsavethr.h"

LogSaveThr::LogSaveThr(const CyberLogSetup *pSetup):mpSetup(pSetup)
{
	start();
}

LogSaveThr::~LogSaveThr()
{
}

void LogSaveThr::run()
{
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return;
	};

	qDebug("Started Thread");
}
