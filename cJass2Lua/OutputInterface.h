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
		String
	};

	enum class NewLineType
	{
		CR,
		LF,
		CRLF
	};

	class NewLine {};
	static NewLine nl;

protected:
	Type							_type;
	std::shared_ptr<WinHandle>		_file;
	std::string*					_strPtr;
	std::string						_nl;

	virtual void _toOutput(const std::string& str);

	OutputInterface(const OutputInterface& copy) = delete;
	OutputInterface& operator=(const OutputInterface& copy) = delete;

public:

	OutputInterface();
	OutputInterface(Type type, NewLineType nlType, std::string& fileNameOrString);
	OutputInterface(Type type, NewLineType nlType);
	virtual ~OutputInterface(){}

	bool IsReady() const;
	void Close();
	void SetOutputFile(const std::string& fname);

	OutputInterface& operator<<(const std::string& str);
	OutputInterface& operator<<(const NewLine&);
};