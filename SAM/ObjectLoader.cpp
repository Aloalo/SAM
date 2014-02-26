#include "ObjectLoader.h"

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <TextureHandler.h>

#include "Utils.h"

using namespace std;
using namespace reng;

ObjectLoader::ObjectLoader(void) :
	model(NULL)
{
}


ObjectLoader::~ObjectLoader(void)
{
}

void ObjectLoader::load(const string &path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(utils::resPath + path, aiProcessPreset_TargetRealtime_MaxQuality);

	if(!scene)
	{
		printf("%s\n", importer.GetErrorString());
		throw 1.21;
	}

	load(path, scene);
}

Texture ObjectLoader::loadTexture(const std::string &path)
{/*
 if(textures.find(ret.getID()) == textures.end())
 {
 optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(ret.getID(), RT_TARGET_GL_TEXTURE_2D);
 sampler->setWrapMode(0, RT_WRAP_REPEAT);
 sampler->setWrapMode(1, RT_WRAP_REPEAT);
 sampler->setWrapMode(2, RT_WRAP_REPEAT);
 sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
 sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
 sampler->setMaxAnisotropy(1.0f);
 sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
 textures[ret.getID()] = sampler;
 }
 */
	return TextureHandler::getTexture(path);
}

void ObjectLoader::load(const string &path, const aiScene *scene)
{
	model = scene;

	for(int i = 0; i < model->mNumMeshes; ++i)
	{
		const aiMesh *mesh = model->mMeshes[i];
		const aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

		if(mat->GetTextureCount(aiTextureType_AMBIENT) > 0)
		{
			aiString name;
			if(mat->GetTexture(aiTextureType_AMBIENT, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
				loadTexture(path + name.C_Str());
		}
		else
		{
			TextureHandler::genTexture("default", GL_TEXTURE_2D);
		}
	}
}
