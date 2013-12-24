#include "GraphicsSettings.h"
#include "macros.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <optix_world.h>
#include <GL/glew.h>

using namespace std;


GraphicsSettings::GraphicsSettings(void)
{
}

GraphicsSettings::GraphicsSettings(const string &path)
{
	format["RT_FORMAT_UNSIGNED_BYTE4"] = RT_FORMAT_UNSIGNED_BYTE4;
	format["RT_FORMAT_FLOAT"] = RT_FORMAT_FLOAT;
	format["RT_FORMAT_FLOAT3"] = RT_FORMAT_FLOAT4;
	format["RT_FORMAT_FLOAT4"] = RT_FORMAT_FLOAT4;
	
	format["GL_NEAREST"] = GL_NEAREST;
	format["GL_LINEAR"] = GL_LINEAR;

	ifstream f(path);
	if(f.is_open())
	{
		try
		{
			while(!f.eof())
			{
				string name, value;
				f >> name;
				if(name.substr(0, 1) == "#")
				{
					f >> value;
					name = name.substr(1);
					try
					{
						settings[name] = stoi(value);
					}
					catch(exception ex)
					{
						settings[name] = format[value];
					}
				}
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
}


unsigned int& GraphicsSettings::operator[](const string &variableName)
{
	return settings[variableName];
}