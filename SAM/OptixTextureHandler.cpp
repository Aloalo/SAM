#include "OptixTextureHandler.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include "Environment.h"

using namespace std;
using namespace optix;

namespace trayc
{
	OptixTextureHandler::OptixTextureHandler()
	{
	}

    static inline ILboolean convertAndGetType(GLenum format, GLenum &type)
    {
        switch(format)
        {
        case GL_RGBA32F_ARB:
            type = GL_FLOAT;
            return ilConvertImage(IL_RGBA, IL_FLOAT);
            break;
        case GL_RGBA8:
            type = GL_UNSIGNED_BYTE;
            return ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
            break;
        case GL_RGBA8UI:
            type = GL_UNSIGNED_BYTE;
            return ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
            break;
        case GL_RGBA16:
            type = GL_UNSIGNED_SHORT;
            return ilConvertImage(IL_RGBA, IL_UNSIGNED_SHORT);
            break;
        case GL_R8:
            type = GL_UNSIGNED_BYTE;
            return ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
            break;
        case GL_R8UI:
            type = GL_UNSIGNED_BYTE;
            return ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
            break;
        case GL_R8I:
            type = GL_BYTE;
            return ilConvertImage(IL_LUMINANCE, IL_BYTE);
            break;
        default:
            throw new exception("Unsupported texture format");
        }
    }

    optix::TextureSampler OptixTextureHandler::get(const string &path, GLfloat anisotropy, RTwrapmode wrapMode, const string &def, GLenum format)
    {
        if(existingTextures.find(path) != existingTextures.end())
            return existingTextures[path];

        ILuint imageID;
        ILboolean success;
        ILenum error;
        ilGenImages(1, &imageID);
        ilBindImage(imageID);
        success = ilLoadImage((const ILstring)path.c_str());

        if(!success)
        {
            error = ilGetError();
            cerr << "Image load failed " + path + " - IL reports error: " << error << " - " << iluErrorString(error) << endl;
            success = ilLoadImage((const ILstring)def.c_str());
            if(!success)
            {
                error = ilGetError();
                cerr << "Image load failed " + def + " - IL reports error: " << error << " - " << iluErrorString(error) << endl;
                system("pause");
            }
        }

        ILinfo ImageInfo;
        iluGetImageInfo(&ImageInfo);
        if(ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
            iluFlipImage();

        GLenum type;
        success = convertAndGetType(format, type);

        if(!success)
        {
            error = ilGetError();
            cerr << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << endl;
            system("pause");
        }

        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), type, ilGetData());

        ilDeleteImages(1, &imageID);

        optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(texID, RT_TARGET_GL_TEXTURE_2D);
        sampler->setWrapMode(0, wrapMode);
        sampler->setWrapMode(1, wrapMode);
        sampler->setWrapMode(2, wrapMode);
        sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
        sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
        existingTextures[path] = sampler;

        return sampler;
    }
}
