#include <iostream>
#include <fstream>

#include <Windows.h>
#include <process.h>
#include <filesystem>
#include <thread>
#include <shlobj_core.h>

#include "cJassParser2.h"
#include "ConfigMgr.h"
#include "Utils.h"
#include "reutils.h"

#define DEFINE_SETTINGS
#include "Settings.h"

namespace fs = std::filesystem;

enum
{
	IDC_INPUT = 101,
	IDC_OUTPUT,
	IDC_STATUS,
	IDC_BUTTON
};


struct ParseArgs
{
	HWND			hwnd;
	HWND			button;
	HWND			status;
	std::string		in;
	std::string		out;
};

cJass::Parser2*	   _parser = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void parserThread(ParseArgs args)
{
	try
	{
		std::string status = "Status: Parsing file ";
		std::string status2 = "Status: Generating code for file ";
		std::string fname;

		if (dirExists(args.in))
		{
			if (fileExists(args.out))
				throw std::runtime_error("Output path when input is directory must be directory.");
			else if (!dirExists(args.out))
				SHCreateDirectoryEx(NULL, args.out.c_str(), NULL);

			for (auto& entry : fs::directory_iterator(args.in))
			{
				status = "Status: Parsing file ";
				status2 = "Status: Generating code for file ";
				fname = entry.path().u8string();

				if (dirExists(fname))
					continue;

				if (!strEndsWith(fname, ".txt")
					&& !strEndsWith(fname, ".j")
					&& !strEndsWith(fname, ".jass")
					&& !strEndsWith(fname, ".cjass")
					&& !strEndsWith(fname, ".cj")
					&& !strEndsWith(fname, ".jj")
					&& !strEndsWith(fname, ".w3j")
					&& !strEndsWith(fname, ".w3cj"))
					continue;

				status += reu::IndexSubstr(fname, fname.find_last_of("\\/") + 1, fname.length() - 1) + " ...";
				status2 += reu::IndexSubstr(fname, fname.find_last_of("\\/") + 1, fname.length() - 1) + " ...";
				SetWindowTextA(args.status, status.c_str());
				UpdateWindow(args.hwnd);
					_parser->Parse(fname);
				SetWindowTextA(args.status, status2.c_str());
				UpdateWindow(args.hwnd);
					_parser->ToLua(args.out);
			}

			SetWindowTextA(args.status, "Status: Done!");
			EnableWindow(args.button, true);
			UpdateWindow(args.hwnd);
		}
		else
		{
			if (fileExists(args.in))
			{
				if (args.in.find("\\") != std::string::npos || args.in.find("/") != std::string::npos)
				{
					status += reu::IndexSubstr(args.in, args.in.find_last_of("\\/") + 1, args.in.length() - 1) + " ...";
					status2 += reu::IndexSubstr(args.in, args.in.find_last_of("\\/") + 1, args.in.length() - 1) + " ...";
				}
				else
				{
					status += args.in + " ...";
					status2 += args.in + " ...";
				}

				SetWindowTextA(args.status, status.c_str());
				UpdateWindow(args.hwnd);
				_parser->Parse(args.in);
				SetWindowTextA(args.status, status2.c_str());
				UpdateWindow(args.hwnd);
				_parser->ToLua(args.out);
				SetWindowTextA(args.status, "Status: Done!");
				EnableWindow(args.button, true);
				UpdateWindow(args.hwnd);
			}
			else
				throw std::runtime_error("Input file does not exists.");
		}
	}
	catch (const std::exception& ex)
	{
		appLog(Fatal) << ex.what();
		SetWindowTextA(args.status, (std::string("Status: Error - ") + ex.what()).c_str());
		EnableWindow(args.button, true);
		UpdateWindow(args.hwnd);
	}
}

void parse(ParseArgs args)
{
	std::thread(parserThread, args).detach();
}

