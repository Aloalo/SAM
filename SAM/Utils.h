#pragma once

#include "SafeOptix.h"
#include <string>
#include <glm/glm.hpp>
#include <assimp/mesh.h>

namespace trayc
{
	//host Utils
	class Utils
	{
	public:
		static float eps;
		static float pi;
		static std::string ptxPath;
		static std::string defTexture(const std::string &name);
		static std::string resource(const std::string &name);
		static std::string shader(const std::string &name);
		static std::string pathToPTX(const std::string &filename);

		static optix::float3 aiToOptix(const aiVector3D &vec);
		static optix::float3 glmToOptix(const glm::vec3 &vec);
		static bool equals(const optix::float3 &x, const optix::float3 &y);
		static bool equals(float x, float y);
		static void print(const optix::float3 &x);
	};
}