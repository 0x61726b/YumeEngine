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
#include "YumeGLShaderVariation.h"
#include "Renderer/YumeShader.h"
#include "Core/YumeFile.h"
#include "Core/YumeIO.h"

#include "Renderer/YumeRendererDefs.h"
#include "YumeGLRendererImpl.h"
#include "YumeGLRenderer.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeDefaults.h"

#include "Logging/logging.h"

#include "Renderer/YumeResourceManager.h"

#include "YumeGLVertexBuffer.h"
#include "Renderer/YumeRHI.h"


#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

namespace YumeEngine
{
	YumeGLShaderVariation::YumeGLShaderVariation(YumeShader* owner,ShaderType type)
	{
		owner_ = owner;
		type_ = type;
	}

	YumeGLShaderVariation::~YumeGLShaderVariation()
	{
		Release();
	}

	void YumeGLShaderVariation::OnDeviceLost()
	{
		YumeGLResource::OnDeviceLost();

		compilerOutput_.Clear();
	}

	void YumeGLShaderVariation::Release()
	{
		if(object_)
		{
			if(!gYume->pRHI)
				return;

			if(!gYume->pRHI->IsDeviceLost())
			{
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

				glDeleteShader(object_);
			}

			object_ = 0;
			gYume->pRHI->CleanupShaderPrograms(this);
		}

		compilerOutput_.Clear();
	}

	bool YumeGLShaderVariation::Create()
	{
		Release();

		if(!owner_)
		{
			compilerOutput_ = "Owner shader has expired";
			return false;
		}

		object_ = glCreateShader(type_ == VS ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
		if(!object_)
		{
			compilerOutput_ = "Could not create shader object";
			return false;
		}

		const YumeString& originalShaderCode = owner_->GetSourceCode(type_);
		YumeString shaderCode;

		// Check if the shader code contains a version define
		unsigned verStart = originalShaderCode.find('#');
		unsigned verEnd = 0;
		if(verStart != std::string::npos)
		{
			if(originalShaderCode.substr(verStart + 1,7) == "version")
			{
				verEnd = verStart + 9;
				while(verEnd < originalShaderCode.length())
				{
					if(IsDigit((unsigned)originalShaderCode[verEnd]))
						++verEnd;
					else
						break;
				}
				// If version define found, insert it first
				YumeString versionDefine = originalShaderCode.substr(verStart,verEnd - verStart);
				shaderCode += versionDefine + "\n";
			}
		}
		// Force GLSL version 150 if no version define and GL3 is being used
		if(!verEnd && YumeGLRenderer::GetGL3Support())
			shaderCode += "#version 150\n";

		// Distinguish between VS and PS compile in case the shader code wants to include/omit different things
		shaderCode += type_ == VS ? "#define COMPILEVS\n" : "#define COMPILEPS\n";

		// Add define for the maximum number of supported bones
		shaderCode += "#define MAXBONES " + String(YumeGLRenderer::GetMaxBones()) + "\n";

		// Prepend the defines to the shader code
		YumeVector<YumeString>::type defineVec = defines_.Split(' ');
		

		for(unsigned i = 0; i < defineVec.size(); ++i)
		{
			if(defineVec[i].length())
			{
				// Add extra space for the checking code below
				defineVec[i].Replace("="," ");
				YumeString defineString = "#define " + defineVec[i] + " \n";
				shaderCode += defineString;

#ifdef _DEBUG
				YumeString defineCheck = defineString.substr(8,defineString.find(' ',8) - 8);
				if(originalShaderCode.find(defineCheck) == std::string::npos)
					YUMELOG_WARN("Shader " << GetFullName().c_str() << " does not use the define " << defineCheck.c_str());
#endif
			}
			// In debug mode, check that all defines are referenced by the shader code

		}

		if(YumeGLRenderer::GetGL3Support())
			shaderCode += "#define GL3\n";

		// When version define found, do not insert it a second time
		if(verEnd > 0)
			shaderCode += (originalShaderCode.c_str() + verEnd);
		else
			shaderCode += originalShaderCode;

		const char* shaderCStr = shaderCode.c_str();
		glShaderSource(object_,1,&shaderCStr,0);
		glCompileShader(object_);

		int compiled,length;
		glGetShaderiv(object_,GL_COMPILE_STATUS,&compiled);
		if(!compiled)
		{
			glGetShaderiv(object_,GL_INFO_LOG_LENGTH,&length);
			compilerOutput_.resize(length);
			int outLength;
			glGetShaderInfoLog(object_,length,&outLength,&compilerOutput_[0]);
			glDeleteShader(object_);
			object_ = 0;
		}
		else
			compilerOutput_.Clear();

		return object_ != 0;
	}

}
