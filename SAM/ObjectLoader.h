#pragma once

#include <assimp/scene.h>
#include <string>
#include <Texture.h>

class ObjectLoader
{
public:
	ObjectLoader(void);
	~ObjectLoader(void);
	
	void load(const std::string &path);
	void load(const std::string &path, const aiScene *scene);
	reng::Texture loadTexture(const std::string &path);

private:
	const aiScene *model;
};

