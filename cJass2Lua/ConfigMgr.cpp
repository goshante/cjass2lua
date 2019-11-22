#include <iostream>
#include <fstream>
#include "ConfigMgr.h"

using namespace std;


CConfigMgr::CConfigMgr(const string& filename) : OutputInterface(Type::File, NewLineType::CRLF, FileMode::CreateIfNotExist)
{
	if (filename != "")
	{
		if (!Load(filename)) throw string("Unable to open the file " + filename + " in read mode.");
	}
}

CConfigMgr::~CConfigMgr()
{
	Free();
}

bool CConfigMgr::Load(const string& filename)
{
	size_t pos;
	size_t pos2;
	size_t length;
	string line;
	string section;
	string key;
	string comment;
	ifstream file;
	ConfigVar iniLine;


	Free();
	m_FileName = filename;

	file.open(m_FileName.c_str(), ifstream::in);
	if (!file) return false;

	while (getline(file, line))
	{
		ParasitCar(line);
		if (line.empty()) continue;
		length = line.length();

		key = "";
		iniLine.value = "";
		iniLine.comment = "";

		if (line.at(0) == '[')
		{
			pos = line.find_first_of(']');
			if (pos == string::npos) pos = line.length();
			section = Trim(line.substr(1, pos - 1));
			if (comment != "")
			{
				m_DescriptionMap[section][""] = comment;
				comment = "";
			}
			continue;
		}

		pos = line.find_first_of(';');
		pos2 = line.find_first_of('#');
		if ((pos != string::npos) && (pos2 != string::npos) && (pos > pos2)) pos = pos2;
		if ((pos == string::npos) && (pos2 != string::npos)) pos = pos2;
		if (pos != string::npos)
		{
			if (pos > 0)
			{
				iniLine.comment = line.substr(pos + 1, length - pos);
				line.erase(pos, length - pos);
			}
			else
			{
				if (comment != "") comment += '\n';
				comment += line.substr(pos + 1, length - pos);
				continue;
			}
		}

		pos = line.find_first_of('=');
		if (pos != string::npos)
		{
			iniLine.value = Trim(line.substr(pos + 1, length - pos));
			line.erase(pos, length - pos);
		}

		key = Trim(line);
		m_IniMap[section][key] = iniLine;
		if (comment != "")
		{
			m_DescriptionMap[section][key] = comment;
			comment = "";
		}

	}

	file.close();
	return true;
}

bool CConfigMgr::Save()
{
	return SaveAs(m_FileName);
}

bool CConfigMgr::SaveAs(const string& filename)
{
	std::map<std::string, std::map<std::string, CConfigMgr::ConfigVar> >::iterator itSection;
	std::map<std::string, CConfigMgr::ConfigVar>::iterator itKey;
	ConfigVar iniLine;
	ofstream file;
	bool first = true;

	file.open(filename.c_str());
	if (!file) return false;

	for (itSection = m_IniMap.begin(); itSection != m_IniMap.end(); ++itSection)
	{
		if (!first) file << endl;
		SaveDescription(itSection->first, "", file);
		if (itSection->first != "") file << "[" << itSection->first << "]" << endl;

		for (itKey = itSection->second.begin(); itKey != itSection->second.end(); ++itKey)
		{
			SaveDescription(itSection->first, itKey->first, file);
			iniLine = itKey->second;
			if (itKey->first != "") file << itKey->first << "=" << iniLine.value;
			if (iniLine.comment != "")
			{
				if (itKey->first != "")
					file << "\t;";
				else
					file << "#";
				file << iniLine.comment;
			}
			file << endl;
		}
		first = false;
	}

	file.flush();
	file.close();

	return true;
}

void CConfigMgr::SaveDescription(string section, string key, ofstream &file)
{
	stringstream ss(m_DescriptionMap[section][key]);
	string item;
	while (std::getline(ss, item, '\n'))
	{
		file << "#" << item << endl;
	}
}

void CConfigMgr::Free()
{
	m_IniMap.clear();
}

bool CConfigMgr::GetValue_Bool(const std::string& section, const std::string& key, const bool& defaultValue)
{
	std::string def = ";";
	std::string value = GetValue(section, key, def);
	if (value == def)
		return defaultValue;

	if (value == "TRUE" || value == "true")
		return true;
	else
		return false;
}

std::string CConfigMgr::GetValue_Str(const std::string& section, const std::string& key, const std::string& defaultValue)
{
	std::string def = ";";
	std::string value = GetValue(section, key, def);
	if (value == def)
		return defaultValue;

	size_t len = value.length();

	if (value[0] != '\"')
		return defaultValue;

	if (value[len - 1] != '\"')
		return defaultValue;

	value = value.substr(1, len - 2);

	return value;
}

