#pragma once

#include "OutputInterface.h"
#include <iostream>
#include <exception>

OutputInterface::NewLine OutputInterface::nl;

OutputInterface::OutputInterface()
	: _type(Type::None)
	, _file(nullptr)
	, _strPtr(nullptr)
	, _nl("\r\n")
{
}

OutputInterface::OutputInterface(Type type, NewLineType nlType, std::string& fileNameOrString)
	: _type(type)
	, _file(nullptr)
	, _strPtr(nullptr)
{
	HANDLE hFile = NULL;
	switch (type)
	{
	case Type::String:
		_strPtr = &fileNameOrString;
		break;

	case Type::FileAndConsole:
	case Type::File:
		hFile = CreateFileA(fileNameOrString.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
		if (hFile == 0 || hFile == HANDLE(~0))
			throw std::runtime_error("OutputInterface::OutputInterface: Cannot open file " + fileNameOrString + " for writing.");
		_file = std::shared_ptr<WinHandle>(new WinHandle(hFile));
		break;
	}

	switch (nlType)
	{
	case NewLineType::CR:
		_nl = "\r";
		break;
	case NewLineType::LF:
		_nl = "\n";
		break;

	case NewLineType::CRLF:
		_nl = "\r\n";
		break;
	}
}

OutputInterface::OutputInterface(Type type, NewLineType nlType)
	: _type(type)
	, _file(nullptr)
	, _strPtr(nullptr)
{
	switch (nlType)
	{
	case NewLineType::CR:
		_nl = "\r";
		break;
	case NewLineType::LF:
		_nl = "\n";
		break;

	case NewLineType::CRLF:
		_nl = "\r\n";
		break;
	}
}

void OutputInterface::_toOutput(const std::string& str)
{
	DWORD dw = 0;

	switch (_type)
	{
	case Type::None:
		break;

	case Type::Console:
		std::cout << str;
		break;

	case Type::String:
		(*_strPtr) += str;
		break;

	case Type::File:
		WriteFile(*_file, &str[0], DWORD(str.length()), &dw, NULL);
		break;

	case Type::FileAndConsole:
		std::cout << str;
		WriteFile(*_file, &str[0], DWORD(str.length()), &dw, NULL);
		break;
	}
}

OutputInterface& OutputInterface::operator<<(const std::string& str)
{
	_toOutput(str);
	return *this;
}

OutputInterface& OutputInterface::operator<<(const NewLine&)
{
	_toOutput(_nl);
	return *this;
}

bool OutputInterface::IsReady() const
{
	return (_type != Type::None);
}

void OutputInterface::SetOutputFile(const std::string& fname)
{
	Close();
	HANDLE hFile = CreateFileA(fname.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if (hFile == 0 || hFile == HANDLE(~0))
		throw std::runtime_error("OutputInterface::OutputInterface: Cannot open file " + fname + " for writing.");
	_file = std::shared_ptr<WinHandle>(new WinHandle(hFile));
}

void OutputInterface::Close()
{
	_file.reset();
}