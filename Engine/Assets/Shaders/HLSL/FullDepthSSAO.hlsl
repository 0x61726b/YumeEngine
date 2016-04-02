#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEVS

cbuffer CustomVS : register(b6)
{
  float2 cBlurHOffsets;
}

#else
cbuffer CustomPS : register(b6)
{
  float2 cBlurHInvSize;
}

SamplerState samRandomVec
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU  = WRAP;
    AddressV  = WRAP;
};


static const float offsets[5] = {
    2.0,
    1.0,
    0.0,
    -1.0,
    -2.0,
};

static const float weights[5] = {
    0.1,
    0.25,
    0.3,
    0.25,
    0.1
};


float3 normal_from_depth(float depth, float2 texcoords) {

	const float2 offset1 = float2(0.0, 0.001);
	const float2 offset2 = float2(0.001, 0.0);

	float depth1 = Sample2D(DepthBuffer, texcoords + offset1).r;
	float depth2 = Sample2D(DepthBuffer, texcoords + offset2).r;

	float3 p1 = float3(offset1, depth1 - depth);
		float3 p2 = float3(offset2, depth2 - depth);

		float3 normal = cross(p1, p2);
		normal.z = -normal.z;

	return normalize(normal);
}

#endif

void VS(float4 iPos : POSITION,
    out float2 oTexCoord : TEXCOORD0,
    out float2 oScreenPos : TEXCOORD1,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = GetQuadTexCoord(oPos) + cBlurHOffsets ;
    oScreenPos = GetScreenPosPreDiv(oPos);
}

Texture2D tRandomVecMap : register(t6);

void PS(float2 iTexCoord : TEXCOORD0,
    float2 iScreenPos : TEXCOORD1,
    out float4 oColor : OUTCOLOR0)
{
#ifdef SSAO_PASS
	const float total_strength = 1.0;
	const float base = 0.2;

	const float area = 0.0075;
	const float falloff = 0.000001;

	const float radius = 0.0002;

	const int samples = 16;
	float3 sample_sphere[samples] = {
		float3(0.5381, 0.1856, -0.4319), float3(0.1379, 0.2486, 0.4430),
		float3(0.3371, 0.5679, -0.0057), float3(-0.6999, -0.0451, -0.0019),
		float3(0.0689, -0.1598, -0.8547), float3(0.0560, 0.0069, -0.1843),
		float3(-0.0146, 0.1402, 0.0762), float3(0.0100, -0.1924, -0.0344),
		float3(-0.3577, -0.5301, -0.4358), float3(-0.3169, 0.1063, 0.0158),
		float3(0.0103, -0.5869, 0.0046), float3(-0.0897, -0.4940, 0.3287),
		float3(0.7119, -0.0154, -0.0918), float3(-0.0533, 0.0596, -0.5411),
		float3(0.0352, -0.0631, 0.5460), float3(-0.4776, 0.2847, -0.0271)
	};

	float3 randVec = tRandomVecMap.SampleLevel(samRandomVec, iScreenPos, 0.0f).rgb;

	float depth = Sample2D(DepthBuffer, iScreenPos).r;
	float3 position = float3(iScreenPos, depth);
	float3 normal = normal_from_depth(depth, iScreenPos);

	float radius_depth = radius / depth;
	float occlusion = 0.0;
	for (int i = 0; i < samples; i++) {

		float3 ray = radius_depth * reflect(sample_sphere[i], randVec);
		float3 hemi_ray = position + sign(dot(ray, normal)) * ray;

		float occ_depth = Sample2D(DepthBuffer, saturate(hemi_ray.xy)).r;
		float difference = depth - occ_depth;

		occlusion += step(falloff, difference) * (1.0 - smoothstep(falloff, area, difference));
	}

	float ao = 1.0 - total_strength * occlusion * (1.0 / samples);

	oColor = saturate(ao + base);
#endif

#ifdef BLURH_PASS
  float3 rgb = 0.0;
  for (int i = 0; i < 5; ++i)
      rgb += Sample2D(DiffMap, iTexCoord + (float2(offsets[i], 0.0)) * cBlurHInvSize).rgb * weights[i];
  oColor = float4(rgb, 1.0);
#endif

#ifdef BLURV_PASS
  float3 rgb = 0.0;
  for (int i = 0; i < 5; ++i)
      rgb += Sample2D(DiffMap, iTexCoord + (float2(0.0, offsets[i])) * cBlurHInvSize).rgb * weights[i];
  oColor = float4(rgb, 1.0);
#endif
}
