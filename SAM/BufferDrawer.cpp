#include "BufferDrawer.h"
#include "Engine.h"
#include "Environment.h"

using namespace optix;
using namespace glm;
using namespace reng;

BufferDrawer::BufferDrawer(void)
	: tex(TextureHandler::genTexture("buffer", GL_TEXTURE_2D)), vbo(GL_ARRAY_BUFFER, GL_STREAM_DRAW), SETTING(textureFilter)
{
	glDataType = GL_FLOAT;
	glFormat = GL_RGBA;
	glTextureFormat = GL_RGBA32F_ARB;
}


BufferDrawer::~BufferDrawer(void)
{
	vbo.destroy();
}

unsigned int BufferDrawer::createGLBuffer()
{
	vbo.bind();
	vbo.setData(0, Environment::get().maxBufferWidth * Environment::get().maxBufferHeight * sizeof(float4));
	return vbo.getID();
}

void BufferDrawer::init(const Buffer &buffer)
{
	tex.bind();
	tex.texParami(GL_TEXTURE_MAG_FILTER, textureFilter);
	tex.texParami(GL_TEXTURE_MIN_FILTER, textureFilter);
	tex.texParami(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	tex.texParami(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vbo.getID());
	RTsize elementSize = buffer->getElementSize();
	if(elementSize % 8 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
	else if(elementSize % 4 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	else if(elementSize % 2 == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	vbo.bind();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
}

void BufferDrawer::draw(optix::Buffer &buffer)
{
	try
	{
		tex.texImage(0, glTextureFormat, vec3(Environment::get().bufferWidth.x, Environment::get().bufferHeight.x, 0), glFormat, glDataType, 0);
		glEnable(GL_TEXTURE_2D);
		{
			float u = 0.5f / (float)Environment::get().screenWidth.x;
			float v = 0.5f / (float)Environment::get().screenHeight.x;

			glBegin(GL_QUADS);
			{
				glTexCoord2f(u, v);
				glVertex2f(0.0f, 0.0f);
				glTexCoord2f(1.0f, v);
				glVertex2f(1.0f, 0.0f);
				glTexCoord2f(1.0f - u, 1.0f - v);
				glVertex2f(1.0f, 1.0f);
				glTexCoord2f(u, 1.0f - v);
				glVertex2f(0.0f, 1.0f); 
			}
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}
	catch(optix::Exception ex)
	{
		printf("%s\n", ex.what());
		exit(-1);
	}
}
