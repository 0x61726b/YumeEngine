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

#define CBUFFER_CONSTANTS_HAVE_NOT_C_ON_FRONT

namespace YumeEngine
{
	struct include_handler : public ID3D10Include
	{
		std::string path;

		include_handler(const std::string& filename)
		{
			path = filename;
			auto it = path.find_last_of('/');

			if(it != std::string::npos)
				path = path.substr(0,it+1);
			else
				path = "";
		}

		STDMETHOD(Open)(D3D10_INCLUDE_TYPE IncludeType,LPCSTR pFileName,LPCVOID pParentData,LPCVOID *ppData,UINT *pByteLen)
		{
			std::ifstream f(path + pFileName);
			std::string fstr((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());

			*pByteLen = static_cast<UINT>(fstr.length());
			char* data = new char[*pByteLen];
			memcpy(data,fstr.c_str(),*pByteLen);
			*ppData = data;

			return S_OK;
		}

		STDMETHOD(Close)(LPCVOID pData)
		{
			char* data = (char*)pData;
			delete[] data;
			return S_OK;
		}
	};

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
			else if(type_ == PS)
			{
				if(gYume->pRHI->GetPixelShader() == this)
					gYume->pRHI->SetShaders(0,0);
			}
			else if(type_ == GS)
			{
				if(gYume->pRHI->GetGeometryShader() == this)
					gYume->pRHI->SetShaders(0,0);
			}

			D3D_SAFE_RELEASE(object_);
		}

		compilerOutput_.Clear();

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

		YumeString binaryShaderName = path + "Cache/" + name + "_" + (defines_)+extension;

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
					compilerOutput_ = "Could not create vertex shader (HRESULT " + String(hr) + ")";
				}
				else
					((ID3D11VertexShader*)object_)->SetPrivateData(WKPDID_D3DDebugObjectName,GetName().length(),GetName().c_str());
			}
			else
				compilerOutput_ = "Could not create vertex shader, empty bytecode";
		}
		else if(type_ == PS)
		{
			if(device && byteCode_.size())
			{
				HRESULT hr = device->CreatePixelShader(&byteCode_[0],byteCode_.size(),0,(ID3D11PixelShader**)&object_);
				if(FAILED(hr))
				{
					D3D_SAFE_RELEASE(object_);
					compilerOutput_ = "Could not create pixel shader (HRESULT " + String(hr) + ")";
				}
				else
					((ID3D11PixelShader*)object_)->SetPrivateData(WKPDID_D3DDebugObjectName,GetName().length(),GetName().c_str());
			}
			else
				compilerOutput_ = "Could not create pixel shader, empty bytecode";
		}
		else if(type_ == GS)
		{
			if(device && byteCode_.size())
			{
				HRESULT hr = device->CreateGeometryShader(&byteCode_[0],byteCode_.size(),0,(ID3D11GeometryShader**)&object_);
				if(FAILED(hr))
				{
					D3D_SAFE_RELEASE(object_);
					compilerOutput_ = "Could not create GEOMETRY shader (HRESULT " + String(hr) + ")";
				}
				else
					((ID3D11GeometryShader*)object_)->SetPrivateData(WKPDID_D3DDebugObjectName,GetName().length(),GetName().c_str());
			}
			else
				compilerOutput_ = "Could not create GEOMETRY shader, empty bytecode";
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
			YUMELOG_ERROR(binaryShaderName.c_str() << " is not a valid shader bytecode file");
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
				YUMELOG_DEBUG("Loaded cached vertex shader " << GetFullName().c_str());
			else
				YUMELOG_DEBUG("Loaded cached pixel shader " << GetFullName().c_str());

			CalculateConstantBufferSizes();
			return true;
		}
		else
		{
			YUMELOG_DEBUG(binaryShaderName.c_str() << " has zero length bytecode");
			return false;
		}

		return false;
	}

	bool YumeD3D11ShaderVariation::Compile()
	{
		const YumeString& sourceCode = owner_->GetSourceCode(type_);
		YumeVector<YumeString>::type defines = defines_.Split(' ');

		const char* entryPoint = 0;
		const char* profile = 0;
		unsigned flags = 0;

		defines.push_back("D3D11");

		if(type_ == VS)
		{
			entryPoint = "VS";
			defines.push_back("COMPILEVS");
			profile = "vs_5_0";
		}
		else if(type_ == PS)
		{
			entryPoint = "PS";
			defines.push_back("COMPILEPS");
			profile = "ps_5_0";
			flags |= 0;
		}
		else if(type_ == GS)
		{
			entryPoint = "GS";
			defines.push_back("COMPILEGS");
			profile = "gs_5_0";
			flags |= 0;
		}
		
		if(shaderEntry.length() > 0)
			entryPoint = shaderEntry.c_str();

		flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
		YUMELOG_INFO("Compiling shader " << GetFullName().c_str());

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
				YUMELOG_WARN("Shader " << GetFullName().c_str() << " does not use the define " << defines[i].c_str());
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

		include_handler ih(gYume->pResourceManager->GetFullPath(str).c_str());
		HRESULT hr = D3DCompile(sourceCode.c_str(),sourceCode.length(),owner_->GetName().c_str(),&macros.front(),&ih,
			entryPoint,profile,flags,0,&shaderCode,&errorMsgs);
		if(FAILED(hr))
		{
			// Do not include end zero unnecessarily
			compilerOutput_ = YumeString((const char*)errorMsgs->GetBufferPointer(),(unsigned)errorMsgs->GetBufferSize() - 1);
		}
		else
		{
			if(type_ == VS)
				YUMELOG_DEBUG("Compiled vertex shader " << GetFullName().c_str());
			if(type_ == PS)
				YUMELOG_DEBUG("Compiled pixel shader " << GetFullName().c_str());
			if(type_ == GS)
				YUMELOG_DEBUG("Compiled geometry shader " << GetFullName().c_str());

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
#ifndef CBUFFER_CONSTANTS_HAVE_NOT_C_ON_FRONT
				if(varName[0] == 'c')
				{
					varName = varName.substr(1);
					parameters_[YumeHash(varName)] = ShaderParameter(type_,varName,cbRegister,varDesc.StartOffset,varDesc.Size);
				}
#else
				parameters_[YumeHash(varName)] = ShaderParameter(type_,varName,cbRegister,varDesc.StartOffset,varDesc.Size);
#endif
			}
		}

		reflection->Release();
	}

	void YumeD3D11ShaderVariation::SaveByteCode(const YumeString& binaryShaderName)
	{
		

	}

	void YumeD3D11ShaderVariation::CalculateConstantBufferSizes()
	{
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			constantBufferSizes_[i] = 0;

		for(YumeMap<YumeHash,ShaderParameter>::const_iterator i = parameters_.begin(); i != parameters_.end(); ++i)
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
