#include "GraphicsSettings.h"
#include "macros.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <optix_world.h>

using namespace std;

unsigned int GraphicsSettings::maxBufferWidth;
unsigned int GraphicsSettings::maxBufferHeight;
unsigned int GraphicsSettings::bufferWidth;
unsigned int GraphicsSettings::bufferHeight;
unsigned int GraphicsSettings::screenWidth;
unsigned int GraphicsSettings::screenHeight;

bool GraphicsSettings::useVBO;
bool GraphicsSettings::choosePTXversion;

unsigned int GraphicsSettings::bufferFormat;

map<string, unsigned int> GraphicsSettings::format;

void GraphicsSettings::readSettingsFromFile(const string &path)
{
	format["RT_FORMAT_UNSIGNED_BYTE4"] = RT_FORMAT_UNSIGNED_BYTE4;
	format["RT_FORMAT_FLOAT"] = RT_FORMAT_FLOAT;
	format["RT_FORMAT_FLOAT3"] = RT_FORMAT_FLOAT4;
	format["RT_FORMAT_FLOAT4"] = RT_FORMAT_FLOAT4;

	ifstream f(path);
	if(f.is_open())
	{
		try
		{
			while(!f.eof())
			{
				std::string name, value;
				f >> name;
				f >> value;
				if(name == "maxBufferWidth")
					maxBufferWidth = std::stoi(value);
				else if(name == "maxBufferHeight")
					maxBufferHeight = std::stoi(value);
				else if(name == "bufferWidth")
					bufferWidth = std::stoi(value);
				else if(name == "bufferHeight")
					bufferHeight = std::stoi(value);
				else if(name == "screenWidth")
					screenWidth = std::stoi(value);
				else if(name == "screenHeight")
					screenHeight = std::stoi(value);
				else if(name == "useVBO")
					useVBO = std::stoi(value);
				else if(name == "choosePTXversion")
					choosePTXversion = std::stoi(value);
				else if(name == "bufferFormat")
					bufferFormat = format[value];
			}
		}
		catch(std::exception *ex)
		{
			printf("%s\n", ex->what());
			exit(0);
		}
	}
	else
	{
		printf("Cannot open graphics settings");
		exit(0);
	}
	f.close();

	if(!useVBO)
	{
		screenHeight = bufferHeight;
		screenWidth = bufferWidth;
	}
}