#pragma once

#include <string>
#include <memory>

#include <Windows.h>

class OutputInterface
{
public:
	class WinHandle
	{
	private:
		HANDLE _handle;
		WinHandle(const WinHandle&) = delete;

	public:
		WinHandle(HANDLE handle) : _handle(handle) {}
		~WinHandle() { CloseHandle(_handle); }
		operator HANDLE() { return _handle;  }
	};

	enum class Type
	{
		None,
		File,
		Console,
		FileAndConsole,
		String
	};

	enum class NewLineType
	{
		CR,
		LF,
		CRLF
	};

	enum class FileMode
	{
		CreateAlways,
		CreateIfNotExist,
		OpenExisting
	};

	class NewLine {};
	static NewLine nl;

protected:
	Type							_type;
	std::shared_ptr<WinHandle>		_file;
	std::string*					_strPtr;
	std::string						_nl;
	FileMode						_mode;
	bool							_directWrite;

	virtual void _toOutput(const std::string& str);

	OutputInterface(const OutputInterface& copy) = delete;
	OutputInterface& operator=(const OutputInterface& copy) = delete;

public:

	OutputInterface();
	OutputInterface(Type type, NewLineType nlType, std::string& fileNameOrString, FileMode mode = FileMode::CreateAlways);
	OutputInterface(Type type, NewLineType nlType, FileMode mode = FileMode::CreateAlways);
	virtual ~OutputInterface(){}

	bool IsReady() const;
	void Close();
	void SetOutputFile(const std::string& fname);
	void SetDirectWrite(bool enable);
	std::string genNl() const;

	OutputInterface& operator<<(const std::string& str);
	OutputInterface& operator<<(const NewLine&);
};