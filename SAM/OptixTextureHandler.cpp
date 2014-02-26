#include "OptixTextureHandler.h"

#include <TextureHandler.h>

using namespace std;
using namespace optix;
using namespace reng;

OptixTextureHandler::OptixTextureHandler(optix::Context &ctx) :
	ctx(ctx)
{
}


OptixTextureHandler::~OptixTextureHandler(void)
{
}

optix::TextureSampler OptixTextureHandler::get(const string &path, const string &def)
{
	Texture ret = TextureHandler::getTexture(path, def);

	if(mp.find(ret.getID()) == mp.end())
	{
		optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(ret.getID(), RT_TARGET_GL_TEXTURE_2D);
		sampler->setWrapMode(0, RT_WRAP_REPEAT);
		sampler->setWrapMode(1, RT_WRAP_REPEAT);
		sampler->setWrapMode(2, RT_WRAP_REPEAT);
		sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
		sampler->setMaxAnisotropy(1.0f);
		sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
		mp[ret.getID()] = sampler;
	}

	return mp[ret.getID()];
}
