#pragma once

#include "Macros.h"
#include <string>
#include <optix_math.h>

//host utils
namespace utils
{
	const float eps = 1e-3f;
	const float pi = 3.1415926;

	enum Materials
	{
		MIRROR = 0,
		GLASS = 1,
		WALL = 2,
		FLOOR = 3,
		EMPTY = 4
	};

	std::string pathToPTX(const std::string &filename);
	bool equals(const optix::float3 &x, const optix::float3 &y);
};