#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEVS
cbuffer CustomVS : register(b6)
{
  float4   cFrustumCorners[4];
}
#else
SamplerState samRandomVec
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

cbuffer CustomPS : register(b6)
{
  float4x4 cViewToTexSpace; // Proj*Texture
  float4   cOffsetVectors[14];
}

float OcclusionFunction(float distZ)
{
	const float occlusionFadeStart = 0.2f;
	const float occlusionFadeEnd = 2.0f;
	const float surfaceEpsilon = 0.05f;
	float occlusion = 0.0f;
	if (distZ > surfaceEpsilon)
	{
		float fadeLength = occlusionFadeEnd - occlusionFadeStart;
		occlusion = saturate((occlusionFadeEnd - distZ) / fadeLength);
	}
	return occlusion;
}


#endif

void VS(float4 iPos : POSITION,
        float3 iToFarPlane : NORMAL,
        float2 iTexCoord : TEXCOORD0,
    out float2 oTexCoord : TEXCOORD0,
    out float2 oScreenPos : TEXCOORD1,
    out float3 oToFarPlane : TEXCOORD2,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    //oTexCoord = GetQuadTexCoord(oPos) + cBlurHOffsets ;
    oTexCoord = iTexCoord;
    oScreenPos = GetScreenPosPreDiv(oPos);
    oToFarPlane = cFrustumCorners[iToFarPlane.x].xyz;
}
Texture2D tRandomVecMap : register(t6);

void PS(float2 iTexCoord : TEXCOORD0,
        float2 iScreenPos : TEXCOORD1,
        float3 iToFarPlane : TEXCOORD2,
    out float4 oColor : OUTCOLOR0)
{
	float4 normalDepth = Sample2DLod0(DepthBuffer, iTexCoord);
	float3 n = normalDepth.xyz;
	float pz = normalDepth.w;
	float3 p = (pz / iToFarPlane.z)*iToFarPlane;
	float3 randVec = 2.0f*tRandomVecMap.SampleLevel(samRandomVec, 4.0f*iTexCoord, 0.0f).rgb - 1.0f;
	float occlusionSum = 0.0f;

	const float occlusionRadius = 0.5f;
	[unroll]
	for (int i = 0; i < 14; ++i)
	{
		float3 offset = reflect(cOffsetVectors[i].xyz, randVec);
		float flip = sign(dot(offset, n));
		float3 q = p + flip * occlusionRadius * offset;
		float4 projQ = mul(float4(q, 1.0f), cViewToTexSpace);
		projQ /= projQ.w;
		float rz = Sample2DLod0(DepthBuffer, projQ.xy).a;
		float3 r = (rz / q.z) * q;
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp * OcclusionFunction(distZ);
		occlusionSum += occlusion;
	}
	occlusionSum /= 14;
	float access = 1.0f - occlusionSum;
	oColor = saturate(pow(access, 4.0f));
}
