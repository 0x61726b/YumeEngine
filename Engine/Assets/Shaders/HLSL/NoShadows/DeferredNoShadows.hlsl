

#include "deferred.hlsl"


float4 ps_deferred(in PS_INPUT inp) : SV_Target
{
  gbuffer gb = unpack_gbuffer(inp.tex_coord);

  float3 N = normalize(gb.normal.xyz * 2.0 - 1.0);

  // view
  float3 V = -normalize(inp.view_ray.xyz);

  float3 P = camera_pos + (-V * gb.depth);

  // light
  float3 Ll = main_light.position.xyz - P;
  float3 L = normalize(Ll);

  // lambert
  float NoL = saturate(dot(N, L));

  // have no gloss maps, roughness is simply inverse spec color/smoothness
  float roughness = 1;

  // calculate power of light and scale it to get the scene adequately lit
  float power = 100 * length(scene_dim_max - scene_dim_min);

  // calculate irradiance
  float3 Li = M_PI * (power * get_spotlight(L, N) * main_light.color.rgb) / pow(length(Ll),2);

  float3 f = brdf(L, V, N, gb.diffuse_albedo.rgb, gb.specular_albedo.rgb, roughness);

  // emissive
  float3 T0 = 0;

  // direct
  float3 T1 = f * Li * NoL;

  return float4(max(T0 + T1, 0.0f), 1.0f);
}
