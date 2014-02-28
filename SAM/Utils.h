#pragma once

#include "Macros.h"
#include <string>
#include <optix_math.h>

//host utils
namespace utils
{
	const float eps = 1e-3f;
	const float pi = 3.1415926;
	const std::string ptxPath = "../SAM/ptxfiles/";
	std::string defTexture(const std::string &name);
	std::string resource(const std::string &name);
	std::string shader(const std::string &name);
	std::string pathToPTX(const std::string &filename);

	bool equals(const optix::float3 &x, const optix::float3 &y);
	bool equals(float x, float y);
	void print(const optix::float3 &x);
};