#include "Utils.h"
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commctrl.h>
#include <richedit.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#include <regex>
#include "reutils.h"
#include "Settings.h"

#ifdef _WIN64
using flexInt = long long;
using flexUint = unsigned long long;
#else
using flexInt = long;
using flexUint = unsigned long;
#endif

namespace Utils
{
	void RemoveBOMFromString(std::string& str)
	{
		if (str.length() < 3)
			return;

		if (str[0] == 0xEF && str[1] == 0xBB && str[2] == 0xBF)
			str = str.substr(3, str.length() - 1);
	}

	bool fileExists(const std::string& str)
	{
		struct stat info;
		if (stat(str.c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFREG)
			return true;
		else
			return false;
	}

	bool dirExists(const std::string& str)
	{
		struct stat info;
		if (stat(str.c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}

	bool entryExists(const std::string& str)
	{
		struct stat info;
		if (stat(str.c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR || info.st_mode & S_IFREG)
			return true;
		else
			return false;
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
		buf.push_back('\0');
		RemoveBOMFromString(buf);
		return buf;
	}

	void StringToFile(std::string path, std::string str)
	{
		HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dw = 0;
		WriteFile(hFile, &str[0], DWORD(str.length()), &dw, NULL);
		CloseHandle(hFile);
	}

	HWND CreateWindowElement(HWND Parent, UINT Type, const char* Title, HINSTANCE hInst, DWORD Style, DWORD StyleEx, HMENU ElementID, INT pos_x, INT pos_y, INT Width, INT Height, BOOL NewRadioGroup)
	{
		HWND hWnd = NULL;
		const char* ClassName = nullptr;

		Style |= WS_CHILD;

		switch (Type)
		{
		case ET_STATIC:
			ClassName = "STATIC";
			break;

		case ET_BUTTON:
			ClassName = "BUTTON";
			break;

		case ET_EDIT:
			ClassName = "EDIT";
			break;

		case ET_COMBOBOX:
			ClassName = "COMBOBOX";
			break;

		case ET_LISTBOX:
			ClassName = "LISTBOX";
			break;

		case ET_MDICLIENT:
			ClassName = "MDICLIENT";
			break;

		case ET_SCROLLBAR:
			ClassName = "SCROLLBAR";
			break;

		case ET_CHECKBOX:
			ClassName = "BUTTON";
			Style |= BS_AUTOCHECKBOX;
			break;

		case ET_RADIOBUTTON:
			ClassName = "BUTTON";
			Style |= BS_AUTORADIOBUTTON;
			if (NewRadioGroup)
				Style |= WS_GROUP;
			break;

		case ET_GROUPBOX:
			ClassName = "BUTTON";
			Style |= BS_GROUPBOX;
			break;

		case ET_PROGRESS:
			ClassName = PROGRESS_CLASS;
			break;


		default: ClassName = "STATIC";
		}

		hWnd = CreateWindowEx(StyleEx, ClassName, Title, Style, pos_x, pos_y, Width, Height, Parent, ElementID, hInst, NULL);

		if (!hWnd)
			return nullptr;


		if (NewRadioGroup)
			CheckDlgButton(Parent, int((flexInt)ElementID), BST_CHECKED);

		HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

		return hWnd;
	}

	bool strEndsWith(const std::string& str, const std::string& suffix)
	{
		return std::regex_search(str,
			std::regex(std::string(suffix) + "$", std::regex_constants::icase));
	}

	char** CommandLineToArgvA(char* CmdLine, int* _argc)
	{
		char** argv;
		char*  _argv;
		ULONG   len;
		ULONG   argc;
		CHAR   a;
		ULONG   i, j;

		BOOLEAN  in_QM;
		BOOLEAN  in_TEXT;
		BOOLEAN  in_SPACE;

		len = ULONG(strlen(CmdLine));
		i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

		argv = (char**)GlobalAlloc(GMEM_FIXED,
			i + (len + 2) * sizeof(CHAR));

		_argv = (char*)(((PUCHAR)argv) + i);

		argc = 0;
		argv[argc] = _argv;
		in_QM = FALSE;
		in_TEXT = FALSE;
		in_SPACE = TRUE;
		i = 0;
		j = 0;

		while (a = CmdLine[i]) {
			if (in_QM) {
				if (a == '\"') {
					in_QM = FALSE;
				}
				else {
					_argv[j] = a;
					j++;
				}
			}
			else {
				switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
				}
			}
			i++;
		}
		_argv[j] = '\0';
		argv[argc] = NULL;

		(*_argc) = argc;
		return argv;
	}

	std::string op2lua(const std::string& op)
	{
		if (op == "&&")
			return " and ";
		else if (op == "||")
			return " or ";
		else if (op == "!")
			return " not ";
		else if (op == "!=")
			return " ~= ";
		else if (op == "++" || op == "--" || op == "+=" || op == "-=" || op == "*=" || op == "/=")
			return "";
		if (op == "dig_minus")
			return "-";

		return " " + op + " ";
	}

	int rawCodeToInt(std::string code)
	{
		auto match = reu::Search(code, "^'(.*)'$");
		if (match.IsMatching())
			code = match[1];

		int n = 0;
		int p = 0;
		for (auto it = code.rbegin(); it != code.rend(); it++)
		{
			char c = *it;
			n += c * int(pow(256, p));
			p++;
		}

		return n;
	}

	std::string const2lua(const std::string& cnst)
	{
		if (cnst[0] == '\'')
		{
			if (Settings::ConvertRawCodes)
				return std::to_string(rawCodeToInt(cnst));
			else
				return "FourCC(" + cnst + ")";
		}

		if (cnst == "null")
			return "nil";

		return cnst;
	}
}