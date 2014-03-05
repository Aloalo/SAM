#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_aabb_namespace.h>

using namespace optix;

rtBuffer<float3> vertex_buffer;     
rtBuffer<float3> normal_buffer;
rtBuffer<float3> tangent_buffer;
rtBuffer<float2> texcoord_buffer;
rtBuffer<int3> index_buffer;
rtTextureSampler<float4, 2, cudaReadModeElementType> normal_map;

rtDeclareVariable(float3, texcoord, attribute texcoord, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

template<bool useNormalMap>
static __inline__ __device__ void mesh_intersect(int primIdx)
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
			if(texcoord_buffer.size() == 0 || idx.x < 0 || idx.y < 0 || idx.z < 0)
				texcoord = make_float3(0.0f, 0.0f, 0.0f);
			else 
			{
				float2 t0 = texcoord_buffer[idx.x];
				float2 t1 = texcoord_buffer[idx.y];
				float2 t2 = texcoord_buffer[idx.z];
				texcoord = make_float3(t1 * beta + t2 * gamma + t0 * (1.0f - beta - gamma));
			}
			
			geometric_normal = normalize(n);

			if(normal_buffer.size() == 0 || idx.x < 0 || idx.y < 0 || idx.z < 0)
				shading_normal = normalize(n);
			else
			{
				float3 n0 = normal_buffer[idx.x];
				float3 n1 = normal_buffer[idx.y];
				float3 n2 = normal_buffer[idx.z];
				shading_normal = normalize(n1 * beta + n2 * gamma + n0 * (1.0f - beta - gamma));
			}

			if(useNormalMap)
			{
				float3 shading_tangent = normalize(tangent_buffer[idx.y] * beta +
					tangent_buffer[idx.z] * gamma + tangent_buffer[idx.x] * (1.0f - beta - gamma));
				float3 normal = make_float3(tex2D(normal_map, texcoord.x, texcoord.y)) * 2.f - 1.f;
				float3 transformed_normal;
				transformed_normal.x = dot(shading_tangent, normal);
				transformed_normal.y = dot(cross(shading_tangent, shading_normal), normal);
				transformed_normal.z = dot(shading_normal, normal);
				shading_normal += transformed_normal;
				shading_normal = normalize(shading_normal);
			}

			rtReportIntersection(0);
		}
	}
}

RT_PROGRAM void mesh_intersect_normalmap(int primIdx)
{
	mesh_intersect<true>(primIdx);
}

RT_PROGRAM void mesh_intersect_no_normalmap(int primIdx)
{
	mesh_intersect<false>(primIdx);
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

