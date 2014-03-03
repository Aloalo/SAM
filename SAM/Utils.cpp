#include "Utils.h"

using namespace optix;
using namespace glm;
using namespace std;

float utils::eps = 1e-3f;
float utils::pi = 3.1415926f;
string utils::ptxPath = "../SAM/ptxfiles/";

string utils::pathToPTX(const string &filename)
{
	return ptxPath + filename + ".ptx";
}

string utils::defTexture(const string &name)
{
	return "../DefaultTextures/" + name;
}

string utils::resource(const string &name)
{
	return "../Resources/" + name;
}

string utils::shader(const string &name)
{
	return "../Shaders/" + name;
}

float3 utils::aiToOptix(const aiVector3D &vec)
{
	return make_float3(vec.x, vec.y, vec.z);
}

float3 utils::glmToOptix(const vec3 &vec)
{
	return make_float3(vec.x, vec.y, vec.z);
}

bool utils::equals(const float3 &x, const float3 &y)
{
	return abs(x.x - y.x) < eps && abs(x.y - y.y) < eps && abs(x.z - y.z) < eps;
}

bool utils::equals(float x, float y)
{
	return abs(x - y) < eps;
}

void utils::print(const float3 &x)
{
	printf("%.2f %.2f %.2f\n", x.x, x.y, x.z);
}
