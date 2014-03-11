#pragma once

#include <OGLobjects.h>
#include "SafeOptix.h"
#include <Setting.h>

namespace trayc
{
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
		void draw(optix::Buffer &buffer);

	private:
		GLenum glDataType;
		GLenum glFormat;
		GLenum glTextureFormat;

		reng::Setting<int> textureFilter;
		reng::Setting<int> postProcess;

		reng::Texture tex;
		reng::BufferObject vertices;
		reng::BufferObject outBuffer;
		reng::VertexArrayObject vao;
		reng::VertexAttribArray vertexAttrib;
		reng::Program p;
	};
}