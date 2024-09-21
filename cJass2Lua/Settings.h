#pragma once

#ifndef DEFINE_SETTINGS

#include <string>
#include "Logger.h"
#include "ConfigMgr.h"

namespace Settings
{
	extern bool								StrictMode;
	extern bool								IgnoreComments;
	extern bool								ConvertRawCodes;
	extern std::string						OutputNewLineType;
	extern std::string						OutputLanguage;
	extern _____LOGGER::Logger::Level		LogLevel;
}

#else

namespace Settings
{
	bool									StrictMode = false;
	bool									IgnoreComments = false;
	bool									ConvertRawCodes = false;
	std::string								OutputNewLineType = "CRLF";
	std::string								OutputLanguage = "Lua";
	_____LOGGER::Logger::Level				LogLevel = LOGLVL(Debug);

	void Load(CConfigMgr& cmgr)
	{
		Settings::StrictMode = cmgr.GetValue_Bool("Settings", "StrictMode", false);
		Settings::IgnoreComments = cmgr.GetValue_Bool("Settings", "IgnoreComments", false);
		Settings::ConvertRawCodes = cmgr.GetValue_Bool("Settings", "ConvertRawCodes", false);
		Settings::OutputNewLineType = cmgr.GetValue_Str("Settings", "OutputNewLineType", "CRLF");
		Settings::OutputLanguage = cmgr.GetValue_Str("Settings", "OutputLanguage", "Lua");
		Settings::LogLevel = static_cast<_____LOGGER::Logger::Level>(cmgr.GetValue_Num<int>("Settings", "LogLevel", 0));
		cmgr.Save();
		cmgr.SaveAs("config.ini");
	}

	void Reset(CConfigMgr& cmgr)
	{
		cmgr.SetValue_Bool("Settings", "StrictMode", false);
		cmgr.SetValue_Bool("Settings", "IgnoreComments", false);
		cmgr.SetValue_Bool("Settings", "ConvertRawCodes", false);
		cmgr.SetValue_Str("Settings", "OutputNewLineType", "CRLF");
		cmgr.SetValue_Str("Settings", "OutputLanguage", "Lua");
		cmgr.SetValue_Num<int>("Settings", "LogLevel", 0);
		cmgr.Save();
		cmgr.SaveAs("config.ini");
	}
}

#endif