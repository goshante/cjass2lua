#include <iostream>
#include <Windows.h>
#include "cJassParser2.h"

std::string FileToString(std::string path)
{
	HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	size_t size = GetFileSize(hFile, NULL);
	std::string buf;
	DWORD dw = 0;
	buf.resize(size);
	ReadFile(hFile, &buf[0], DWORD(size), &dw, NULL);
	CloseHandle(hFile);
	return buf;
}

int main()
{
	appLog(Debug) << "Starting cJass2Lua application";

	std::string	inputFile		 =	"D:\\test\\test2.j";
	std::string text			 =  FileToString(inputFile);
	std::string outputFilePath   =  "D:\\test\\output.lua";
	HANDLE hFile = CreateFileA(outputFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	cJass::Parser2 parser(OutputInterface::Type::File, OutputInterface::NewLineType::LF, hFile);

	try
	{
		parser.Parse(text, inputFile);
		parser.ToLua();
	}
	catch (const std::exception& ex)
	{
		appLog(Fatal) << ex.what();
	}

	CloseHandle(hFile);
	system("pause");
	return 0;
}