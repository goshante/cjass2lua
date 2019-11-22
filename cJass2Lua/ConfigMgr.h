
#pragma once

#include <string>
#include <sstream>
#include <map>

#include "OutputInterface.h"


class CConfigMgr : public OutputInterface
{
private:
	struct ConfigVar
	{
		std::string value;
		std::string comment;
	};

public:

	class SectionIterator;
	class KeyIterator;
	CConfigMgr(const std::string& filename = "");
	~CConfigMgr();
	void Free();
	bool Load(const std::string& filename);
	bool Save();
	bool SaveAs(const std::string& filename);
	std::string GetValue(const std::string& section, const std::string& key, const std::string& defaultValue);

	bool GetValue_Bool(const std::string& section, const std::string& key, const bool& defaultValue);
	std::string GetValue_Str(const std::string& section, const std::string& key, const std::string& defaultValue);

	template <class T>
	T GetValue_Num(const std::string& section, const std::string& key, const T& defaultValue)
	{
		std::string def = ";";
		std::string value = GetValue(section, key, def);
		if (value == def)
			return defaultValue;

		T val;
		std::istringstream iss(value);
		iss >> val;

		return val;
	}

	void SetValue(const std::string& section, const std::string& key, const std::string& value);

	void SetValue_Str(const std::string& section, const std::string& key, std::string value);
	void SetValue_Bool(const std::string& section, const std::string& key, bool value);

	template <class T>
	void SetValue_Num(const std::string& section, const std::string& key, const T& value)
	{
		std::ostringstream oss;
		std::string str;

		oss << value;
		str = oss.str();

		SetValue(section, key, str);
	}

	std::string GetComment(const std::string& section, const std::string& key);
	void SetComment(const std::string& section, const std::string& key, const std::string& comment);
	void DeleteKey(const std::string& section, const std::string& key);
	SectionIterator beginSection();
	SectionIterator endSection();
	KeyIterator beginKey(const std::string& section);
	KeyIterator endKey(const std::string& section);

private:
	std::map<std::string, std::map<std::string, CConfigMgr::ConfigVar> > m_IniMap;
	std::map<std::string, std::map<std::string, std::string> > m_DescriptionMap;
	std::string m_FileName;
	void SaveDescription(std::string section, std::string key, std::ofstream &file);
	void ParasitCar(std::string& str);
	std::string Trim(const std::string& str);
};


class CConfigMgr::SectionIterator
{
public:

	SectionIterator();
	SectionIterator(std::map<std::string, std::map<std::string, CConfigMgr::ConfigVar> >::iterator mapIterator);
	const std::string& operator*();
	SectionIterator operator++();
	bool operator==(SectionIterator const& a);
	bool operator!=(SectionIterator const& a);

private:
	std::map<std::string, std::map<std::string, CConfigMgr::ConfigVar> >::iterator m_mapIterator;
};


class CConfigMgr::KeyIterator
{
public:
	KeyIterator();
	KeyIterator(std::map<std::string, CConfigMgr::ConfigVar>::iterator mapIterator);
	const std::string& operator*();
	KeyIterator operator++();
	bool operator==(KeyIterator const& a);
	bool operator!=(KeyIterator const& a);

private:
	std::map<std::string, CConfigMgr::ConfigVar>::iterator m_mapIterator;
};