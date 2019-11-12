#define _LOGGER_MAIN_CPP

#include "Logger.h"
#include <time.h>
#include <sstream>
#include <memory>

namespace _____LOGGER
{
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

	Logger::Logger(const std::string fileName) :
		OutputInterface
		(Type::File
			, NewLineType::CRLF
			, nullptr
		)
		, _level(Level::Info)
		, _file("None")
		, _line(0)
		, _func("?")
		, _buffer("")
		, _logFileName(fileName)
	{
		_hFile = CreateFileA(_logFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(_hFile);
	}

	Logger::~Logger()
	{
		if (_hFile != NULL)
			CloseHandle(_hFile);
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
		_buffer += str + " ";
	}

	void Logger::_write()
	{
		if (_buffer != "")
		{
			_writeMutex.lock();
			_hFile = CreateFileA(_logFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			std::ostringstream ss;
			ss << _generateTimeStamp() << "[" << _file << ":" << _line << " (" << _func << ")] [" << _levelToString() << "]:\t" << _buffer << _nl;
			OutputInterface::_toOutput(ss.str());
			_buffer = "";
			CloseHandle(_hFile);
			_hFile = NULL;
			_writeMutex.unlock();
		}
	}

	void Logger::prepare(const std::string& file, const std::string& func, int line, Level level)
	{
		_file  = file;
		_line  = line;
		_func  = func;
		_level = level;
	}

	/****************
	 *	  WRITER	*
	 ****************/

	Logger::Writer::Writer(Logger& logger) : _logger(logger)
	{
	}

	Logger::Writer::~Writer()
	{
		_logger._write();
	}

	Logger::Writer& Logger::Writer::operator<<(const std::string& str)
	{
		_logger._toOutput(str);
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(signed long n)
	{
		_logger._toOutput(std::to_string(n));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(unsigned long n)
	{
		_logger._toOutput(std::to_string(n));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(signed long long n)
	{
		_logger._toOutput(std::to_string(n));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(unsigned long long n)
	{
		_logger._toOutput(std::to_string(n));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(char c)
	{
		_logger._toOutput(std::string({ c }));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(BYTE b)
	{
		char buf[8] = { '\0' };
		sprintf_s(buf, sizeof(buf), "0x%X", b);
		std::string str(buf);
		_logger._toOutput(str);
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(float f)
	{
		_logger._toOutput(std::to_string(f));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(double d)
	{
		_logger._toOutput(std::to_string(d));
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(const NewLine&)
	{
		_logger._toOutput(_logger._nl);
		return *this;
	}

	Logger::Writer& Logger::Writer::operator<<(const std::vector<std::string> strings)
	{
		_logger._toOutput("{ ");
		for (size_t i = 0; i < strings.size(); i++)
		{
			_logger._toOutput("\"");
			_logger._toOutput(strings[i]);
			_logger._toOutput("\"");
			if (i != strings.size() - 1)
				_logger._toOutput(", ");
		}
		_logger._toOutput(" }");
		return *this;
	}
}