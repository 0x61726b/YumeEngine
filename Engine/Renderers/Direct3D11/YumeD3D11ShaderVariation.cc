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

#include "Logging/logging.h"

#include "Renderer/YumeResourceManager.h"


#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

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

		if(owner_->GetTimeStamp())
			SaveByteCode(binaryShaderName);

		return false;
	}

	bool YumeD3D11ShaderVariation::LoadByteCode(const YumeString& binaryShaderName)
	{
		return false;
	}

	bool YumeD3D11ShaderVariation::Compile()
	{
		const YumeString& sourceCode = owner_->GetSourceCode(type_);
		YumeVector<YumeString>::type defines;

		boost::split(defines,defines_,boost::is_any_of(" "));

		const char* entryPoint = 0;
		const char* profile = 0;
		unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

		defines.push_back("D3D11");

		if(type_ == VS)
		{
			entryPoint = "VS";
			defines.push_back("COMPILEVS");
			profile = "vs_4_0";
		}
		else
		{
			entryPoint = "PS";
			defines.push_back("COMPILEPS");
			profile = "ps_4_0";
			flags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
		}

		YUMELOG_INFO("Compiling shader " << GetFullName());

		YumeVector<YumeString>::type defineValues;
		YumeVector<D3D_SHADER_MACRO>::type macros;

		for(unsigned i = 0; i < defines.size(); ++i)
		{
			unsigned equalsPos = defines[i].find('=');
			if(equalsPos != 0xffffffff)
			{
				defineValues.push_back(defines[i].substr(equalsPos + 1));
				defines[i].resize(equalsPos);
			}
			else
				defineValues.push_back("1");
		}

		for(unsigned i = 0; i < defines.size(); ++i)
		{
			D3D_SHADER_MACRO macro;
			macro.Name = defines[i].c_str();
			macro.Definition = defineValues[i].c_str();
			macros.push_back(macro);

			// In debug mode, check that all defines are referenced by the shader code
#ifdef _DEBUG
			if(sourceCode.find(defines[i]) == 0xffffffff)
				YUMELOG_WARN("Shader " + GetFullName() + " does not use the define " + defines[i]);
#endif
		}

		D3D_SHADER_MACRO endMacro;
		endMacro.Name = 0;
		endMacro.Definition = 0;
		macros.push_back(endMacro);

		// Compile using D3DCompile
		ID3DBlob* shaderCode = 0;
		ID3DBlob* errorMsgs = 0;

		HRESULT hr = D3DCompile(sourceCode.c_str(),sourceCode.length(),owner_->GetName().c_str(),&macros.front(),0,
			entryPoint,profile,flags,0,&shaderCode,&errorMsgs);
		if(FAILED(hr))
		{
			// Do not include end zero unnecessarily
			compilerOutput_ = YumeString((const char*)errorMsgs->GetBufferPointer(),(unsigned)errorMsgs->GetBufferSize() - 1);
		}
		else
		{
			if(type_ == VS)
				YUMELOG_DEBUG("Compiled vertex shader " + GetFullName());
			else
				YUMELOG_DEBUG("Compiled pixel shader " + GetFullName());

			unsigned char* bufData = (unsigned char*)shaderCode->GetBufferPointer();
			unsigned bufSize = (unsigned)shaderCode->GetBufferSize();
			// Use the original bytecode to reflect the parameters
			ParseParameters(bufData,bufSize);
			CalculateConstantBufferSizes();

			// Then strip everything not necessary to use the shader
			ID3DBlob* strippedCode = 0;
			D3DStripShader(bufData,bufSize,
				D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS,&strippedCode);
			byteCode_.resize((unsigned)strippedCode->GetBufferSize());
			memcpy(&byteCode_[0],strippedCode->GetBufferPointer(),byteCode_.size());
			strippedCode->Release();
		}

		D3D_SAFE_RELEASE(shaderCode);
		D3D_SAFE_RELEASE(errorMsgs);

		return !byteCode_.empty();;
	}

	void YumeD3D11ShaderVariation::ParseParameters(unsigned char* bufData,unsigned bufSize)
	{

	}

	void YumeD3D11ShaderVariation::SaveByteCode(const YumeString& binaryShaderName)
	{
		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();
		YumeResourceManager* rm_ = YumeEngine3D::Get()->GetResourceManager();

		YumeString path = GetPath(rm_->GetFullPath(owner_->GetName())) + "Cache/";

		if(!io_->IsDirectoryExist(path))
			io_->CreateDir(path);

		YumeString p,file,extension;
		SplitPath(binaryShaderName,p,file,extension);
		YumeString fullName = p+ file + extension;



		SharedPtr<YumeFile> file_ = SharedPtr<YumeFile>(new YumeFile(path + file + extension,FILEMODE_WRITE));

		
		if(!file_)
			return;

		file_->WriteFileID("USHD");
		file_->WriteShort((unsigned short)type_);
		file_->WriteShort(4);
		file_->WriteUInt(elementMask_);

	}

	void YumeD3D11ShaderVariation::CalculateConstantBufferSizes()
	{

	}
}
