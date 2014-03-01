#pragma once

#include <OGLobjects.h>
#include "SafeOptix.h"
#include "Setting.h"

/*
draws the optix::Buffer on the whole screen
void init(const optix::Buffer &buffer) must be called each time buffer type changes
*/
class BufferDrawer
{
public:
	BufferDrawer(void);
	~BufferDrawer(void);
	
	unsigned int createGLBuffer();
	void init(const optix::Buffer &buffer);
	void draw(optix::Buffer &buffer) const;

private:
	GLenum glDataType;
	GLenum glFormat;
	GLenum glTextureFormat;

	reng::Texture tex;
	reng::BufferObject vertices;
	reng::BufferObject outBuffer;
	reng::VertexArrayObject vao;
	reng::VertexAttribArray vertexAttrib;
	reng::Program p;

	Setting<int> textureFilter;
	Setting<int> postProcess;
};
