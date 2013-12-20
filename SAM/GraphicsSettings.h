#pragma once

#include <string>
#include <map>

struct GraphicsSettings
{
	GraphicsSettings(void);

	static unsigned int maxBufferWidth;
	static unsigned int maxBufferHeight;
	static unsigned int bufferWidth;
	static unsigned int bufferHeight;
	static unsigned int screenWidth;
	static unsigned int screenHeight;

	static bool useVBO;
	static bool choosePTXversion;

	static unsigned int bufferFormat;

	//static bool castShadows;

	static void readSettingsFromFile(const std::string &path);
private:
	static std::map<std::string, unsigned int> format;
};

