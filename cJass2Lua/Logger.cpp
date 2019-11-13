#define _LOGGER_MAIN_CPP

#include "Logger.h"
#include <time.h>
#include <sstream>
#include <memory>

namespace _____LOGGER
{
	std::mutex Logger::Writer::_writeMutex;

	std::string _generateTimeStamp()
	{
		char date[64];
		time_t t = time(0);
		struct tm tm;

		gmtime_s(&tm, &t);
		strftime(date, sizeof(date), "[%d.%m.%Y|%H:%M:%S]", &tm);
		return date;
	}

	/****************
	 *	  LOGGER	*
	 ****************/

	Logger::Logger(OutputInterface::Type outType, std::string fileName) :
		OutputInterface
		(	  outType
			, NewLineType::CRLF
			, fileName
		)
		, _level(Level::Info)
		, _file("None")
		, _line(0)
		, _func("?")
		, _buffer("")
		, _logFileName(fileName)
		, _doNotAppendSpaces(false)
	{
	}

	Logger::~Logger()
	{
	}

	std::string Logger::_levelToString()
	{
		switch (_level)
		{
		case Level::Debug:
			return "Debug";

		case Level::Info:
			return "Info";

		case Level::Warning:
			return "Warning";

		case Level::Critical:
			return "Critical";

		case Level::Fatal:
			return "Fatal";

		default:
			return "Unknown";
		}
	}

	void Logger::_toOutput(const std::string& str)
	{
		_buffer += str;
		if (!_doNotAppendSpaces)
			_buffer += " ";
	}

	void Logger::_write()
	{
		if (_buffer != "")
		{
			std::ostringstream ss;
			ss << _generateTimeStamp() << "[" << _file << ":" << _line << " (" << _func << ")] [" << _levelToString() << "]:\t" << _buffer << _nl;
			OutputInterface::_toOutput(ss.str());
			_buffer = "";
		}
	}

	/****************
	 *	  WRITER	*
	 ****************/

	Logger::Writer::Writer(const std::string& file, const std::string& func, int line, Level level, Logger& logger) : _logger(&logger)
	{
		_writeMutex.lock();
		logger._file = file;
		logger._line = line;
		logger._func = func;
		logger._level = level;
	}

	Logger::Writer::~Writer()
	{
		_logger->_write();
		_writeMutex.unlock();
	}

	Logger::Writer& Logger::Writer::operator<<(const std::string& str)
	{
		_logger->_toOutput(str);
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(const NewLine&)
	{
		_logger->_toOutput(_logger->_nl);
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(const std::vector<std::string>& strings)
	{
		_logger->_doNotAppendSpaces = true;
		_logger->_toOutput("{ ");
		for (size_t i = 0; i < strings.size(); i++)
		{
			_logger->_toOutput("\"" + strings[i] + "\"");
			if (i != strings.size() - 1)
				_logger->_toOutput(", ");
		}
		_logger->_toOutput(" }");
		_logger->_doNotAppendSpaces = false;
		return *this;
	}
}