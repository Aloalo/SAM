#include "OptixTextureHandler.h"

#include <TextureHandler.h>
#include "Environment.h"

using namespace std;
using namespace optix;
using namespace reng;

namespace trayc
{
	OptixTextureHandler::OptixTextureHandler()
	{
	}

	optix::TextureSampler OptixTextureHandler::get(const string &path, const string &def, float anisotropy, RTwrapmode wrapMode, GLenum format)
	{
		Texture ret = TextureHandler::getTexture(path, def, format);
		
		if(mp.find(ret.getID()) == mp.end())
		{
			optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(ret.getID(), RT_TARGET_GL_TEXTURE_2D);
			sampler->setWrapMode(0, wrapMode);
			sampler->setWrapMode(1, wrapMode);
			sampler->setWrapMode(2, wrapMode);
			sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
			sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
			mp[ret.getID()] = sampler;
		}

		return mp[ret.getID()];
	}
}