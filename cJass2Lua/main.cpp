#include <iostream>
#include <Windows.h>
#include "cJassParser2.h"

void RemoveBOMFromString(std::string& str)
{
	if (str.length() < 3)
		return;

	if (str[0] == 0xEF && str[1] == 0xBB && str[2] == 0xBF)
		str = str.substr(3, str.length() - 1);
}

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
	RemoveBOMFromString(buf);
	return buf;
}

//For testing purposes only
void StringToFile(std::string path, std::string str)
{
	HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dw = 0;
	WriteFile(hFile, &str[0], DWORD(str.length()), &dw, NULL);
	CloseHandle(hFile);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//APP_LOG_LEVEL(Info);
	appLog(Info) << "Starting cJass2Lua application";

	std::string	inputFile		 =	"D:\\test\\test3.j";
	std::string text			 =  FileToString(inputFile);
	std::string outputFilePath   =  "D:\\test\\output2.lua";
	cJass::Parser2 parser(OutputInterface::Type::File, OutputInterface::NewLineType::LF, outputFilePath);

	try
	{
		parser.Parse(text, inputFile);
		parser.ToLua();
	}
	catch (const std::exception& ex)
	{
		appLog(Fatal) << ex.what();
	}

	appLog(Info) << "Exiting";
	return 0;
}