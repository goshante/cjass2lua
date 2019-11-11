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
	ReadFile(hFile, &buf[0], size, &dw, NULL);
	CloseHandle(hFile);
	return buf;
}

int main()
{
	std::string text = FileToString("D:\\test\\test2.j");
	std::string outputFilePath   =  "D:\\test\\output.lua";
	HANDLE hFile = CreateFileA(outputFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	cJass::Parser2 parser(text, cJass::OutputInterface::Type::File, cJass::OutputInterface::NewLineType::LF, hFile);

	try
	{
		parser.Parse();
		parser.ToLua();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}
	CloseHandle(hFile);
	system("pause");
	return 0;
}