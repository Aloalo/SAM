#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_aabb_namespace.h>

using namespace optix;

rtDeclareVariable(float3, boxmin, , );
rtDeclareVariable(float3, boxmax, , );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 

static __device__ float3 boxnormal(float t)
{
	float3 t0 = (boxmin - ray.origin)/ray.direction;
	float3 t1 = (boxmax - ray.origin)/ray.direction;
	float3 neg = make_float3(t==t0.x?1:0, t==t0.y?1:0, t==t0.z?1:0);
	float3 pos = make_float3(t==t1.x?1:0, t==t1.y?1:0, t==t1.z?1:0);
	return pos-neg;
}

RT_PROGRAM void box_intersect(int)
{
	float3 t0 = (boxmin - ray.origin) / ray.direction;
	float3 t1 = (boxmax - ray.origin) / ray.direction;
	float3 near = fminf(t0, t1);
	float3 far = fmaxf(t0, t1);
	float tmin = fmaxf(near);
	float tmax = fminf(far);

	if(tmin <= tmax)
	{
		bool check_second = true;
		if(rtPotentialIntersection(tmin))
		{
			shading_normal = geometric_normal = boxnormal(tmin);
			if(rtReportIntersection(0))
				check_second = false;
		} 
		if(check_second)
		{
			if(rtPotentialIntersection(tmax))
			{
				shading_normal = geometric_normal = boxnormal(tmax);
				rtReportIntersection(0);
			}
		}
	}
}

RT_PROGRAM void box_bounds(int, float result[6])
{
	optix::Aabb* aabb = (optix::Aabb*)result;
	aabb->set(boxmin, boxmax);
}
