#pragma once

#include <optixu/optixu_vector_types.h>

struct BasicLight
{
	typedef optix::float3 float3;
	float3 pos;
	float3 color;
	int casts_shadow; 
	int padding;
};