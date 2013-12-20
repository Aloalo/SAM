#pragma once

#include "Texture2D.h"
#include "BufferObject.h"
#include "Macros.h"
#include <optix_world.h>

/*
draws the optix::Buffer on the whole screen
void init(const optix::Buffer &buffer) must be called each time buffer type changes
*/
class BufferDrawer
{
public:
	BufferDrawer(void);
	~BufferDrawer(void);
	
	unsigned int createGLBuffer(int width, int height);
	void init(const optix::Buffer &buffer);
	void draw(optix::Buffer &buffer);

private:
	GLenum glDataType;
	GLenum glFormat;
	GLenum glTextureFormat;
	Texture2D tex;
	BufferObject vbo;
};
