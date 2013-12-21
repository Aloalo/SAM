#include "BufferDrawer.h"
#include "Engine.h"
#include "Settings.h"

using namespace optix;
using namespace glm;

BufferDrawer::BufferDrawer(void)
	: tex(GL_TEXTURE_2D), vbo(GL_ARRAY_BUFFER, GL_STREAM_DRAW)
{
}


BufferDrawer::~BufferDrawer(void)
{
	tex.destroy();
	vbo.destroy();
}

unsigned int BufferDrawer::createGLBuffer(int width, int height)
{
	vbo.bind();
	vbo.setData(0, Settings::GS["maxBufferHeight"] * Settings::GS["maxBufferHeight"] * sizeof(float4));
	return vbo.getID();
}

void BufferDrawer::init(const Buffer &buffer)
{
	switch (Settings::GS["bufferFormat"])
	{
	case RT_FORMAT_UNSIGNED_BYTE4:
		glDataType = GL_UNSIGNED_BYTE;
		glFormat = GL_BGRA;
		glTextureFormat = GL_RGBA8;
		break;
	case RT_FORMAT_FLOAT:
		glDataType = GL_FLOAT;
		glFormat = GL_LUMINANCE;
		glTextureFormat = GL_LUMINANCE32F_ARB;
		break;
	case RT_FORMAT_FLOAT3:
		glDataType = GL_FLOAT;
		glFormat = GL_RGB;
		glTextureFormat = GL_RGB32F_ARB;
		break;
	case RT_FORMAT_FLOAT4:
		glDataType = GL_FLOAT;
		glFormat = GL_RGBA;
		glTextureFormat = GL_RGBA32F_ARB;
		break;
	default:
		fprintf(stderr, "Unrecognized buffer data type or format.\n");
		exit(-1);
		break;
	}
	if(Settings::GS["useVBO"])
	{
		tex.bind();
		tex.texParami(GL_TEXTURE_MAG_FILTER, Settings::GS["textureFilter"]);
		tex.texParami(GL_TEXTURE_MIN_FILTER, Settings::GS["textureFilter"]);
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
}

void BufferDrawer::draw(optix::Buffer &buffer)
{
	try
	{
		if(Settings::GS["useVBO"])
		{
			tex.texImage(0, glTextureFormat, vec3(Settings::GS["bufferWidth"], Settings::GS["bufferHeight"], 0), glFormat, glDataType, 0);
			glEnable(GL_TEXTURE_2D);
			{
				float u = 0.5f / (float)Settings::GS["screenWidth"];
				float v = 0.5f / (float)Settings::GS["screenHeight"];

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
		else
		{
			GLvoid *data = buffer->map();
			glDrawPixels(Settings::GS["bufferWidth"], Settings::GS["bufferHeight"], glFormat, glDataType, data);
			buffer->unmap();
		}
	}
	catch(optix::Exception ex)
	{
		printf("%s\n", ex.what());
		exit(-1);
	}
}
