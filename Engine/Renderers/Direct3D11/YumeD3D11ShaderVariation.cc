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
#include "YumeD3D11RendererImpl.h"
#include "YumeD3D11Renderer.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeDefaults.h"

#include "Logging/logging.h"

#include "Renderer/YumeResourceManager.h"

#include "YumeD3D11VertexBuffer.h"
#include "Renderer/YumeRHI.h"

#include <d3dcompiler.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>


namespace YumeEngine
{
	YumeD3D11ShaderVariation::YumeD3D11ShaderVariation(YumeShader* owner,ShaderType type)
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
		Release();
	}

	void YumeD3D11ShaderVariation::Release()
	{
		if(object_)
		{
			if(!gYume->pRHI)
				return;

			static_cast<YumeD3D11Renderer*>(gYume->pRHI)->CleanupShaderPrograms(this);

			if(type_ == VS)
			{
				if(gYume->pRHI->GetVertexShader() == this)
					gYume->pRHI->SetShaders(0,0);
			}
			else
			{
				if(gYume->pRHI->GetPixelShader() == this)
					gYume->pRHI->SetShaders(0,0);
			}

			D3D_SAFE_RELEASE(object_);
		}

		compilerOutput_.clear();

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			useTextureUnit_[i] = false;
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			constantBufferSizes_[i] = 0;
		parameters_.clear();
		byteCode_.clear();
		elementMask_ = 0;
	}

	bool YumeD3D11ShaderVariation::Create()
	{
		Release();

		if(!gYume->pRHI)
			return false;

		if(!owner_)
		{
			compilerOutput_ = "Owner shader has expired";
			return false;
		}

		YumeString path,name,extension;
		SplitPath(owner_->GetName(),path,name,extension);
		extension = type_ == VS ? ".vs4" : ".ps4";

		YumeString binaryShaderName = path + "Cache/" + name + "_" + (defines_) + extension;

		if(!Compile())
			return false;
		/*if(!LoadByteCode(binaryShaderName))
		{
			if(!Compile())
				return false;

			if(owner_->GetTimeStamp())
				SaveByteCode(binaryShaderName);
		}*/

		// Then create shader from the bytecode
		ID3D11Device* device = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDevice();
		if(type_ == VS)
		{
			if(device && byteCode_.size())
			{
				HRESULT hr = device->CreateVertexShader(&byteCode_[0],byteCode_.size(),0,(ID3D11VertexShader**)&object_);
				if(FAILED(hr))
				{
					D3D_SAFE_RELEASE(object_);
					compilerOutput_ = "Could not create vertex shader (HRESULT " + std::to_string(hr) + ")";
				}
			}
			else
				compilerOutput_ = "Could not create vertex shader, empty bytecode";
		}
		else
		{
			if(device && byteCode_.size())
			{
				HRESULT hr = device->CreatePixelShader(&byteCode_[0],byteCode_.size(),0,(ID3D11PixelShader**)&object_);
				if(FAILED(hr))
				{
					D3D_SAFE_RELEASE(object_);
					compilerOutput_ = "Could not create pixel shader (HRESULT " + std::to_string(hr) + ")";
				}
			}
			else
				compilerOutput_ = "Could not create pixel shader, empty bytecode";
		}

		return object_ != 0;
	}

	bool YumeD3D11ShaderVariation::LoadByteCode(const YumeString& binaryShaderName)
	{
		YumeIO* io_ = gYume->pIO;
		YumeResourceManager* rm_ = gYume->pResourceManager;
		if(!rm_->Exists(binaryShaderName))
			return false;

		unsigned sourceTimeStamp = owner_->GetTimeStamp();

		if(sourceTimeStamp && io_->GetLastModifiedTime(rm_->GetFullPath(binaryShaderName)) < sourceTimeStamp)
			return false;

		SharedPtr<YumeFile> file_ = rm_->GetFile(binaryShaderName);

		if(!file_ || file_->GetFileExtension() != "USHD")
		{
			YUMELOG_ERROR(binaryShaderName + " is not a valid shader bytecode file");
			return false;
		}
		file_->ReadUShort();
		file_->ReadUShort();
		elementMask_ = file_->ReadUInt();

		unsigned numParameters = file_->ReadUInt();
		for(unsigned i = 0; i < numParameters; ++i)
		{
			YumeString name = file_->ReadString();
			unsigned buffer = file_->ReadUByte();
			unsigned offset = file_->ReadUInt();
			unsigned size = file_->ReadUInt();

			ShaderParameter parameter(type_,name_,buffer,offset,size);
			parameters_[(name)] = parameter;
		}

		unsigned numTextureUnits = file_->ReadUInt();
		for(unsigned i = 0; i < numTextureUnits; ++i)
		{
			/*String unitName = */file_->ReadString();
			unsigned reg = file_->ReadUByte();

			if(reg < MAX_TEXTURE_UNITS)
				useTextureUnit_[reg] = true;
		}

		unsigned byteCodeSize = file_->ReadUInt();
		if(byteCodeSize)
		{
			byteCode_.resize(byteCodeSize);
			file_->Read(&byteCode_[0],byteCodeSize);

			if(type_ == VS)
				YUMELOG_DEBUG("Loaded cached vertex shader " + GetFullName());
			else
				YUMELOG_DEBUG("Loaded cached pixel shader " + GetFullName());

			CalculateConstantBufferSizes();
			return true;
		}
		else
		{
			YUMELOG_DEBUG(binaryShaderName + " has zero length bytecode");
			return false;
		}

		return false;
	}

	bool YumeD3D11ShaderVariation::Compile()
	{
		const YumeString& sourceCode = owner_->GetSourceCode(type_);
		std::vector<YumeString> defines;

		boost::split(defines,defines_,boost::is_any_of(" "));

		const char* entryPoint = 0;
		const char* profile = 0;
		unsigned flags = 0;

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
			flags |= 0;
		}

		flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
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

		const char* str = owner_->GetName().c_str();

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
			//ID3DBlob* strippedCode = 0;
			//D3DStripShader(bufData,bufSize,
			//	D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS,&strippedCode);
			//byteCode_.resize((unsigned)strippedCode->GetBufferSize());
			//memcpy(&byteCode_[0],strippedCode->GetBufferPointer(),byteCode_.size());
			//strippedCode->Release();

			byteCode_.resize(bufSize);
			memcpy(&byteCode_[0],bufData,byteCode_.size());
		}

		D3D_SAFE_RELEASE(shaderCode);
		D3D_SAFE_RELEASE(errorMsgs);

		return !byteCode_.empty();;
	}

	void YumeD3D11ShaderVariation::ParseParameters(unsigned char* bufData,unsigned bufSize)
	{
		ID3D11ShaderReflection* reflection = 0;
		D3D11_SHADER_DESC shaderDesc;

		HRESULT hr = D3DReflect(bufData,bufSize,IID_ID3D11ShaderReflection,(void**)&reflection);
		if(FAILED(hr) || !reflection)
		{
			D3D_SAFE_RELEASE(reflection);
			YUMELOG_ERROR("Failed to reflect vertex shader's input signature " << hr);
			return;
		}

		reflection->GetDesc(&shaderDesc);

		if(type_ == VS)
		{
			for(unsigned i = 0; i < shaderDesc.InputParameters; ++i)
			{
				D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
				reflection->GetInputParameterDesc((UINT)i,&paramDesc);
				for(unsigned j = 0; j < MAX_VERTEX_ELEMENTS; ++j)
				{
					if(strcmp(paramDesc.SemanticName,YumeD3D11VertexBuffer::elementSemantics[j]) == 0 &&
						paramDesc.SemanticIndex == YumeD3D11VertexBuffer::elementSemanticIndices[j])
					{
						elementMask_ |= (1 << j);
						break;
					}
				}
			}
		}

		YumeMap<YumeString,unsigned>::type cbRegisterMap;

		for(unsigned i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
			reflection->GetResourceBindingDesc(i,&resourceDesc);
			YumeString resourceName(resourceDesc.Name);
			if(resourceDesc.Type == D3D_SIT_CBUFFER)
				cbRegisterMap[resourceName] = resourceDesc.BindPoint;
			else if(resourceDesc.Type == D3D_SIT_SAMPLER && resourceDesc.BindPoint < MAX_TEXTURE_UNITS)
				useTextureUnit_[resourceDesc.BindPoint] = true;
		}

		for(unsigned i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* cb = reflection->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC cbDesc;
			cb->GetDesc(&cbDesc);
			unsigned cbRegister = cbRegisterMap[YumeString(cbDesc.Name)];

			for(unsigned j = 0; j < cbDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				var->GetDesc(&varDesc);
				YumeString varName(varDesc.Name);
				if(varName[0] == 'c')
				{
					varName = varName.substr(1);
					parameters_[YumeHash(varName)] = ShaderParameter(type_,varName,cbRegister,varDesc.StartOffset,varDesc.Size);
				}
			}
		}

		reflection->Release();
	}

	void YumeD3D11ShaderVariation::SaveByteCode(const YumeString& binaryShaderName)
	{
		YumeIO* io_ = gYume->pIO;
		YumeResourceManager* rm_ = gYume->pResourceManager;

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

		file_->WriteUInt(parameters_.size());
		for(EastlHashMap<YumeHash,ShaderParameter>::const_iterator i = parameters_.begin(); i != parameters_.end(); ++i)
		{
			file_->WriteString(i->second.name_);
			file_->WriteUByte((unsigned char)i->second.buffer_);
			file_->WriteUInt(i->second.offset_);
			file_->WriteUInt(i->second.size_);
		}

		unsigned usedTextureUnits = 0;
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			if(useTextureUnit_[i])
				++usedTextureUnits;
		}
		file_->WriteUInt(usedTextureUnits);
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			if(useTextureUnit_[i])
			{
				file_->WriteString(gYume->pRHI->GetTextureUnitName((TextureUnit)i));
				file_->WriteUByte((unsigned char)i);
			}
		}

		file_->WriteUInt(byteCode_.size());
		if(byteCode_.size())
			file_->Write(&byteCode_[0],byteCode_.size());

	}

	void YumeD3D11ShaderVariation::CalculateConstantBufferSizes()
	{
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			constantBufferSizes_[i] = 0;

		for(EastlHashMap<YumeHash,ShaderParameter>::const_iterator i = parameters_.begin(); i != parameters_.end(); ++i)
		{
			if(i->second.buffer_ < MAX_SHADER_PARAMETER_GROUPS)
			{
				unsigned oldSize = constantBufferSizes_[i->second.buffer_];
				unsigned paramEnd = i->second.offset_ + i->second.size_;
				if(paramEnd > oldSize)
					constantBufferSizes_[i->second.buffer_] = paramEnd;
			}
		}
	}
}
