#pragma once

#include "Setting.h"

class Environment
{
public:
	static Environment& get();
	
	Setting<int> screenWidth;
	Setting<int> screenHeight;
	Setting<int> bufferWidth;
	Setting<int> bufferHeight;

private:
	Environment();

	static Environment *instance;
};
