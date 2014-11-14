#pragma once

#include <GL/glew.h>
#include "SafeOptix.h"
#include <map>
#include "Utils.h"
#include <Engine/Utils/Singleton.h>

namespace trayc
{
	class OptixTextureHandler : public Singleton<OptixTextureHandler>
	{
	public:
		using Singleton<OptixTextureHandler>::Get;

		optix::TextureSampler get(const std::string &path, GLfloat anisotropy = 0.0f, RTwrapmode wrapMode = RT_WRAP_REPEAT, 
			const std::string &def = Utils::defTexture("error.png"), GLenum format = GL_RGBA8);

	private:
		OptixTextureHandler(void);
		friend Singleton<OptixTextureHandler>;
        std::map<std::string, optix::TextureSampler> existingTextures;
	};
}
