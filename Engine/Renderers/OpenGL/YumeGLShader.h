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
#ifndef __YumeD3D11Shader_h__
#define __YumeD3D11Shader_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"

#include "Renderer/YumeShader.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGLShaderVariation;
	class YumeGLExport YumeGLShader : public YumeShader
	{
	public:
		YumeGLShader();
		virtual ~YumeGLShader();

		virtual bool BeginLoad(YumeFile& source);

		virtual bool EndLoad();

		/// Return a variation with defines.
		virtual YumeShaderVariation* GetVariation(ShaderType type,const YumeString& defines);
		/// Return a variation with defines.
		virtual YumeShaderVariation* GetVariation(ShaderType type,const char* defines);

		virtual void RefreshMemoryUse() ;

		YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> >::type vsVariations_;
		/// Pixel shader variations.
		YumeMap<YumeHash,SharedPtr<YumeGLShaderVariation> >::type psVariations_;
	};
}


//----------------------------------------------------------------------------
#endif
