
// Scalable Ambient Obscurance (SAO) screen-space ambient obscurance algorithm
// by Morgan McGuire and Michael Mara, NVIDIA Research. See license_sao.txt

#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#line 11

#ifdef COMPILEVS

// Gets a vector in view space pointing to 'clipPos' of the far plane.
// We can use this to calculate the view space position of the fragment
// at vScreenPos only.
float3 GetViewFarRay(float4 clipPos)
{
    return float3(cFrustumSize.x * clipPos.x,
                  cFrustumSize.y * clipPos.y,
                  cFrustumSize.z);
}

void VS(float4 iPos : POSITION,
    out float4 oPos : POSITION,
    out float2 oScreenPos : TEXCOORD0,
    out float3 oFarRay : TEXCOORD1,
    out float3 oFrustumSize : TEXCOORD2)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
    oFarRay = GetViewFarRay(oPos);
    oFrustumSize = cFrustumSize;
}

#endif


#ifdef COMPILEPS

// Total number of direct samples to take at each pixel
#define NUM_SAMPLES 9.0

// This is the number of turns around the circle that the spiral pattern makes.
// This should be prime to prevent taps from lining up.  This particular choice
// was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS 7.0

// Far plane informations
//uniform float3 cFrustumSize;
// Viewport inverse sizes
//uniform float4 cGBufferOffsets;

// To transform screen pos to view space
uniform float4 cProjInfo;
// View matrix
uniform float3x3 cView;

// Multiplier of the noise textures, often you see = sizeof(viewport) / sizeof(noise)
uniform float cNoiseMult;

// The height in pixels of a 1m object if viewed from 1m away.
// You can compute it from your projection matrix.  The actual value is just
// a scale factor on radius; you can simply hardcode this to a constant (~500)
// and make your radius value unitless (...but resolution dependent.)
uniform float cProjScale;
uniform float cProjScale2;

// World-space AO radius in scene units (r).  e.g., 1.0m
uniform float cRadius;

// Bias to avoid AO in smooth corners, e.g., 0.01m
uniform float cBias;

// Intensity / radius^6
uniform float cIntensityDivR6;

float2 SmallEncodeDepth(float depth)
{
    float2 ret;
    depth *= 255.0;
    ret.x = floor(depth);
    depth = (depth - ret.x) * 255.0;
    ret.y = floor(depth);
    ret.xy *= 1.0 / 255.0;
    return ret;
}

float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898,78.233))) * 43758.5453);
}

float3 GetViewPosition(float2 ssPos, float depth, float3 iFrustumSize)
{
    // eye_z = depth(0=camera .. 1=far) * far
    float eye_z = depth * iFrustumSize.z;

    float2 t = ssPos * cProjInfo.xy + cProjInfo.zw;
    return float3(t.x, t.y, 1.0) * eye_z;
}

void PS(float2 iScreenPos : TEXCOORD0, // DirectX iScreenPos zero in upper left corner
    float3 iFarRay : TEXCOORD1,
    float3 iFrustumSize : TEXCOORD2,
    out float4 oColor : OUTCOLOR0)
{
    // View space position
    float depthC = Sample2D(DepthBuffer, iScreenPos).r;
    float3 vsC = GetViewPosition(iScreenPos, depthC, iFrustumSize);

#if 0
    // For testing position reconstruction
    float originDepth = Sample(sDepthBuffer, iScreenPos).r;
    float3 viewPos = iFarRay * originDepth;
    float vlen;
    if (iScreenPos.x < 0.3 || iScreenPos.x > 0.7)
        vlen = length(viewPos)/100.0;
    else
        vlen = length(vsC)/100.0;
    oColor = float4(vlen, vlen, vlen, 1.0);
    return;
#endif

#if 1
    // Decrease intensity in the distance (TODO: the blur makes occlusion too big)
    float fadeStart = 20.0;
    float fadeEnd = 60.0;
    if (vsC.z > fadeEnd)
    {
        oColor = float4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    float intensity = clamp((fadeEnd - vsC.z) / (fadeEnd - fadeStart), 0.0, 1.0) * cIntensityDivR6;
#else
    float intensity = cIntensityDivR6;
#endif

    // Hash function
    float randomAngle = frac( pow(iScreenPos.x, iScreenPos.y) * 43758.5453 ) / cGBufferInvSize.x;
    //float2 ssC = iScreenPos / cGBufferInvSize;
    //float randomAngle = float((rand(ssC) * ssC.x * ssC.y) * 10);

#ifdef NORMAL_MAP
    float3 normal = DecodeNormal(Sample2D(NormalMap, iScreenPos));
    float3 vsN = mul(normal, cView);
#else
    // Reconstruct normals from positions. These will lead to 1-pixel black lines
	// at depth discontinuities, however the blur will wipe those out so they are
    // not visible in the final image.
	//float3 vsN = normalize( cross(ddy(vsC), ddx(vsC)) );
	float3 vsN = normalize( cross(ddx(vsC), ddy(vsC)) );
#endif

    // Choose the screen-space sample radius
	// proportional to the projected area of the sphere
	float ssDiskRadius = cProjScale2 * cProjScale * cRadius / vsC.z;

    float sum = 0.0;
    float radius2 = cRadius * cRadius;
    const float epsilon = 0.01;

	for (float i = 0.0; i < NUM_SAMPLES; ++i)
	{
        // Screen-space radius for the tap on a unit disk
        float alpha = (i + 0.5) / NUM_SAMPLES;
        float angle = randomAngle + alpha * (NUM_SPIRAL_TURNS * 6.28);

        float2 ssOffset = floor( alpha * ssDiskRadius * float2(cos(angle), sin(angle)) );

        // Sample screen space position in pixels
        float2 ssP = iScreenPos + ssOffset * cGBufferInvSize;

        // Sample view space position
        float depthP = Sample2D(DepthBuffer, ssP).r;
        float3 vsP = GetViewPosition(ssP, depthP, iFrustumSize);

        // Dal sample all'origin
        float3 v = vsP - vsC;

        float vv = dot(v, v);
        float vn = dot(v, vsN);

        // A: From the HPG12 paper
        // Note large epsilon to avoid overdarkening within cracks
        // sum += float(vv < radius2) * max((vn - cBias) / (epsilon + vv), 0.0) * radius2 * 0.6;

        // B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
        float f = max(radius2 - vv, 0.0);
        sum += f * f * f * max((vn - cBias) / (epsilon + vv), 0.0);

        // C: Medium contrast (which looks better at high radii), no division.  Note that the
        // contribution still falls off with radius^2, but we've adjusted the rate in a way that is
        // more computationally efficient and happens to be aesthetically pleasing.
        // sum += 4.0 * max(1.0 - vv * invRadius2, 0.0) * max(vn - cBias, 0.0);

        // D: Low contrast, no division operation
        // sum += 2.0 * float(vv < radius2) * max(vn - cBias, 0.0);
	}

	float occlusion = max(0.0, 1.0 - sum * intensity * (5.0 / NUM_SAMPLES));

#if 0
	// Bilateral box-filter over a quad for free, respecting depth edges
	// (the difference that this makes is subtle)
	if (abs(ddx(vsC.z)) < 0.02)
	{
		occlusion -= ddx(occlusion) * ((ssC.x & 1) - 0.5);
	}
	if (abs(ddy(vsC.z)) < 0.02)
	{
		occlusion -= ddy(occlusion) * ((ssC.y & 1) - 0.5);
	}
#endif

    oColor = float4(occlusion, SmallEncodeDepth(depthC), 1.0);
    //oColor = float4(occlusion, occlusion, occlusion, 1.0);
}
#endif
