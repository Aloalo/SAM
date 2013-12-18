#pragma once

#include "Macros.h"
#include <optix_world.h>
#include "Camera.h"
#include <map>

//handles optix traceing
class Scene
{
public:
	Scene(int width, int height);
	~Scene(void);

	void initialize();
	void trace();
	optix::Buffer getBuffer();
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void setBufferSize(int w, int h);
	void setCamera(const Camera &cam);

private:
	void createSceneGraph();
	void createMaterials();
	optix::Context ctx;
	unsigned int width, height; 
	std::map<int, optix::Material> materials;
};

