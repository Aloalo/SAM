#include "helper.h"
#include <optix_world.h>

using namespace optix;

rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(float3, plane_normal, , );
rtDeclareVariable(float3, recmin, , );
rtDeclareVariable(float3, recmax, , );
 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void intersect(int primIdx)
{
	float t = dot(recmin - ray.origin, plane_normal) / dot(ray.direction, plane_normal);
	if(t > ray.tmin && t < ray.tmax)
	{
		float3 p = ray.origin + ray.direction * t;

		if(isBetween(recmin, recmax, p))
			if(rtPotentialIntersection(t))
			{
				shading_normal = geometric_normal = plane_normal;
				rtReportIntersection(0);
			}
	}
}

RT_PROGRAM void bounds(int, float result[6])
{
	optix::Aabb* aabb = (optix::Aabb*)result;
	aabb->set(recmin, recmax);
}

