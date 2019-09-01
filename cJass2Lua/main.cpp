#include <iostream>
#include <Windows.h>
#include "cJassParser.h"

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
	cJass::Parser parser(text);

	parser.Parse();

	system("pause");
	return 0;
}