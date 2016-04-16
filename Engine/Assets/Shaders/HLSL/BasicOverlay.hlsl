/*
 * overlay.hlsl by Tobias Alexander Franke (tob@cyberhead.de) 2011
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

SamplerState ShadowFilter   : register(s2);

struct PS_INPUT
{
	float4 position	    : SV_POSITION;
	float2 tex_coord    : TEXCOORD0;
	float3 view_ray	    : TEXCOORD1;
};

Texture2D overlay_tex       : register(t10);

float2 calc_tex_coord(in float2 tex_coord)
{
    float s = 0.3;

	if (tex_coord.x >= s ||
		tex_coord.x <  0.0 ||
		tex_coord.y >= s ||
		tex_coord.x <  0.0)
		discard;

	return tex_coord * (1.0/s);
}

float4 PS(in PS_INPUT In) : SV_Target
{
	float2 ntex = (In.tex_coord);

	float4 over = abs(overlay_tex.Sample(ShadowFilter, ntex));

    if (over.x > 1.0 || over.y > 1.0 || over.z > 1.0)
        over/=1000;

    return over;
}
