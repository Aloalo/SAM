#include "Box.h"

using namespace optix;

namespace trayc
{
	Box::Box(void)
	{
	}

	Box::Box(const float3 &boxmin, const float3 &boxmax, int materialIdx)
		: boxmin(boxmin), boxmax(boxmax), matIdx(materialIdx)
	{
	}

	Box Box::translated(const optix::float3 &t) const
	{
		return Box(boxmin + t, boxmax + t, matIdx);
	}

	Box Box::translated(const optix::float3 &t, int matId) const
	{
		return Box(boxmin + t, boxmax + t, matId);
	}

	float Box::getWidth() const
	{
		return abs(boxmin.x - boxmax.x);
	}

	float Box::getHeight() const
	{
		return abs(boxmin.y - boxmax.y);
	}

	float Box::getDepth() const
	{
		return abs(boxmin.z - boxmax.z);
	}
}
