#pragma once

#include <optixu/optixu_vector_types.h>

//basic point light implementation
struct BasicLight
{
	optix::float3 pos;
	optix::float3 color;
};