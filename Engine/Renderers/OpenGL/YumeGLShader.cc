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
#include "YumeGLShader.h"

#include "Renderer/YumeRHI.h"
#include "YumeGLRenderer.h"
#include "YumeGLShaderVariation.h"

#include "Core/YumeDefaults.h"

#include "Core/YumeIO.h"
#include "Engine/YumeEngine.h"

#include <boost/algorithm/string.hpp>

namespace YumeEngine
{
	void CommentOutFunction(YumeString& code,const YumeString& signature)
	{
		unsigned startPos = code.find(signature);
		unsigned braceLevel = 0;
		if(startPos == std::string::npos)
			return;

		code.insert(startPos,"/*");

		for(unsigned i = startPos + 2 + signature.length(); i < code.length(); ++i)
		{
			if(code[i] == '{')
				++braceLevel;
			else if(code[i] == '}')
			{
				--braceLevel;
				if(braceLevel == 0)
				{
					code.insert(i + 1,"*/");
					return;
				}
			}
		}
	}
	YumeGLShader::YumeGLShader()
	{
	}

	YumeGLShader::~YumeGLShader()
	{
	}

	bool YumeGLShader::BeginLoad(YumeFile& source)
	{
		YumeRHI* graphics_ = YumeEngine3D::Get()->GetRenderer();

		if(!graphics_)
			return false;

		timeStamp_ = 0;
		YumeString shaderCode;
		if(!ProcessSource(shaderCode,source))
			return false;

		vsSourceCode_ = shaderCode;
		psSourceCode_ = shaderCode;
		CommentOutFunction(vsSourceCode_,"void PS(");
		CommentOutFunction(psSourceCode_,"void VS(");

		boost::replace_all(vsSourceCode_,"void VS(","void main(");
		boost::replace_all(psSourceCode_,"void PS(","void main(");



		RefreshMemoryUse();
		return true;
	}

	bool YumeGLShader::EndLoad()
	{
		for(YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> >::iterator i = vsVariations_.begin(); i != vsVariations_.end(); ++i)
			i->second->Release();
		for(YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> >::iterator i = psVariations_.begin(); i != psVariations_.end(); ++i)
			i->second->Release();

		return true;
	}

	YumeShaderVariation* YumeGLShader::GetVariation(ShaderType type,const YumeString& defines)
	{
		return GetVariation(type,defines.c_str());
	}

	YumeShaderVariation* YumeGLShader::GetVariation(ShaderType type,const char* defines)
	{
		YumeHash definesHash = (defines);
		typedef YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> > ShaderMap;
		ShaderMap::type& variations = (type == VS ? vsVariations_ : psVariations_);

		ShaderMap::iterator i = variations.find(definesHash);

		if(i == variations.end())
		{
			// If shader not found, normalize the defines (to prevent duplicates) and check again. In that case make an alias
			// so that further queries are faster
			YumeString normalizedDefines = NormalizeDefines(defines);
			YumeHash normalizedHash = (normalizedDefines);

			i = variations.find(normalizedHash);
			if(i != variations.end())
				variations.insert(std::make_pair(definesHash,i->second));
			else
			{
				std::pair<YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> >::iterator,bool> ret;
				// No shader variation found. Create new
				YumeGLShaderVariation* shr_ = YumeAPINew YumeGLShaderVariation(this,type);
				ret = variations.insert(ShaderMap::type::value_type(normalizedHash,SharedPtr<YumeGLShaderVariation>(shr_)));
				if(definesHash != normalizedHash)
					variations.insert(std::make_pair(definesHash,ret.first->second));



				ret.first->second->SetName(GetFileName(GetName()));
				ret.first->second->SetDefines(normalizedDefines);
				++numVariations_;
				RefreshMemoryUse();

				return boost::static_pointer_cast<YumeShaderVariation>(ret.first->second).get();
			}
		}
		return (YumeShaderVariation*)i->second.get();
	}

	void YumeGLShader::RefreshMemoryUse()
	{
		SetMemoryUsage(
			(unsigned)(sizeof(YumeGLShader) + vsSourceCode_.length() + psSourceCode_.length() + numVariations_ * sizeof(YumeGLShaderVariation)));
	}
}
