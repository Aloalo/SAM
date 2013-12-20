#include "BufferDrawer.h"
#include "Engine.h"

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
	Buffer buffer;
	vbo.bind();
	vbo.setData(0, sizeof(uchar4) * 1920 * 1080);
	return vbo.getID();
}

void BufferDrawer::init(const optix::Buffer &buffer)
{
	RTformat buffer_format = buffer->getFormat();
	switch (buffer_format)
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
	tex.bind();
	tex.texParami(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	tex.texParami(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	tex.texParami(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	tex.texParami(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unsigned int PBOid = buffer->getGLBOId();

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOid);
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
		RTsize w, h;
		buffer->getSize(w, h);

		tex.texImage(0, glTextureFormat, vec3(w, h, 0), glFormat, glDataType, 0);
		glEnable(GL_TEXTURE_2D);
		{
			float u = 0.5f / (float)w;
			float v = 0.5f / (float)h;

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

		/*GLvoid *data = buffer->map();
		glDrawPixels(w, h, glFormat, glDataType, data);
		buffer->unmap();*/
	}
	catch(optix::Exception ex)
	{
		printf("%s\n", ex.what());
		exit(-1);
	}
}
