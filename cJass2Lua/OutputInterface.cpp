#pragma once

#include "OutputInterface.h"
#include <iostream>

OutputInterface::NewLine OutputInterface::nl;

OutputInterface::OutputInterface()
	: _type(Type::None)
	, _hFile(NULL)
	, _strPtr(nullptr)
	, _nl("\r\n")
{
}

OutputInterface::OutputInterface(Type type, NewLineType nlType, void* ptr)
	: _type(type)
	, _hFile(NULL)
	, _strPtr(nullptr)
{
	switch (type)
	{
	case Type::String:
		_strPtr = static_cast<std::string*>(ptr);
		break;
	case Type::File:
		_hFile = static_cast<HANDLE>(ptr);
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

OutputInterface::OutputInterface(const OutputInterface& copy)
	: _type(copy._type)
	, _hFile(copy._hFile)
	, _strPtr(copy._strPtr)
	, _nl(copy._nl)
{
}

OutputInterface& OutputInterface::operator=(const OutputInterface& copy)
{
	_type = copy._type;
	_hFile = copy._hFile;
	_strPtr = copy._strPtr;
	return *this;
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
		WriteFile(_hFile, &str[0], static_cast<DWORD>(str.length()), &dw, NULL);
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