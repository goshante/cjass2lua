#pragma once

#include "OutputInterface.h"
#include <iostream>

OutputInterface::NewLine OutputInterface::nl;

OutputInterface::OutputInterface()
	: _type(Type::None)
	, _file(nullptr)
	, _strPtr(nullptr)
	, _nl("\r\n")
{
}

void ofstreamDeleter(std::ofstream* p) 
{
	p->close();
	delete p;
}

OutputInterface::OutputInterface(Type type, NewLineType nlType, std::string& fileNameOrString)
	: _type(type)
	, _file(nullptr)
	, _strPtr(nullptr)
{
	switch (type)
	{
	case Type::String:
		_strPtr = &fileNameOrString;
		break;
	case Type::File:
		_file = std::shared_ptr<std::ofstream>(new std::ofstream, ofstreamDeleter);
		_file->open(fileNameOrString, std::ios::out | std::ios_base::trunc | std::ios::binary);
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
	, _file(_file)
	, _strPtr(copy._strPtr)
	, _nl(copy._nl)
{
}

OutputInterface& OutputInterface::operator=(const OutputInterface& copy)
{
	_type = copy._type;
	_file = copy._file;
	_strPtr = copy._strPtr;
	return *this;
}

void OutputInterface::_toOutput(const std::string& str)
{
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
		(*_file) << str;
		_file->flush();
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