void parse_thisThread(const std::string& in, const std::string& out)
{
	try
	{
		std::string status = "Status: Parsing file ";
		std::string status2 = "Status: Generating code for file ";
		std::string fname;

		if (dirExists(in))
		{
			if (fileExists(out))
				throw std::runtime_error("Output path when input is directory must be directory.");
			else if (!dirExists(out))
				SHCreateDirectoryEx(NULL, out.c_str(), NULL);

			for (auto& entry : fs::directory_iterator(in))
			{
				status = "Status: Parsing file ";
				status2 = "Status: Generating code for file ";
				fname = entry.path().u8string();

				if (dirExists(fname))
					continue;

				if (!strEndsWith(fname, ".txt")
					&& !strEndsWith(fname, ".j")
					&& !strEndsWith(fname, ".jass")
					&& !strEndsWith(fname, ".cjass")
					&& !strEndsWith(fname, ".cj")
					&& !strEndsWith(fname, ".jj")
					&& !strEndsWith(fname, ".w3j")
					&& !strEndsWith(fname, ".w3cj"))
					continue;

				status += reu::IndexSubstr(fname, fname.find_last_of("\\/") + 1, fname.length() - 1) + " ...";
				status2 += reu::IndexSubstr(fname, fname.find_last_of("\\/") + 1, fname.length() - 1) + " ...";

				std::cout << "Parsing file " << fname << " ..." << std::endl;
				_parser->Parse(fname);
				std::cout << "Writing code... " << std::endl;
				_parser->ToLua(out);
			}

			std::cout << "Done!" << std::endl;
		}
		else
		{
			if (fileExists(in))
			{
				std::cout << "Parsing file " << in << " ..." << std::endl;
				_parser->Parse(in);
				std::cout << "Writing code... " << std::endl;
				_parser->ToLua(out);

				std::cout << "Done!" << std::endl;
			}
			else
				throw std::runtime_error("Input file does not exists.");
		}
	}
	catch (const std::exception& ex)
	{
		appLog(Fatal) << ex.what();
		std::cout << "Error: " << ex.what() << std::endl;
	}
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	try
	{
		const char CLASS_NAME[] = "MainWn";
		WNDCLASS wc = {};
		wc.lpfnWndProc = &WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hIcon = NULL;

		int width = 436;
		int height = 250;
		int xPos = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int yPos = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;


		RegisterClassA(&wc);
							
		HWND hwnd = CreateWindowExA(
			NULL,
			CLASS_NAME,
			(std::string("cJass2Lua v") + APP_VER + " " + APP_BUILD).c_str(),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN,
			xPos, yPos, width, height,
			NULL,   
			NULL,
			hInstance,
			NULL
		);

		if (!hwnd)
			throw (std::runtime_error("Failed to create window."));

		//Begin constructing window
		int ox = 0, oy = 0;
		CreateWindowElement(hwnd, ET_STATIC, TEXT("Input file or directory"), hInstance, WS_VISIBLE, NULL, NULL, 10 + ox, 4 + oy, 132, 20, false);
		oy += 17;
		CreateWindowElement(hwnd, ET_EDIT, TEXT(""), hInstance, WS_VISIBLE | WS_BORDER | WS_TABSTOP, NULL, HMENU(IDC_INPUT), 10 + ox, 4 + oy, 400, 23, false);
		oy += 27;
		CreateWindowElement(hwnd, ET_STATIC, TEXT("Output file or directory"), hInstance, WS_VISIBLE, NULL, NULL, 10 + ox, 4 + oy, 132, 20, false);
		oy += 17;
		CreateWindowElement(hwnd, ET_EDIT, TEXT(""), hInstance, WS_VISIBLE | WS_BORDER | WS_TABSTOP, NULL, HMENU(IDC_OUTPUT), 10 + ox, 4 + oy, 400, 23, false);
		oy += 27;
		CreateWindowElement(hwnd, ET_STATIC, TEXT("Status: Ready"), hInstance, WS_VISIBLE, NULL, HMENU(IDC_STATUS), 10 + ox, 4 + oy, 400, 45, false);
		oy += 79;
		CreateWindowElement(hwnd, ET_BUTTON, TEXT("Translate"), hInstance, WS_VISIBLE | WS_TABSTOP | BS_FLAT, NULL, HMENU(IDC_BUTTON), 10 + ox, 4 + oy, 400, 30, false);

		//End constructing window

		ShowWindow(hwnd, SW_SHOW);

		MSG msg = { 0 };
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (const std::exception& ex)
	{
		MessageBoxA(NULL, ex.what(), "Fatal error", MB_OK | MB_ICONERROR);
		appLog(Fatal) << ex.what();
		appLog(Fatal) << "Exiting with fatal error.";

		return -1;
	}

	appLog(Info) << "Exiting normally";
	return 0;
}

int main()
{
	try
	{
		appLog(Info) << "Starting cJass2Lua application (version" << APP_VER << "/" << APP_BUILD << ")";
		std::cout << std::string("cJass2Lua v") + APP_VER + " " + APP_BUILD << std::endl;
		CConfigMgr	config;
		LPSTR* szArgList;
		int argCount;
		szArgList = CommandLineToArgvA(GetCommandLine(), &argCount);

		if (argCount <= 1)
			::ShowWindow(::GetConsoleWindow(), SW_HIDE);

		if (!fileExists("config.ini"))
			Settings::Reset(config);
		config.Load("config.ini");
		Settings::Load(config);

		APP_LOG_LEVEL(Settings::LogLevel);
		if (Settings::OutputLanguage != "Lua")
			throw std::runtime_error("Error! Unsupported output language '" + Settings::OutputLanguage + "' in config file.");

		appLog(Info) << "Config loaded";

		_parser = new cJass::Parser2;

		if (argCount > 1)
		{
			std::string out = "";
			if (argCount > 2)
				out = szArgList[2];
			std::cout << "Starting parser..." << std::endl;
			parse_thisThread(szArgList[1], out);
			return 0;
		}
		else
		{
			auto res = WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWNORMAL);
			if (_parser)
				delete _parser;
			return res;
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
		appLog(Fatal) << ex.what();
		appLog(Fatal) << "Exiting with fatal error.";

		if (_parser)
			delete _parser;

		return -1;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ParseArgs args;
	HWND btn, input, status, output;
	input = GetDlgItem(hwnd, IDC_INPUT);
	output = GetDlgItem(hwnd, IDC_OUTPUT);
	status = GetDlgItem(hwnd, IDC_STATUS);
	btn = GetDlgItem(hwnd, IDC_BUTTON);
	auto wp = LOWORD(wParam);

	char buf[MAX_PATH];
	GetWindowText(input, buf, sizeof(buf));

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_COMMAND:
		switch (wp)
		{
		case IDC_BUTTON:
			GetWindowText(input, buf, sizeof(buf));
			args.in = buf;
			GetWindowText(output, buf, sizeof(buf));
			args.out = buf;
			args.button = btn;
			args.status = status;
			args.hwnd = hwnd;
			EnableWindow(btn, false);
			SetWindowTextA(status, "Status: Starting parser...");
			UpdateWindow(hwnd);
			parse(args);
			break;
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}