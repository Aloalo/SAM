#include "BufferDrawer.h"
#include "Engine.h"

using namespace optix;
using namespace glm;

BufferDrawer::BufferDrawer(void)
{
}


BufferDrawer::~BufferDrawer(void)
{
}

void BufferDrawer::init(const optix::Buffer &buffer)
{
	RTformat buffer_format = buffer->getFormat();
	switch (buffer_format)
	{
	case RT_FORMAT_UNSIGNED_BYTE4:
		glDataType = GL_UNSIGNED_BYTE;
		glFormat = GL_BGRA;
		break;
	case RT_FORMAT_FLOAT:
		glDataType = GL_FLOAT;
		glFormat = GL_LUMINANCE;
		break;
	case RT_FORMAT_FLOAT3:
		glDataType = GL_FLOAT;
		glFormat = GL_RGB;
		break;
	case RT_FORMAT_FLOAT4:
		glDataType = GL_FLOAT;
		glFormat = GL_RGBA;
		break;
	default:
		fprintf(stderr, "Unrecognized buffer data type or format.\n");
		exit(-1);
		break;
	}
}

void BufferDrawer::draw(optix::Buffer &buffer)
{
	try
	{
		RTsize w, h;
		buffer->getSize(w, h);
		GLvoid *data = buffer->map();
		glDrawPixels(w, h, glFormat, glDataType, data);
		buffer->unmap();
		printf("%d %d\n", w, h);
	}
	catch(optix::Exception ex)
	{
		printf("%s\n", ex.what());
		exit(-1);
	}
}
