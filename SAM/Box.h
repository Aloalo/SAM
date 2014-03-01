#pragma once

#include "Macros.h"
#include <optix_math.h>

struct Box
{
	Box(void);
	Box(const optix::float3 &boxmin, const optix::float3 &boxmax, int materialIdx);
    
	Box translated(const optix::float3 &t) const;
	Box translated(const optix::float3 &t, int matId) const;
	float getWidth() const;
	float getHeight() const;
	float getDepth() const;

	optix::float3 boxmin;
	optix::float3 boxmax;
	int matIdx;
};
