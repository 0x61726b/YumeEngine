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

float3 BRDFPointLight(in float3 diffuseAlbedo,in float3 normal,in float3 position,in float3 lightColor,float3 lightPosition,float lightRange,in float shininess)
{
		float att = 1.0f;
		float3 L = lightPosition - position;
		float distance = length( L );

		att = max(0, 1.0f - ( distance / lightRange ));

		L /= distance;

		const float PI = 3.14159265f;

		float3 diffuse = (lightColor * diffuseAlbedo) / PI;
		float3 E = normalize(camera_pos - position); //E
		float3 H = normalize(L + E); //H

		float3 Ks = float3(0.3, 0.3, 0.3);

		float3 D = ((shininess + 2) / (2 * PI)) * (pow(max(0.01, dot(normal, H)), shininess));

		float3 F = Ks + ((1 - Ks) * (pow(1 - (max(0.0, dot(L, H))), 5)));

		float3 G = 1 / pow(max(0.0001, dot(L, H)), 2);

		float3 BRDF = (F * G * D) / 4;

		float3 light = (diffuse + BRDF) * (max(0.0, dot(normal, L)) * (lightColor * diffuseAlbedo * PI));

		return light*att;
}

float3 GetDirectionalLight(float3 diffuse,float4 specular,float3 viewRay,float depth,float3 normal)
{
	float3 V = viewRay;
	float3 P = camera_pos + (-V * depth);;

	float3 Ll = -LightDirection.rgb;

	float3 L = normalize(Ll);

	float3 N = normal;

  float3 Vv = camera_pos - P;
	float3 H = normalize( L + Vv );

	//Lambert
	float NoL = saturate(dot(N,L));

	float power = 1;

	float3 Li = M_PI * (power * get_spotlight(L, N) * LightColor.rgb) / pow(length(Ll), 2);

	float roughness = 1;

	float3 f = brdf(L, V, N, diffuse, float3(0.6f, 0.6f, 0.6f), roughness);

  float specPower = 16;
  float specNormalizationFactor = ( ( specPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 S = pow( saturate( dot( N, H ) ), specPower ) * specNormalizationFactor * LightColor.xyz * float3(0.6f, 0.6f, 0.6f) * NoL;

	return f * Li * NoL + S;
}
