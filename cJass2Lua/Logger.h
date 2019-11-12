#pragma once

#include "OutputInterface.h"
#include <vector>
#include <mutex>

namespace _____LOGGER
{
	class Logger : public OutputInterface
	{
	public:
		enum class Level
		{
			Debug,
			Info,
			Warning,
			Critical,
			Fatal
		};

		class Writer
		{
		private:
			Logger&							 _logger;

			Writer(const Writer&)			 = delete;
			Writer& operator=(const Writer&) = delete;

		public:
			Writer(Logger& logger);
			~Writer();

			Writer& operator<<(const std::string& str);
			Writer& operator<<(signed long n);
			Writer& operator<<(unsigned long n);
			Writer& operator<<(signed long long n);
			Writer& operator<<(unsigned long long n);
			Writer& operator<<(char c);
			Writer& operator<<(BYTE b);
			Writer& operator<<(float f);
			Writer& operator<<(double d);
			Writer& operator<<(const NewLine&);
			Writer& operator<<(const std::vector<std::string> strings);
		};

		friend class Writer;

	protected:
		Level			_level;
		std::string		_file;
		int				_line;
		std::string		_func;
		std::string		_buffer;
		std::string		_logFileName;
		std::mutex		_writeMutex;

		virtual void	_toOutput(const std::string& str) override;
		std::string		_levelToString();
		void			_write();

	private:
		Logger(const Logger&)			 = delete;
		Logger& operator=(const Logger&) = delete;

	public:
		Logger(const std::string fileName);
		~Logger();

		void prepare(const std::string& file, const std::string& func, int line, Level level);
	};
}

#ifdef _LOGGER_MAIN_CPP
_____LOGGER::Logger ___Logger("cjass2lua.log");
#else
extern _____LOGGER::Logger ___Logger;
#endif

#define appLog(lvl) ___Logger.prepare(__FILE__, __FUNCTION__, __LINE__, _____LOGGER::Logger::Level::lvl); _____LOGGER::Logger::Writer(___Logger)