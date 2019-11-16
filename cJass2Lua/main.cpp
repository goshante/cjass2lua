#include <iostream>
#include <fstream>

#include <Windows.h>
#include <process.h>

#include "cJassParser2.h"
#include "ConfigMgr.h"
#include "Utils.h"

#define DEFINE_SETTINGS
#include "Settings.h"

unsigned int __stdcall parserThread(LPVOID* lpArgs)
{
	cJass::Parser2* parser = reinterpret_cast<cJass::Parser2*>(lpArgs);
	return 0;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	appLog(Info) << "Starting cJass2Lua application (version" << APP_VER << "/" << APP_BUILD << ")";
	CConfigMgr	config;

	if (!fileExists("config.ini"))
		Settings::Reset(config);
	config.Load("config.ini");
	Settings::Load(config);

	std::string	inputFile		 =	"D:\\test\\test3.j";
	std::string outputFilePath   =  "D:\\test\\output2.lua";
	std::string	inputFile2 = "D:\\test\\test2.j";
	std::string outputFilePath2 = "D:\\test\\output.lua";
	OutputInterface::NewLineType nlType = OutputInterface::NewLineType::LF;

	APP_LOG_LEVEL(Settings::LogLevel);

	try
	{
		if (Settings::OutputLanguage != "Lua")
			throw std::runtime_error("Error! Unsupported output language '" + Settings::OutputLanguage + "' in config file.");

		appLog(Info) << "Config loaded";
		cJass::Parser2 parser;
		_beginthreadex(NULL, 0, _beginthreadex_proc_type(&parserThread), &parser, 0, NULL);
		parser.Parse(inputFile, outputFilePath);
		//parser.Parse(inputFile2, outputFilePath2);
		parser.ToLua();
	}
	catch (const std::exception& ex)
	{
		appLog(Fatal) << ex.what();
		appLog(Info) << "Exiting with error";
		return -1;
	}

	appLog(Info) << "Exiting normally";
	return 0;
}