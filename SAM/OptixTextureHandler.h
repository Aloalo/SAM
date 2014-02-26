#pragma once

#include "Macros.h"
#include <map>
#include <optix_world.h>

class OptixTextureHandler
{
public:
	OptixTextureHandler(optix::Context &ctx);
	~OptixTextureHandler(void);
	
	optix::TextureSampler get(const std::string &path, const std::string &def = "../Resources/error.png");

private:

	std::map<unsigned int,optix::TextureSampler> mp;
	optix::Context &ctx;
};
