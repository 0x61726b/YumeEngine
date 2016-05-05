//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "deferred.hlsl"

Texture2D PointLightAtt : register(t6);

cbuffer LightParameters : register(b3)
{
  float4 LightColor;
  float4 LightPosition;
  float4 LightDirection;
}

#ifdef POINTLIGHT_STANDARD
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float3 ViewRay  : VIEWRAY;
};
#elif POINTLIGHT_BRDF
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float3 ViewRay  : VIEWRAY;
};
#elif DIRECTIONALLIGHT
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float2 TexCoord : TEXCOORD0;
  float3 ViewRay  : VIEWRAY;
};
#endif

#include "Lighting.hlsl"

// Toksvig AA for specular highlights
float toksvig_ft(in float3 Na, in float roughness)
{
    float a = roughness*roughness;
    float s = 2.0f / (a * a) - 2.0f;

    float len = length(Na);
    return len/lerp(s, 1, len);
}


float4 ps_df(in PS_INPUT_POS inp) : SV_Target
{
  float4 screenPos = inp.Position;
	float3 viewRay = inp.ViewRay;

	int3 texCoord = int3(screenPos.xy, 0);

	float3 diffuseAlbedo = rt_colors.Load(texCoord).xyz;
	float3 normal = rt_normals.Load(texCoord).xyz;
	float4 spec = rt_specular.Load(texCoord);
	float depth = rt_lineardepth.Load(texCoord).x;

	float3 Na = normal.xyz * 2.0 - 1.0;
	float3 N = normalize(Na);

  float3 View = -normalize(viewRay.xyz);
  float3 position = camera_pos + (-View * depth);
	normal = N;
  #ifdef DIRECTIONAL_LIGHT
	return float4(GetDirectionalLight(diffuseAlbedo, spec, View, depth, normal), 1.0f);
#endif




	float3 specularAlbedo = float3(0.6f,0.6f,0.6f);
  float specPower = 16;

  //Lighting
  float3 L = 0;
  float att = 1.0f;
  float lightRange = LightDirection.w;

#ifdef POINTLIGHT
  L = LightPosition.xyz - position;

  float dist = length( L );

  att = max(0, 1.0f - ( dist / lightRange ) );

  L /= dist;
#elif DIRECTIONALLIGHT
  L = -LightDirection.xyz;
#endif

#ifdef DIRECTIONALLIGHT
  const float DiffuseNormalizationFactor = 1.0f / 3.14159265f;
	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * LightColor.xyz * diffuseAlbedo * DiffuseNormalizationFactor;

  float3 V = camera_pos - position;
	float3 H = normalize( L + V );
	float specNormalizationFactor = ( ( specPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 specular = pow( saturate( dot( normal, H ) ), specPower ) * specNormalizationFactor * LightColor.xyz * specularAlbedo.xyz * nDotL;

  float t = toksvig_ft(Na,spec.w);

  return float4(( diffuse + specular ) * att,1.0f);
  #elif POINTLIGHT_BRDF
  float3 light = BRDFPointLight(diffuseAlbedo,normal,position,LightColor.xyz,LightPosition.xyz,LightDirection.w,32);

  return float4(light,1.0f);
  #elif POINTLIGHT_STANDARD
  float3 lightDir;
  float diff = GetPointLightDiffuse(normal,position,lightDir);
  float gspec = GetSpecular(normal,-position,lightDir,255);
  return diff * float4(LightColor.xyz * (diffuseAlbedo.rgb + gspec),1.0f);
  #endif
}
