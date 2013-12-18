#include "Utils.h"

namespace utils
{
	std::string pathToPTX(const std::string &filename)
	{
		return "../SAM/ptxfiles/" + filename + ".ptx";
	}

	
	bool equals(const float3 &x, const float3 &y)
	{
		return abs(x.x - y.x) < eps && abs(x.y - y.y) < eps && abs(x.z - y.z) < eps;
	}
};