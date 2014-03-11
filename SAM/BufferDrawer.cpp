#include "BufferDrawer.h"
#include "Engine.h"
#include "Environment.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace optix;
using namespace glm;
using namespace reng;

namespace trayc
{
	BufferDrawer::BufferDrawer(void)
		: tex(TextureHandler::genTexture("buffer", GL_TEXTURE_2D)), SETTING(textureFilter),
		vertices(GL_ARRAY_BUFFER, GL_STATIC_DRAW), outBuffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW),
		vertexAttrib(0, 3, GL_FLOAT, GL_FALSE), p(VertexShader(Utils::shader("passthrough").c_str()), FragmentShader(Utils::shader(postProcess ? "fxaa" : "passthrough").c_str())),
		SETTING(postProcess)
	{
		glDataType = GL_FLOAT;
		glFormat = GL_RGBA;
		glTextureFormat = GL_RGBA32F_ARB;
	}


	BufferDrawer::~BufferDrawer(void)
	{
		vao.destroy();
		vertices.destroy();
	}

	unsigned int BufferDrawer::createGLBuffer()
	{
		allocateBuffer(Environment::get().bufferWidth, Environment::get().bufferHeight);
		return outBuffer.getID();
	}

	void BufferDrawer::init(const Buffer &buffer)
	{
		const GLfloat quad[] = 
		{ 
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,
		};

		tex.bind();
		tex.texParami(GL_TEXTURE_MAG_FILTER, textureFilter);
		tex.texParami(GL_TEXTURE_MIN_FILTER, textureFilter);
		tex.texParami(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tex.texParami(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, outBuffer.getID());
		RTsize elementSize = buffer->getElementSize();
		if(elementSize % 8 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		else if(elementSize % 4 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		else if(elementSize % 2 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		p.use();
		glActiveTexture(GL_TEXTURE0);

		vao.bind();
		vertices.setData(quad, sizeof(quad));
		vertexAttrib.attribPointer();

		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
			cerr << "OpenGL error: 0x" << hex << err << endl;
	}

	void BufferDrawer::draw(optix::Buffer &buffer)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, outBuffer.getID());
		tex.texImage(0, glTextureFormat, vec3(Environment::get().bufferWidth.x, Environment::get().bufferHeight.x, 0), glFormat, glDataType, 0);
		p.use();
		vao.bind();
		vertices.bind();

			glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		vao.unBind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		tex.unBind();
	}

	
	void BufferDrawer::allocateBuffer(int width, int height)
	{
		outBuffer.bind();
		outBuffer.setData(0, width * height * sizeof(float4));
	}
}