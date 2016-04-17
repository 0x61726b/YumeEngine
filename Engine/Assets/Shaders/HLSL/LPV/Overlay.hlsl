/*
 * overlay.hlsl by Tobias Alexander Franke (tob@cyberhead.de) 2011
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

SamplerState StandardFilter   : register(s0);

struct PS_OVERLAY_INPUT
{
	float4 position	        : SV_POSITION;
	float2 tex_coord        : TEXCOORD0;
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

float4 PS(in PS_OVERLAY_INPUT In) : SV_Target
{
	float2 ntex = (In.tex_coord);

	float4 over = overlay_tex.Sample(StandardFilter, ntex);

  return over;
}
