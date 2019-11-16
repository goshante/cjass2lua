#pragma once

#ifndef DEFINE_SETTINGS

#include <string>
#include "Logger.h"
#include "ConfigMgr.h"

namespace Settings
{
	extern CConfigMgr						config;
	extern bool								StrictMode;
	extern bool								IgnoreComments;
	extern bool								ConvertRawCodes;
	extern bool								ClearLogsOnNewTranslate;
	extern bool								DoNotIncrementArrayIndexConstants;
	extern OutputInterface::NewLineType		OutputNewLineType;
	extern std::string						OutputLanguage;
	extern _____LOGGER::Logger::Level		LogLevel;
	extern std::string						lastInputPath;
	extern std::string						lastOutputPath;
}

#else

namespace Settings
{
	CConfigMgr								config;
	bool									StrictMode = false;
	bool									IgnoreComments = false;
	bool									ConvertRawCodes = false;
	bool									ClearLogsOnNewTranslate = false;
	extern bool								DoNotIncrementArrayIndexConstants = false;
	OutputInterface::NewLineType			OutputNewLineType = OutputInterface::NewLineType::CRLF;
	std::string								OutputLanguage = "Lua";
	_____LOGGER::Logger::Level				LogLevel = LOGLVL(Debug);
	std::string								lastInputPath;
	std::string								lastOutputPath;

	void Load()
	{
		Settings::StrictMode = config.GetValue_Bool("Settings", "StrictMode", false);
		Settings::IgnoreComments = config.GetValue_Bool("Settings", "IgnoreComments", false);
		Settings::ConvertRawCodes = config.GetValue_Bool("Settings", "ConvertRawCodes", false);
		Settings::ClearLogsOnNewTranslate = config.GetValue_Bool("Settings", "ClearLogsOnNewTranslate", false);
		Settings::DoNotIncrementArrayIndexConstants = config.GetValue_Bool("Settings", "DoNotIncrementArrayIndexConstants", false);
		std::string nlType = config.GetValue_Str("Settings", "OutputNewLineType", "CRLF");
		Settings::OutputLanguage = config.GetValue_Str("Settings", "OutputLanguage", "Lua");
		Settings::LogLevel = static_cast<_____LOGGER::Logger::Level>(config.GetValue_Num<int>("Settings", "LogLevel", 0));
		Settings::lastInputPath = config.GetValue_Str("Pathes", "Input", "");
		Settings::lastOutputPath = config.GetValue_Str("Pathes", "Output", "");

		if (nlType == "CR")
			Settings::OutputNewLineType = OutputInterface::NewLineType::CR;
		else if (nlType == "LF")
			Settings::OutputNewLineType = OutputInterface::NewLineType::LF;
		else if (nlType == "CRLF")
			Settings::OutputNewLineType = OutputInterface::NewLineType::CRLF;

		config.Save();
		config.SaveAs("config.ini");
	}

	void Save()
	{
		config.SetValue_Bool("Settings", "StrictMode", Settings::StrictMode);
		config.SetValue_Bool("Settings", "IgnoreComments", Settings::IgnoreComments);
		config.SetValue_Bool("Settings", "ConvertRawCodes", Settings::ConvertRawCodes);
		config.SetValue_Bool("Settings", "ClearLogsOnNewTranslate", Settings::ClearLogsOnNewTranslate);
		config.SetValue_Bool("Settings", "DoNotIncrementArrayIndexConstants", Settings::DoNotIncrementArrayIndexConstants);
		config.SetValue_Str("Settings", "OutputLanguage", Settings::OutputLanguage);
		config.SetValue_Num<int>("Settings", "LogLevel", static_cast<int>(Settings::LogLevel));
		config.SetValue_Str("Pathes", "Input", Settings::lastInputPath);
		config.SetValue_Str("Pathes", "Output", Settings::lastOutputPath);
		config.Save();
		config.SaveAs("config.ini");
	}

	void Reset()
	{
		config.SetValue_Bool("Settings", "StrictMode", false);
		config.SetValue_Bool("Settings", "IgnoreComments", false);
		config.SetValue_Bool("Settings", "ConvertRawCodes", false);
		config.SetValue_Bool("Settings", "ClearLogsOnNewTranslate", false);
		config.SetValue_Bool("Settings", "DoNotIncrementArrayIndexConstants", false);
		config.SetValue_Str("Settings", "OutputNewLineType", "CRLF");
		config.SetValue_Str("Settings", "OutputLanguage", "Lua");
		config.SetValue_Num<int>("Settings", "LogLevel", 0);
		config.SetValue_Str("Pathes", "Input", "");
		config.SetValue_Str("Pathes", "Output", "");
		config.Save();
		config.SaveAs("config.ini");
	}
}

#endif