#pragma once

#include <optixu/optixu_vector_types.h>

//basic point light implementation
struct BasicLight
{
#ifdef _WIN32
	typedef optix::float3 float3;
#endif // _WIN32

	__host__ BasicLight(const float3 &pos, const float3 &color, const float3 &attenuation, const float3 &spot_direction,
		float spot_cutoff, float spot_exponent, float radius, int casts_shadows, int is_directional)
		: pos(pos), color(color), attenuation(attenuation), spot_direction(spot_direction), spot_cutoff(spot_cutoff),
		spot_exponent(spot_exponent), radius(radius), casts_shadows(casts_shadows), is_directional(is_directional)
	{}

	float3 pos;
	float3 color;
	
	//constant, linear, quadratic
	float3 attenuation;
	float3 spot_direction;
	float spot_cutoff;
	float spot_exponent;
	float radius;
	int casts_shadows;
	int is_directional;
};
