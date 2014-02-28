#include "BufferDrawer.h"
#include "Engine.h"
#include "Environment.h"

using namespace optix;
using namespace glm;
using namespace reng;

BufferDrawer::BufferDrawer(void)
	: tex(TextureHandler::genTexture("buffer", GL_TEXTURE_2D)), SETTING(textureFilter),
	vertices(GL_ARRAY_BUFFER, GL_STATIC_DRAW), outBuffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW),
	vertexAttrib(0, 3, GL_FLOAT, GL_FALSE), p("../Shaders/passthrough")
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
	outBuffer.bind();
	outBuffer.setData(0, Environment::get().maxBufferWidth * Environment::get().maxBufferHeight * sizeof(float4));
	return outBuffer.getID();
}

void BufferDrawer::init(const Buffer &buffer)
{
	GLfloat quad[] = 
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

	vao.bind();
	vertices.setData(quad, sizeof(quad));
	vertexAttrib.attribPointer();

	p.use();
	glActiveTexture(GL_TEXTURE0);
	p.setUniform("renderedTexture", 0);

	
	outBuffer.bind();
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);*/
}

void BufferDrawer::draw(optix::Buffer &buffer) const
{
	try
	{
		//outBuffer.bind();
		tex.texImage(0, glTextureFormat, vec3(Environment::get().bufferWidth.x, Environment::get().bufferHeight.x, 0), glFormat, glDataType, 0);
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		/*glEnable(GL_TEXTURE_2D);
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
		glDisable(GL_TEXTURE_2D);*/
	}
	catch(Exception ex)
	{
		printf("%s\n", ex.what());
		exit(-1);
	}
}
