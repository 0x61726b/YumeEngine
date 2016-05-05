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
#include "YumeHeaders.h"
#include "Material.h"



namespace YumeEngine
{
	Material::Material()
	{
		textures_.resize(MAX_TEXTURE_UNITS);
	}

	Material::~Material()
	{
	}

	void Material::SetTexture(unsigned index,TexturePtr texture)
	{
		if(textures_.size() <= index)
			textures_.resize(index + 1);

		textures_[index] = texture;
	}

	void Material::SetShaderParameter(YumeHash param,const Variant& var)
	{
		parameters_[param] = var;
	}

	void Material::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& value)
	{
		shaderVectors3[param] = value;
	}

	void Material::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& value)
	{
		shaderVectors4[param] = value;
	}

	bool Material::HasTexture(const YumeString& tname)
	{
		if(tname == "Textures/default.png")
			return false;
		else
			return true;
	}
}
