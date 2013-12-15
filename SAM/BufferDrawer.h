#pragma once

#include <GL/glew.h>
#include "Macros.h"
#include <optix_world.h>

/*
draws the optix::Buffer buffer on the whole screen
void init(const optix::Buffer &buffer) must be called each time buffer type changes
*/
class BufferDrawer
{
public:
	BufferDrawer(void);
	~BufferDrawer(void);
	
	void init(const optix::Buffer &buffer);
	void draw(optix::Buffer &buffer);

private:
	GLenum glDataType;
	GLenum glFormat;
};
