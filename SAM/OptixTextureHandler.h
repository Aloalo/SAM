#pragma once

#include "SafeOptix.h"
#include <map>
#include "Utils.h"

class OptixTextureHandler
{
public:
	OptixTextureHandler(optix::Context &ctx);
	~OptixTextureHandler(void);
	
	optix::TextureSampler get(const std::string &path, const std::string &def = utils::defTexture("error.png"));

private:

	std::map<unsigned int,optix::TextureSampler> mp;
	optix::Context &ctx;
};
