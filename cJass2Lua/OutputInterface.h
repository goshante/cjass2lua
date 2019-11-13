#pragma once

#include <string>
#include <fstream>
#include <memory>

class OutputInterface
{
public:
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
	std::shared_ptr<std::ofstream>	_file;
	std::string*					_strPtr;
	std::string						_nl;

	virtual void _toOutput(const std::string& str);

public:

	OutputInterface();
	OutputInterface(Type type, NewLineType nlType, std::string& fileNameOrString);
	OutputInterface(const OutputInterface& copy);
	virtual ~OutputInterface(){}

	bool IsReady() const;

	OutputInterface& operator=(const OutputInterface& copy);
	OutputInterface& operator<<(const std::string& str);
	OutputInterface& operator<<(const NewLine&);
};