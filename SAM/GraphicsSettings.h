#pragma once

#include <string>
#include <map>

struct GraphicsSettings
{
	GraphicsSettings(void);
	GraphicsSettings(const std::string &path);

	unsigned int& operator[](const std::string &variableName);
private:
	std::map<std::string, unsigned int> settings;
	std::map<std::string, unsigned int> format;
};

