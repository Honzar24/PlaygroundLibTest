#include "./Logger.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <string.h>

#ifdef _WIN32
//#include <Windows.h>

//#include <DbgHelp.h>
//#pragma comment( lib, "dbghelp.lib" )

#	if defined(_MSC_VER) && defined(__clang__)
#		define WIN32_LEAN_AND_MEAN
#		define VC_EXTRALEAN
#	endif
#	include <windows.h>   // WinApi header
#endif

#ifdef __ANDROID_API__
#	include <android/log.h>
#endif

#include "../FileUtils/FileMacros.h"
#include "../Macros.h"

using namespace MyUtils;

std::shared_ptr<Logger> Logger::instanceLogger = nullptr;


Logger::Logger() :
	colorsEnabled(false),
	enableErrors{ true, true, true },
	enableWarnings{ true, true, true },
#if defined(_DEBUG) || defined(DEBUG)
	enableInfo{ true, true, true },
#else
	enableInfo{ false, false, false },
#endif
	loggerOutput{ 0, 0, 0 }
{
	this->LogToStdout();
}

Logger::~Logger()
{
	if (this->loggerOutput[LOG_FILE] != nullptr)
	{
		fclose(this->loggerOutput[1]);
	}
}

void Logger::Initialize()
{
	instanceLogger = std::shared_ptr<Logger>(new Logger());
}

void Logger::Destroy()
{
	instanceLogger = nullptr;
}

std::shared_ptr<Logger> Logger::GetInstance()
{
	if (instanceLogger == nullptr)
	{
		Logger::Initialize();
	}

	return instanceLogger;
}


void Logger::LogToFile(const char* fileName)
{
	this->loggerOutput[LOG_FILE] = nullptr;

	my_fopen(&this->loggerOutput[LOG_FILE], fileName, "w");
}

void Logger::LogToStdout()
{
	this->loggerOutput[LOG_STDOUT] = stdout;
}

void Logger::LogToStder()
{
	this->loggerOutput[LOG_STDERR] = stderr;
}

void Logger::SetColorsEnabled(bool val)
{
	this->colorsEnabled = val;
}

void Logger::StartColor(int colorID)
{
	if (colorsEnabled == false)
	{
		return;
	}

#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorID);
#endif
}

void Logger::EndColor(int colorID)
{
	if (colorsEnabled == false)
	{
		return;
	}

#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorID);
#endif
}

void Logger::LogError(const char* message, ...)
{
	auto l = Logger::GetInstance();



	va_list vl;
	va_start(vl, message);

	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == nullptr)
		{
			continue;
		}
		if (l->enableErrors[i] == false)
		{
			continue;
		}

#ifdef __ANDROID_API__
		__android_log_vprint(ANDROID_LOG_ERROR, "CLogger", message, vl);
#else
		l->StartColor(Logger::COLOR_RED);
		fprintf(l->loggerOutput[i], "[Error] ");
		l->EndColor(Logger::COLOR_WHITE);

		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");

		fflush(l->loggerOutput[i]);
#endif
	}
	va_end(vl);



}


void Logger::LogWarning(const char* message, ...)
{
	auto l = Logger::GetInstance();


	va_list vl;
	va_start(vl, message);

	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == nullptr)
		{
			continue;
		}
		if (l->enableWarnings[i] == false)
		{
			continue;
		}

#ifdef __ANDROID_API__
		__android_log_vprint(ANDROID_LOG_WARN, "CLogger", message, vl);
#else
		l->StartColor(Logger::COLOR_YELLOW);
		fprintf(l->loggerOutput[i], "[Warning] ");
		l->EndColor(Logger::COLOR_WHITE);

		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");
		fflush(l->loggerOutput[i]);
#endif
	}

	va_end(vl);


}

void Logger::LogInfo(const char* message, ...)
{
	auto l = Logger::GetInstance();



	va_list vl;
	va_start(vl, message);

	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == nullptr)
		{
			continue;
		}
		if (l->enableInfo[i] == false)
		{
			continue;
		}

#ifdef __ANDROID_API__
		__android_log_vprint(ANDROID_LOG_INFO, "CLogger", message, vl);
#else
		l->StartColor(Logger::COLOR_GREEN);
		fprintf(l->loggerOutput[i], "[Info] ");
		l->EndColor(Logger::COLOR_WHITE);

		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");
		fflush(l->loggerOutput[i]);
#endif

	}

	va_end(vl);


}

void Logger::LogMessage(const char* message, ...)
{
	auto l = Logger::GetInstance();

	va_list vl;
	va_start(vl, message);

	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == nullptr)
		{
			continue;
		}

#ifdef __ANDROID_API__
		__android_log_vprint(ANDROID_LOG_DEBUG, "CLogger", message, vl);
#else
		vfprintf(l->loggerOutput[i], message, vl);
		fflush(l->loggerOutput[i]);
#endif
	}

	va_end(vl);


}



void Logger::DisableErrorLogging(LOG_OUTPUT type)
{
	this->enableErrors[type] = false;
}

void Logger::DisableWarningLogging(LOG_OUTPUT type)
{
	this->enableWarnings[type] = false;
}

void Logger::DisableInfoLogging(LOG_OUTPUT type)
{
	this->enableInfo[type] = false;
}


void Logger::EnableErrorLogging(LOG_OUTPUT type)
{
	this->enableErrors[type] = true;
}

void Logger::EnableWarningLogging(LOG_OUTPUT type)
{
	this->enableWarnings[type] = true;
}

void Logger::EnableInfoLogging(LOG_OUTPUT type)
{
	this->enableInfo[type] = true;
}


bool Logger::IsErrorLoggingEnabled(LOG_OUTPUT type)
{
	return this->enableErrors[type];
}

bool Logger::IsWarningLoggingEnabled(LOG_OUTPUT type)
{
	return this->enableWarnings[type];
}

bool Logger::IsInfoLoggingEnabled(LOG_OUTPUT type)
{
	return this->enableInfo[type];
}