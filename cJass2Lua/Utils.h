#pragma once

#include <string>


void RemoveBOMFromString(std::string& str);
bool fileExists(const char *fileName);
std::string FileToString(std::string path);
void StringToFile(std::string path, std::string str);