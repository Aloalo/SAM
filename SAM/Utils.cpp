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

	bool equals(float x, float y)
	{
		return abs(x - y) < eps;
	}
	
	void print(const float3 &x)
	{
		printf("%.2f %.2f %.2f\n", x.x, x.y, x.z);
	}
};