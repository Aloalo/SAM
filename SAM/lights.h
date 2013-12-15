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

struct TriangleLight
{
	typedef optix::float3 float3;
	float3 v1, v2, v3;
	float3 normal;
	float3 emission;
};

