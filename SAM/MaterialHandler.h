#pragma once

#include "OptixTextureHandler.h"
#include <assimp/material.h>

class MaterialHandler
{
public:
	MaterialHandler(optix::Context &ctx);
	~MaterialHandler(void);

	enum LabMaterials
	{
		MIRROR = 0,
		GLASS = 1,
		WALL = 2
	};

	optix::Material createMaterial(const std::string &path, const aiMaterial *mat);
	optix::Material& getLabyrinthMaterial(int mat);
	void createLabMaterials();
	
	std::string getTextureName(const aiMaterial *mat, aiTextureType type, const std::string &path, const std::string &def = "error.png") const;

	OptixTextureHandler texHandler;
private:
	std::map<int, optix::Material> labmat;

	optix::Context &ctx;
};
