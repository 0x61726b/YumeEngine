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
#include "YumeD3D11Shader.h"

#include "Renderer/YumeRHI.h"
#include "YumeD3D11Renderer.h"
#include "YumeD3D11ShaderVariation.h"

#include "Core/YumeDefaults.h"

#include "Core/YumeIO.h"
#include "Engine/YumeEngine.h"

namespace YumeEngine
{
	void CommentOutFunction(YumeString& code,const YumeString& signature)
	{
		unsigned startPos = code.find(signature);
		unsigned braceLevel = 0;
		if(startPos == M_MAX_UNSIGNED)
			return;

		code.Insert(startPos,"/*");

		for(unsigned i = startPos + 2 + signature.length(); i < code.length(); ++i)
		{
			if(code[i] == '{')
				++braceLevel;
			else if(code[i] == '}')
			{
				--braceLevel;
				if(braceLevel == 0)
				{
					code.Insert(i + 1,"*/");
					return;
				}
			}
		}
	}

	YumeHash YumeD3D11Shader::type_ = "D3D11Shader";
	YumeD3D11Shader::YumeD3D11Shader()
	{
	}

	YumeD3D11Shader::~YumeD3D11Shader()
	{
	}

	bool YumeD3D11Shader::BeginLoad(YumeFile& source)
	{
		YumeRHI* graphics_ = gYume->pRHI;

		if(!graphics_)
			return false;

		timeStamp_ = 0;
		YumeString shaderCode;
		if(!ProcessSource(shaderCode,source))
			return false;

		vsSourceCode_ = shaderCode;
		psSourceCode_ = shaderCode;
		gsSourceCode_ = shaderCode;

		CommentOutFunction(vsSourceCode_,"void PS(");
		CommentOutFunction(vsSourceCode_,"void GS(");

		CommentOutFunction(psSourceCode_,"void VS(");
		CommentOutFunction(psSourceCode_,"VSOutput VS(");
		CommentOutFunction(psSourceCode_,"void GS(");

		CommentOutFunction(gsSourceCode_,"void VS(");
		CommentOutFunction(gsSourceCode_,"void PS(");


		RefreshMemoryUse();
		return true;
	}

	bool YumeD3D11Shader::EndLoad()
	{
		for(YumeMap<YumeHash,SharedPtr<YumeD3D11ShaderVariation> >::iterator i = vsVariations_.begin(); i != vsVariations_.end(); ++i)
			i->second->Release();
		for(YumeMap<YumeHash,SharedPtr<YumeD3D11ShaderVariation> >::iterator i = psVariations_.begin(); i != psVariations_.end(); ++i)
			i->second->Release();
		for(YumeMap<YumeHash,SharedPtr<YumeD3D11ShaderVariation> >::iterator i = gsVariations_.begin(); i != gsVariations_.end(); ++i)
			i->second->Release();

		return true;
	}

	YumeShaderVariation* YumeD3D11Shader::GetVariation(ShaderType type,const YumeString& defines,const YumeString& entryPoint)
	{
		return GetVariation(type,defines.c_str(),entryPoint);
	}

	YumeShaderVariation* YumeD3D11Shader::GetVariation(ShaderType type,const char* defines,const YumeString& entryPoint)
	{
		YumeHash definesHash = defines;
		typedef YumeMap<YumeHash,SharedPtr<YumeD3D11ShaderVariation> > ShaderMap;


		if(type == VS)
		{
			ShaderMap::iterator i = vsVariations_.find(definesHash);
			if(i == vsVariations_.end())
			{
				YumeString normalizedDefines = NormalizeDefines(defines);
				YumeHash normalizedHash = (normalizedDefines);

				i = vsVariations_.find(normalizedHash);
				if(i != vsVariations_.end())
					vsVariations_.insert(MakePair(definesHash,i->second));
				else
				{
					YumeD3D11ShaderVariation* shr = (YumeAPINew YumeD3D11ShaderVariation(this,type));
					i = vsVariations_.insert(MakePair(normalizedHash,SharedPtr<YumeD3D11ShaderVariation>(shr)));
					if(definesHash != normalizedHash)
						vsVariations_.insert(MakePair(definesHash,i->second));

					i->second->SetName(GetFileName(GetName()));
					i->second->SetDefines(normalizedDefines);
					i->second->SetEntryPoint(entryPoint);
					++numVariations_;
					RefreshMemoryUse();

					return StaticCast<YumeShaderVariation>(i->second);
				}
			}
			else
				return (YumeShaderVariation*)i->second;
		}
		if(type == PS)
		{
			ShaderMap::iterator i = psVariations_.find(definesHash);
			if(i == psVariations_.end())
			{
				YumeString normalizedDefines = NormalizeDefines(defines);
				YumeHash normalizedHash = (normalizedDefines);

				i = psVariations_.find(normalizedHash);
				if(i != psVariations_.end())
					psVariations_.insert(MakePair(definesHash,i->second));
				else
				{
					YumeD3D11ShaderVariation* shr = (YumeAPINew YumeD3D11ShaderVariation(this,type));
					i = psVariations_.insert(MakePair(normalizedHash,SharedPtr<YumeD3D11ShaderVariation>(shr)));
					if(definesHash != normalizedHash)
						psVariations_.insert(MakePair(definesHash,i->second));

					i->second->SetName(GetFileName(GetName()));
					i->second->SetDefines(normalizedDefines);
					i->second->SetEntryPoint(entryPoint);
					++numVariations_;
					RefreshMemoryUse();

					return StaticCast<YumeShaderVariation>(i->second);
				}
			}
			else
				return (YumeShaderVariation*)i->second;
		}
		if(type == GS)
		{
			ShaderMap::iterator i = gsVariations_.find(definesHash);
			if(i == gsVariations_.end())
			{
				YumeString normalizedDefines = NormalizeDefines(defines);
				YumeHash normalizedHash = (normalizedDefines);

				i = gsVariations_.find(normalizedHash);
				if(i != gsVariations_.end())
					gsVariations_.insert(MakePair(definesHash,i->second));
				else
				{
					YumeD3D11ShaderVariation* shr = (YumeAPINew YumeD3D11ShaderVariation(this,type));
					i = gsVariations_.insert(MakePair(normalizedHash,SharedPtr<YumeD3D11ShaderVariation>(shr)));
					if(definesHash != normalizedHash)
						gsVariations_.insert(MakePair(definesHash,i->second));

					i->second->SetName(GetFileName(GetName()));
					i->second->SetDefines(normalizedDefines);
					i->second->SetEntryPoint(entryPoint);
					++numVariations_;
					RefreshMemoryUse();

					return StaticCast<YumeShaderVariation>(i->second);
				}
			}
			else
				return (YumeShaderVariation*)i->second;
		}
	}

	void YumeD3D11Shader::RefreshMemoryUse()
	{
		SetMemoryUsage(
			(unsigned)(sizeof(YumeD3D11Shader) + vsSourceCode_.length() + psSourceCode_.length() + gsSourceCode_.length() +  numVariations_ * sizeof(YumeD3D11ShaderVariation)));
	}
}
