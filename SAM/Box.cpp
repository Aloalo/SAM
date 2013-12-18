#include "Box.h"

using namespace optix;

Box::Box(void)
{
}

Box::Box(const float3 &boxmin, const float3 &boxmax, int materialIdx)
	: boxmin(boxmin), boxmax(boxmax), matIdx(materialIdx)
{
}

Box Box::translated(const optix::float3 &t)
{
	return Box(boxmin + t, boxmax + t, matIdx);
}

Box Box::translated(const optix::float3 &t, int matId)
{
	return Box(boxmin + t, boxmax + t, matId);
}

