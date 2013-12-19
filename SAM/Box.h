#pragma once

#include "macros.h"
#include <optix_math.h>

struct Box
{
	Box(void);
	Box(const optix::float3 &boxmin, const optix::float3 &boxmax, int materialIdx);
    
	Box translated(const optix::float3 &t);
	Box translated(const optix::float3 &t, int matId);

	optix::float3 boxmin;
	optix::float3 boxmax;
	int matIdx;
};
