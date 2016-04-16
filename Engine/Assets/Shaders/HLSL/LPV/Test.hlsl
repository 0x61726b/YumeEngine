


cbuffer camera_vs               : register(b0)
{
    float4x4 vp                 : packoffset(c0);
    float4x4 vp_inv             : packoffset(c4);
    float3 camera_pos           : packoffset(c8);
    float pad                   : packoffset(c8.w);
}

cbuffer meshdata_vs             : register(b1)
{
    float4x4 world              : packoffset(c0);
}


struct VS_MESH_INPUT
{
    float3 pos                  : POSITION;
    float3 norm                 : NORMAL;
	float2 texcoord 	        : TEXCOORD0;
	float3 tangent		        : TANGENT;
};


struct VS_MESH_OUTPUT
{
    float4 pos			        : SV_POSITION;
	float4 norm			        : NORMAL;
	float3 tangent		        : TANGENT;
    float2 texcoord             : TEXCOORD0;
    float3 ldepth               : LINEARDEPTH;
};

cbuffer meshdata_ps             : register(b0)
{
	  float4 diffuse_color        : packoffset(c0);
    float4 specular_color       : packoffset(c1);
	  float4 emissive_color	    : packoffset(c2);
	  bool has_diffuse_tex        : packoffset(c3.x);
	  bool has_normal_tex         : packoffset(c3.y);
	  bool has_specular_tex       : packoffset(c3.z);
	  bool has_alpha_tex          : packoffset(c3.w);
    float shading_mode           : packoffset(c4.x);
    float roughness             : packoffset(c4.y);
    float refractive_index      : packoffset(c4.z);
}


VS_MESH_OUTPUT VS(in VS_MESH_INPUT input)
{
	VS_MESH_OUTPUT output;

    float4 pos_world = mul(world, float4(input.pos, 1.0));
    output.ldepth = pos_world.xyz - camera_pos;

    output.pos = mul(vp, pos_world);

    // TODO: needs normal matrix!
    output.norm = normalize(mul(world, float4(input.norm, 0.0)));

    // TODO: needs transform!
	output.tangent = input.tangent;
    output.texcoord = input.texcoord;

    return output;
}

float4 PS(in VS_MESH_OUTPUT input) : SV_Target0
{
	return diffuse_color;
}
