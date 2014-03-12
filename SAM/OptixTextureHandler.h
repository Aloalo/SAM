#pragma once

#include <GL/glew.h>
#include "SafeOptix.h"
#include <map>
#include "Utils.h"
#include <Singleton.h>

namespace trayc
{
	class OptixTextureHandler : public Singleton<OptixTextureHandler>
	{
	public:
		using Singleton<OptixTextureHandler>::get;

		optix::TextureSampler get(const std::string &path, const std::string &def = Utils::defTexture("error.png"), 
			float anisotropy = 16.0f, RTwrapmode wrapMode = RT_WRAP_REPEAT, GLenum format = GL_RGBA8);

	private:
		OptixTextureHandler(void);
		friend Singleton<OptixTextureHandler>;
		std::map<unsigned int,optix::TextureSampler> mp;
	};
}