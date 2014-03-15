#include "Utils.h"

using namespace optix;
using namespace glm;
using namespace std;

namespace trayc
{
	float Utils::eps = 1e-3f;
	float Utils::pi = 3.1415926f;
	string Utils::ptxPath = "../Resources/ptxfiles/";

	string Utils::pathToPTX(const string &filename)
	{
		return ptxPath + filename + ".ptx";
	}

	string Utils::defTexture(const string &name)
	{
		return "../DefaultTextures/" + name;
	}

	string Utils::resource(const string &name)
	{
		return "../Resources/" + name;
	}

	string Utils::shader(const string &name)
	{
		return "../Shaders/" + name;
	}

	float3 Utils::aiToOptix(const aiVector3D &vec)
	{
		return make_float3(vec.x, vec.y, vec.z);
	}

	float3 Utils::glmToOptix(const vec3 &vec)
	{
		return make_float3(vec.x, vec.y, vec.z);
	}

	bool Utils::equals(const float3 &x, const float3 &y)
	{
		return abs(x.x - y.x) < eps && abs(x.y - y.y) < eps && abs(x.z - y.z) < eps;
	}

	bool Utils::equals(float x, float y)
	{
		return abs(x - y) < eps;
	}

	void Utils::print(const float3 &x)
	{
		printf("%.2f %.2f %.2f\n", x.x, x.y, x.z);
	}
}