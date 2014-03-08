#pragma once

#include <GL/glew.h>
#include "SafeOptix.h"
#include <map>
#include "Utils.h"

namespace trayc
{
	class OptixTextureHandler
	{
	public:
		OptixTextureHandler(optix::Context &ctx);
		~OptixTextureHandler(void);

		optix::TextureSampler get(const std::string &path, const std::string &def = Utils::defTexture("error.png"), 
			float anisotropy = 0.0f, RTwrapmode wrapMode = RT_WRAP_REPEAT, GLenum format = GL_RGBA8);

	private:
		std::map<unsigned int,optix::TextureSampler> mp;
		optix::Context &ctx;
	};
}