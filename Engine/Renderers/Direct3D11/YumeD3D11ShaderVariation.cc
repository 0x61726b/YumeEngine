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
#include "YumeD3D11ShaderVariation.h"
#include "Renderer/YumeShader.h"
#include "Core/YumeFile.h"
#include "Core/YumeIO.h"

#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeRendererImpl.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeDefaults.h"


namespace YumeEngine
{
	YumeD3D11ShaderVariation::YumeD3D11ShaderVariation(YumeShader* owner,ShaderType type):
		elementMask_(0)
	{
		owner_ = owner;
		type_ = type;

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			useTextureUnit_[i] = false;
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			constantBufferSizes_[i] = 0;
	}

	YumeD3D11ShaderVariation::~YumeD3D11ShaderVariation()
	{

	}

	void YumeD3D11ShaderVariation::Release()
	{

	}

	bool YumeD3D11ShaderVariation::Create()
	{
		Release();

		YumeRenderer* graphics_ = YumeEngine3D::Get()->GetRenderer();


		if(!graphics_)
			return false;

		if(!owner_)
		{
			compilerOutput_ = "Owner shader has expired";
			return false;
		}

		YumeString path,name,extension;
		SplitPath(owner_->GetName(),path,name,extension);

		YumeString binaryShaderName = path + "Cache/" + name + "_" + std::to_string(GenerateHash(defines_)) + extension;

		if(!Compile())
			return false;

		return false;
	}

	bool YumeD3D11ShaderVariation::LoadByteCode(const YumeString& binaryShaderName)
	{
		return false;
	}

	bool YumeD3D11ShaderVariation::Compile()
	{
		return false;
	}

	void YumeD3D11ShaderVariation::ParseParameters(unsigned char* bufData,unsigned bufSize)
	{

	}

	void YumeD3D11ShaderVariation::SaveByteCode(const YumeString& binaryShaderName)
	{
	}

	void YumeD3D11ShaderVariation::CalculateConstantBufferSizes()
	{

	}
}
