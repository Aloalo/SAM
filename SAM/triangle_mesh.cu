#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_aabb_namespace.h>

using namespace optix;

rtBuffer<float3> vertex_buffer;     
rtBuffer<float3> normal_buffer;
rtBuffer<float3> tangent_buffer;
rtBuffer<float3> bitangent_buffer;
rtBuffer<float2> texcoord_buffer;
rtBuffer<int3> index_buffer;
rtTextureSampler<uchar4, 2, cudaReadModeNormalizedFloat> normal_map;

rtDeclareVariable(float3, texcoord, attribute texcoord, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void mesh_intersect(int primIdx)
{
	const int3 &idx = index_buffer[primIdx];

	const float3 &p0 = vertex_buffer[idx.x];
	const float3 &p1 = vertex_buffer[idx.y];
	const float3 &p2 = vertex_buffer[idx.z];

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
				const float2 &t0 = texcoord_buffer[idx.x];
				const float2 &t1 = texcoord_buffer[idx.y];
				const float2 &t2 = texcoord_buffer[idx.z];
				texcoord = make_float3(t1 * beta + t2 * gamma + t0 * (1.0f - beta - gamma));
			}
			
			geometric_normal = normalize(n);

			if(normal_buffer.size() == 0 || idx.x < 0 || idx.y < 0 || idx.z < 0)
				shading_normal = geometric_normal;
			else
			{
				shading_normal = normalize(normal_buffer[idx.y] * beta + 
					normal_buffer[idx.z] * gamma + normal_buffer[idx.x] * (1.0f - beta - gamma));
		
				if(tangent_buffer.size() > 0)
				{
					const float3 shading_tangent = normalize(tangent_buffer[idx.y] * beta +
						tangent_buffer[idx.z] * gamma + tangent_buffer[idx.x] * (1.0f - beta - gamma));
					const float3 shading_bitangent = normalize(bitangent_buffer[idx.y] * beta +
						bitangent_buffer[idx.z] * gamma + bitangent_buffer[idx.x] * (1.0f - beta - gamma));
					Matrix3x3 tbni;
					tbni.setCol(0, shading_tangent);
					tbni.setCol(1, shading_bitangent);
					tbni.setCol(2, shading_normal);
					shading_normal = tbni * normalize(make_float3(tex2D(normal_map, texcoord.x, texcoord.y)) * 2.f - 1.f);
				}
			}

			rtReportIntersection(0);
		}
	}
}

RT_PROGRAM void mesh_bounds(int primIdx, float result[6])
{  
	const int3 &idx = index_buffer[primIdx];

	const float3 &v0 = vertex_buffer[idx.x];
	const float3 &v1 = vertex_buffer[idx.y];
	const float3 &v2 = vertex_buffer[idx.z];
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