string CConfigMgr::GetValue(const string& section, const string& key, const string& defaultValue)
{
	map<string, map<string, CConfigMgr::ConfigVar> >::iterator itSection = m_IniMap.find(section);
	if (itSection == m_IniMap.end()) return defaultValue;

	map<string, CConfigMgr::ConfigVar>::iterator itKey = itSection->second.find(key);
	if (itKey == itSection->second.end()) return defaultValue;

	return itKey->second.value;
}

void CConfigMgr::SetValue(const string& section, const string& key, const string& value)
{
	ConfigVar iniLine;

	iniLine = m_IniMap[section][key];
	iniLine.value = value;
	m_IniMap[section][key] = iniLine;
}

void CConfigMgr::SetValue_Str(const std::string& section, const std::string& key, std::string value)
{
	std::string str;
	str = "\"" + value + "\"";
	SetValue(section, key, str);
}

void CConfigMgr::SetValue_Bool(const std::string& section, const std::string& key, bool value)
{
	std::string str;

	if (value == true)
		str = "TRUE";
	else
		str = "FALSE";

	SetValue(section, key, str);
}

string CConfigMgr::GetComment(const string& section, const string& key)
{
	map<string, map<string, CConfigMgr::ConfigVar> >::iterator itSection = m_IniMap.find(section);
	if (itSection == m_IniMap.end()) return "";

	map<string, CConfigMgr::ConfigVar>::iterator itKey = itSection->second.find(key);
	if (itKey == itSection->second.end()) return "";

	return itKey->second.comment;
}

void CConfigMgr::SetComment(const string& section, const string& key, const string& comment)
{
	ConfigVar iniLine;

	iniLine = m_IniMap[section][key];
	iniLine.comment = comment;
	m_IniMap[section][key] = iniLine;
}

void CConfigMgr::DeleteKey(const string& section, const string& key)
{
	m_IniMap[section].erase(key);
}

CConfigMgr::SectionIterator CConfigMgr::beginSection()
{
	return SectionIterator(m_IniMap.begin());
}

CConfigMgr::SectionIterator CConfigMgr::endSection()
{
	return SectionIterator(m_IniMap.end());
}

CConfigMgr::KeyIterator CConfigMgr::beginKey(const std::string& section)
{
	map<string, map<string, CConfigMgr::ConfigVar> >::iterator itSection = m_IniMap.find(section);
	if (itSection == m_IniMap.end())
	{
		itSection = m_IniMap.begin();
		return KeyIterator(itSection->second.end());
	}

	return KeyIterator(itSection->second.begin());
}

CConfigMgr::KeyIterator CConfigMgr::endKey(const std::string& section)
{
	map<string, map<string, CConfigMgr::ConfigVar> >::iterator itSection = m_IniMap.find(section);
	if (itSection == m_IniMap.end()) itSection = m_IniMap.begin();

	return KeyIterator(itSection->second.end());
}

void CConfigMgr::ParasitCar(string& str)
{
	size_t fin = str.size();

	if (fin < 1) return;

	if (str.at(fin - 1) < ' ') str.erase(fin - 1);
}

string CConfigMgr::Trim(const string& str)
{
	size_t deb = 0;
	size_t fin = str.size();
	char   chr;

	while (deb < fin)
	{
		chr = str.at(deb);
		if ((chr != ' ') && (chr != '\t')) break;
		deb++;
	}

	while (fin > 0)
	{
		chr = str.at(fin - 1);
		if ((chr != ' ') && (chr != '\t')) break;
		fin--;
	}

	return str.substr(deb, fin - deb);
}




CConfigMgr::SectionIterator::SectionIterator()
{
}

CConfigMgr::SectionIterator::SectionIterator(std::map<std::string, std::map<std::string, CConfigMgr::ConfigVar> >::iterator mapIterator)
{
	m_mapIterator = mapIterator;
}

const std::string& CConfigMgr::SectionIterator::operator*()
{
	return m_mapIterator->first;
}

CConfigMgr::SectionIterator CConfigMgr::SectionIterator::operator++()
{
	++m_mapIterator;
	return *this;
}

bool CConfigMgr::SectionIterator::operator==(SectionIterator const& a)
{
	return a.m_mapIterator == m_mapIterator;
}

bool CConfigMgr::SectionIterator::operator!=(SectionIterator const& a)
{
	return a.m_mapIterator != m_mapIterator;
}




CConfigMgr::KeyIterator::KeyIterator()
{
}

CConfigMgr::KeyIterator::KeyIterator(std::map<std::string, CConfigMgr::ConfigVar>::iterator mapIterator)
{
	m_mapIterator = mapIterator;
}

const std::string& CConfigMgr::KeyIterator::operator*()
{
	return m_mapIterator->first;
}

CConfigMgr::KeyIterator CConfigMgr::KeyIterator::operator++()
{
	++m_mapIterator;
	return *this;
}

bool CConfigMgr::KeyIterator::operator==(KeyIterator const& a)
{
	return a.m_mapIterator == m_mapIterator;
}

bool CConfigMgr::KeyIterator::operator!=(KeyIterator const& a)
{
	return a.m_mapIterator != m_mapIterator;
}
