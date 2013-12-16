#pragma once

#include "Macros.h"
#include <optix_world.h>
#include <glm/glm.hpp>

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
	void setCamera(const glm::vec3 &eye, const glm::vec3 &dir, const glm::vec3 &up, const glm::vec3 &right);

private:
	void createSceneGraph();
	optix::Context ctx;
	unsigned int width, height; 
};

