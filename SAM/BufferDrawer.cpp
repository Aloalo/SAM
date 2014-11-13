#include "BufferDrawer.h"
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
        : SETTING(textureFilter), p(VertexShader(Utils::shader("passthrough").c_str()), FragmentShader(Utils::shader(postProcess ? "fxaa" : "passthrough").c_str())), SETTING(postProcess)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &verticesID);
        glGenTextures(1, &textureID);
        glGenBuffers(1, &outBufferID);

        glDataType = GL_UNSIGNED_BYTE;
        glFormat = GL_BGRA;
        glTextureFormat = GL_RGBA8;
    }


    BufferDrawer::~BufferDrawer(void)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &verticesID);
        glDeleteBuffers(1, &outBufferID);
        glDeleteTextures(1, &textureID);
    }

    unsigned int BufferDrawer::createGLBuffer()
    {
        allocateBuffer(Environment::get().bufferWidth, Environment::get().bufferHeight);
        return outBufferID;
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

        glBindTexture(GL_TEXTURE_2D, textureID);
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, outBufferID);
        {
            RTsize elementSize = buffer->getElementSize();
            if(elementSize % 8 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
            else if(elementSize % 4 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            else if(elementSize % 2 == 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
            else
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        p.use();
        p.setUniform("renderedTexture", 0);

        glBindVertexArray(VAO);
        {
            glBindBuffer(GL_ARRAY_BUFFER, verticesID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
        glBindVertexArray(0);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            cerr << "OpenGL error: 0x" << hex << err << endl;
            system("pause");
        }
    }

    void BufferDrawer::draw(optix::Buffer &buffer)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, outBufferID);

        glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, Environment::get().bufferWidth.x, Environment::get().bufferHeight.x, 0, glFormat, glDataType, nullptr);

        p.use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    void BufferDrawer::allocateBuffer(int width, int height)
    {
        glBindBuffer(GL_ARRAY_BUFFER, outBufferID);
        {
            glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(uchar4), nullptr, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            cerr << "OpenGL error: 0x" << hex << err << endl;
            system("pause");
        }
    }
}
