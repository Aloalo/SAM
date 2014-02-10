#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_aabb_namespace.h>

using namespace optix;

rtBuffer<float3> vertex_buffer;     
rtBuffer<float3> normal_buffer;
rtBuffer<float2> texcoord_buffer;
rtBuffer<int3> index_buffer;

//rtBuffer<uint> material_buffer; // per-face material index
rtDeclareVariable(float3, texcoord, attribute texcoord, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void mesh_intersect(int primIdx)
{
	int3 idx = index_buffer[primIdx];

	float3 p0 = vertex_buffer[idx.x];
	float3 p1 = vertex_buffer[idx.y];
	float3 p2 = vertex_buffer[idx.z];

	float3 n;
	float t, beta, gamma;
	if(intersect_triangle(ray, p0, p1, p2, n, t, beta, gamma))
	{
		if(rtPotentialIntersection(t))
		{
			if(normal_buffer.size() == 0 || idx.x < 0 || idx.y < 0 || idx.z < 0)
				shading_normal = normalize(n);
			else
			{
				float3 n0 = normal_buffer[idx.x];
				float3 n1 = normal_buffer[idx.y];
				float3 n2 = normal_buffer[idx.z];
				shading_normal = normalize(n1 * beta + n2 * gamma + n0 * (1.0f - beta - gamma));
			}
			geometric_normal = normalize(n);

			if(texcoord_buffer.size() == 0 || idx.x < 0 || idx.y < 0 || idx.z < 0)
				texcoord = make_float3(0.0f, 0.0f, 0.0f);
			else 
			{
				float2 t0 = texcoord_buffer[idx.x];
				float2 t1 = texcoord_buffer[idx.y];
				float2 t2 = texcoord_buffer[idx.z];
				texcoord = make_float3(t1 * beta + t2 * gamma + t0 * (1.0f - beta - gamma));
			}
			//rtReportIntersection(material_buffer[primIdx]);
			rtReportIntersection(0);
		}
	}
}

RT_PROGRAM void mesh_bounds(int primIdx, float result[6])
{  
	const int3 idx = index_buffer[primIdx];

	const float3 v0 = vertex_buffer[idx.x];
	const float3 v1 = vertex_buffer[idx.y];
	const float3 v2 = vertex_buffer[idx.z];
	const float area = length(cross(v1 - v0, v2 - v0));

	optix::Aabb *aabb = (optix::Aabb*)result;

	if(area > 0.0f && !isinf(area))
	{
		aabb->m_min = fminf(fminf(v0, v1), v2);
		aabb->m_max = fmaxf(fmaxf(v0, v1), v2);
	}
	else
		aabb->invalidate();
}
