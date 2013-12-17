#pragma once

#include "Macros.h"
#include <optix_world.h>
#include "Camera.h"

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
	optix::Context ctx;
	unsigned int width, height; 
};

