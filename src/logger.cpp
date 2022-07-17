#include "logger.h"
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif


namespace logger{
	void Log(LogLevel level, const std::string &message)
	{
		// also print out the time, in the format of "HH:MM:SS"
		auto time = std::chrono::system_clock::now();
		auto intime = std::chrono::system_clock::to_time_t(time);
		std::string time_str = std::ctime(&intime);
		time_str.erase(time_str.end() - 1);
		std::cout << "[" << time_str << "]" << " ";
		// set the color of the output based on the log LogLevel
		switch (level)
		{
		case LogLevel::DEBUG:
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
#else
			std::cout << "\033[1;32m";
#endif
			std::cout << "DEBUG";
			break;
		case LogLevel::INFO:
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
#else
			std::cout << "\033[1;34m";
#endif
			std::cout << "INFO ";
			break;
		case LogLevel::WARNING:
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_YELLOW);
#else
			std::cout << "\033[1;33m";
#endif
			std::cout << "WARN ";
			break;
		case LogLevel::ERROR:
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
#else
			std::cout << "\033[1;31m";
#endif
			std::cout << "ERROR";
			break;
		case LogLevel::FATAL:
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
			std::cout << "\033[1;35m";
#endif
			std::cout << "FATAL";
			break;
		}
		// print the message
		std::cout << " "<< message << "\033[0m" << std::endl;

	}
}
