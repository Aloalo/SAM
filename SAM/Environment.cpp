#include "Environment.h"

Environment *Environment::instance = 0;

Environment& Environment::get()
{
	if(instance)
		return *instance;
	return *(instance = new Environment());
}

Environment::Environment(void) :
	SETTING(screenHeight),
	SETTING(screenWidth),
	SETTING(bufferHeight),
	SETTING(bufferWidth)
{
}