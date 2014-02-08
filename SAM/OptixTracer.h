#pragma once

#include "Macros.h"
#include <optix_world.h>
#include <map>
#include <assimp/scene.h>
#include <Camera.h>
#include "Labyrinth.h"
#include "Setting.h"
#include "lights.h"

//handles optix traceing
class OptixTracer
{
public:
	OptixTracer(void);
	~OptixTracer(void);

	void initialize(unsigned int GLBO);

	void createSceneGraph(const Labyrinth &lab);
	void createSceneGraph(const aiScene *scene);

	void trace();
	optix::Buffer getBuffer();
	void setBufferSize(int w, int h);
	void setCamera(const reng::Camera &cam);

	optix::Context ctx;
private:
	template<class T>
	optix::Buffer getBufferFromVector(const std::vector<T> &vec, RTformat type);
	optix::TextureSampler getTexture(const aiMaterial *mat, aiTextureType type);

	void createMaterials();
	std::map<int, optix::Material> materials;
	std::map<int, optix::TextureSampler> textures;

	Setting<int> maxRayDepth;
	Setting<int> castsShadows;
	Setting<int> useSchlick;
	Setting<int> useInternalReflections;
};

