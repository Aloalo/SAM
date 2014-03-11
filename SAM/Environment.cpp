#include "Environment.h"

namespace trayc
{
	Environment::Environment(void) :
		SETTING(screenHeight),
		SETTING(screenWidth),
		SETTING(bufferHeight),
		SETTING(bufferWidth)
	{
	}

	optix::Context &ctx = Environment::get().ctx;
}