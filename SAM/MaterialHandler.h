#pragma once

#include "OptixTextureHandler.h"
#include <assimp/material.h>
#include <Singleton.h>

namespace trayc
{
	class MaterialHandler : public Singleton<MaterialHandler>
	{
	public:
		optix::Material createMaterial(const std::string &path, const aiMaterial *mat);
		std::string getTextureName(const aiMaterial *mat, aiTextureType type, const std::string &path, const std::string &def = "error.png") const;

	private:
		MaterialHandler(void);
		friend Singleton<MaterialHandler>;
	};
}