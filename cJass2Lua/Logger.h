#pragma once

#include "OutputInterface.h"
#include <vector>
#include <mutex>
#include <type_traits>

namespace _____LOGGER
{
	const char __logFileName[] = "cjass2lua.log";

	class Logger : public OutputInterface
	{
	public:
		enum class Level
		{
			Clear,
			Debug,
			Info,
			Warning,
			Critical,
			Fatal
		};

		class Writer
		{
		private:
			Logger*							 _logger;
			static std::recursive_mutex		_writeMutex;

			Writer(const Writer&)			 = delete;
			Writer& operator=(const Writer&) = delete;

		public:
			Writer(std::string file, const std::string& func, int line, Level level, Logger& logger);
			~Writer();

			Writer& operator<<(const std::string& str);
			template<typename T, typename = std::enable_if_t<std::is_fundamental_v<T>>>
			inline Writer& operator<<(T n)
			{
				if (std::is_same<T, char>::value)
					_logger->_toOutput(std::string({ char(n) }));
				else if (std::is_same<T, unsigned char>::value)
				{
					char buf[8] = { '\0' };
					sprintf_s(buf, sizeof(buf), "0x%X", uint8_t(n));
					std::string str(buf);
					_logger->_toOutput(str);
				}
				else
					_logger->_toOutput(std::to_string(n));
				return *this;
			}
			Writer& operator<<(const NewLine&);
			Writer& operator<<(const std::vector<std::string>& strings);
		};

		friend class Writer;

	protected:
		Level			_level;
		Level			_minLevel;
		std::string		_file;
		int				_line;
		std::string		_func;
		std::string		_buffer;
		std::string		_logFileName;
		bool			_doNotAppendSpaces;

		virtual void	_toOutput(const std::string& str) override;
		std::string		_levelToString();
		void			_write();

	private:
		Logger(const Logger&)			 = delete;
		Logger& operator=(const Logger&) = delete;

	public:
		Logger(OutputInterface::Type outType, std::string fileName);
		~Logger();

		void level(Level lvl);
	};

#ifdef _LOGGER_MAIN_CPP
#ifndef _DEBUG
	Logger ___Logger(OutputInterface::Type::File, __logFileName);
#else
	Logger ___Logger(OutputInterface::Type::Console, __logFileName);
#endif
#else
	extern Logger ___Logger;
#endif
}



#define LOGLVL(lvl)			_____LOGGER::Logger::Level::lvl
#define APP_LOG_LEVEL(lvl) _____LOGGER::___Logger.level(lvl)
#define appLog(lvl) _____LOGGER::Logger::Writer(__FILE__, __FUNCTION__, __LINE__, _____LOGGER::Logger::Level::lvl, _____LOGGER::___Logger)