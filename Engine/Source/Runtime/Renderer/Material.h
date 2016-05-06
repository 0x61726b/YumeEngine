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
#ifndef __Material_h__
#define __Material_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeTexture.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport Material : public YumeBase
	{
	public:
		Material();
		virtual ~Material();

		void SetTexture(unsigned index,TexturePtr texture);
		void SetShaderParameter(YumeHash param,const Variant& var);
		void SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& value);
		void SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& value);

		TexturePtr GetTexture(unsigned index) const { return textures_[index]; }

		bool HasTexture(const YumeString& tname);

		const YumeMap<YumeHash, Variant>::type& GetParameters() const { return parameters_; }
		const YumeMap<YumeHash,DirectX::XMFLOAT3>::type& GetShaderVectors3() { return shaderVectors3; }
		const YumeMap<YumeHash,DirectX::XMFLOAT4>::type& GetShaderVectors4() { return shaderVectors4; }

		const YumeVector<SharedPtr<YumeTexture> >::type& GetTextures() const { return textures_; }

		unsigned GetNumTextures() const { return numTextures; }
	private:
		typedef YumeVector<SharedPtr<YumeTexture> > MaterialTextures;
		MaterialTextures::type textures_;

		typedef YumeMap<YumeHash, Variant> MaterialParameters;
		MaterialParameters::type parameters_;

		YumeMap<YumeHash,DirectX::XMFLOAT3>::type shaderVectors3;
		YumeMap<YumeHash,DirectX::XMFLOAT4>::type shaderVectors4;

		unsigned numTextures;
	};

	typedef Material* MaterialPtr;
}


//----------------------------------------------------------------------------
#endif
