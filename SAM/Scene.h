#pragma once

#include "Macros.h"
#include <optix_world.h>
#include "Camera.h"
#include <map>
#include "Labyrinth.h"

//handles optix traceing
class Scene
{
public:
	Scene(int width, int height);
	~Scene(void);

	void initialize();
	void createSceneGraph(const Labyrinth &lab);
	void trace();
	optix::Buffer getBuffer();
	void setBufferSize(int w, int h);
	void setCamera(const Camera &cam);

private:
	void createMaterials();
	optix::Context ctx;
	unsigned int width, height;
	std::map<int, optix::Material> materials;
};

