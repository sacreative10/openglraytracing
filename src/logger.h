#include <iostream>


namespace logger {
	enum class LogLevel {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};
	void Log(LogLevel level, const std::string& message);
}
