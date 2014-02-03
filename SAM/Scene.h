#pragma once

#include "Macros.h"
#include <optix_world.h>
#include "Camera.h"
#include <map>
#include "Labyrinth.h"
#include "Setting.h"

//handles optix traceing
class Scene
{
public:
	Scene(void);
	~Scene(void);

	void initialize(unsigned int GLBO);
	void createSceneGraph(const Labyrinth &lab);
	void trace();
	optix::Buffer getBuffer();
	void setBufferSize(int w, int h);
	void setCamera(const reng::Camera &cam);

private:
	void createMaterials();
	optix::Context ctx;
	std::map<int, optix::Material> materials;

	Setting<int> maxRayDepth;
	Setting<int> castsShadows;
	Setting<int> useSchlick;
	Setting<int> useInternalReflections;
};

