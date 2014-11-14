#pragma once

#include <Engine/GL/Program.h>
#include "SafeOptix.h"
#include <Engine/Utils/Setting.h>

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

		GLuint createGLBuffer();
		void init(const optix::Buffer &buffer);
		void draw(optix::Buffer &buffer);
		void allocateBuffer(int width, int height);

	private:
		GLenum glDataType;
		GLenum glFormat;
		GLenum glTextureFormat;

		engine::Setting<int> textureFilter;
		engine::Setting<int> postProcess;

        GLuint textureID;
        GLuint verticesID;
        GLuint outBufferID;
        GLuint VAO;

        engine::Program p;
	};
}
