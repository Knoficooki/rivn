#pragma once
#include <fstream>
#include <iostream>
#include <string>

#ifdef _NOB_TEST
#	ifndef _Debug
#		define _Debug
#	endif
#	define _NOB_IF_TEST(yes, no) yes
#else
#	define _NOB_IF_TEST(yes, no) no
#endif

#if defined(_Debug) || defined(_DEBUG) || defined(NDEBUG) || defined(_NDEBUG)
#define _NOB_IF_DEBUG(yes, no) yes
#else
#define _NOB_IF_DEBUG(yes, no) no
#endif
#define _NOB_DEBUG(code) _NOB_IF_DEBUG( code ,  )

namespace nob {

	static std::string log_path = "./build/nob.log";

	enum LogLevel : uint8_t {
		MIN = 0,
		TEST,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL,
	};
	static const char* _LogLevelStings[] = {
		"\tBe kind, walk good, be ..?\n\t\tBe, what? Careful? Better? Smarter?\n\t\"No... just... be.\"",
		"[TEST] ",
		"[DEBUG] ",
		"[INFO] ",
		"[WARNING] ",
		"[ERROR] ",
		"[FATAL] ",
	};

	class logger {
	public:
	private:
		std::ofstream log_file;

		// Level, when logged the program should close
		LogLevel exit_level;

		// Levels which indicate the minimum level for:
		// 1. printing to the console or
		// 2. writing the log-file
		LogLevel to_console_level;
		LogLevel to_log_level;

		LogLevel activeLevel;
	public:
		logger(LogLevel console_lvl = LogLevel::WARNING, LogLevel log_lvl = _NOB_IF_DEBUG(LogLevel::MIN, LogLevel::INFO), LogLevel exit_lvl = LogLevel::FATAL);

		template<typename T>
		logger& write(LogLevel lvl, T obj) {
			if(lvl == LogLevel::MIN) {
				log_file << _LogLevelStings[lvl] << "\n";
			}
			if (lvl >to_console_level)
				std::cout << _LogLevelStings[lvl] << obj << "\n";
			if (lvl > to_log_level)
				log_file << _LogLevelStings[lvl] << obj << "\n";

			if (lvl > exit_level)
				exit(-1);

			return *this;
		}

		LogLevel level() {
			return activeLevel;
		}

		void level(LogLevel lvl) {
			activeLevel = lvl;
		}

		// A counter maximum. when the warning counter reaches this number the program errors
		uint16_t warning_max = 0;
		// A counter maximum. when the error counter reaches this number the program errors
		uint16_t error_max = 0;
	};

}
#include <type_traits>

template<typename T>
nob::logger& operator<<(nob::logger& out, T obj) {
	if constexpr (std::is_convertible_v<T, std::string> ||
		std::is_arithmetic_v<T> ||
		std::is_same_v<T, const char*> ||
		std::is_same_v<T, char*>) {
		return out.write(out.level(), obj);
	}
	else if constexpr (std::is_class_v<T>) {
		static_assert(std::is_convertible_v<T, std::string> ||
		std::is_same_v<T, std::ostream&(*)(std::ostream&)>,
					  "Type T must be printable (convertible to std::string or be a stream manipulator)");
		return out.write(out.level(), obj);
	} else {
		static_assert(std::is_convertible_v<T, std::string> ||
		std::is_arithmetic_v<T> ||
		std::is_same_v<T, const char*> ||
		std::is_same_v<T, char*>,
		"Type T must be printable (convertible to std::string, arithmetic, or C-string)");
		return out; // This line will never be reached due to static_assert
	}
}

inline nob::logger& operator<<(nob::logger& out, nob::LogLevel level) {
	out.level(level);
	return out;
}

namespace nob {

	class instance {
		int exit_code = 0;
	public:
		instance(int argc, char** argv);
		int code();

		logger log;
	};

}

namespace nob {

inline instance::instance(int argc, char** argv) :log(_NOB_IF_TEST(LogLevel::MIN,_NOB_DEBUG(LogLevel::DEBUG)))
{

}

inline int instance::code() {
	return exit_code;
}

inline logger::logger(LogLevel console_lvl, LogLevel log_lvl, LogLevel exit_lvl) :
	to_console_level(console_lvl),
	to_log_level(log_lvl),
	exit_level(exit_lvl),
	log_file(nob::log_path, std::ios::trunc | std::ios::out)
{
}

}
