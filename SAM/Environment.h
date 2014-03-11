#pragma once

#include <Setting.h>
#include "MaterialHandler.h"
#include <Singleton.h>

namespace trayc
{
	class Environment : public Singleton<Environment>
	{
	public:
		reng::Setting<int> screenWidth;
		reng::Setting<int> screenHeight;
		reng::Setting<int> bufferWidth;
		reng::Setting<int> bufferHeight;

		optix::Context ctx;

	private:
		Environment(void);
		friend Singleton<Environment>;
	};

	extern optix::Context &ctx;
}