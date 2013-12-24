#pragma once

#include <optixu/optixu_vector_types.h>

//basic point light implementation
struct BasicLight
{
	optix::float3 pos;
	optix::float3 color;

	__device__ optix::float3 colorAtDistance(float d)
	{
		return color * (1.0f / (attenuation.x + attenuation.y * d + attenuation.z * d * d));
	}

	//constant, linear, quadratic
	optix::float3 attenuation;
};