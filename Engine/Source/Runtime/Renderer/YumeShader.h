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
#ifndef __YumeShader_h__
#define __YumeShader_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "YumeResource.h"

#include "YumeRendererDefs.h"
#include <boost/shared_ptr.hpp>


//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeShaderVariation;

	class YumeAPIExport YumeShader : public YumeResource
	{
	public:
		YumeShader();

		virtual ~YumeShader();

		virtual bool BeginLoad(YumeFile& source) = 0;

		virtual bool EndLoad() = 0;

		/// Return a variation with defines.
		virtual YumeShaderVariation* GetVariation(ShaderType type,const YumeString& defines) = 0;
		/// Return a variation with defines.
		virtual YumeShaderVariation* GetVariation(ShaderType type,const char* defines) = 0;

		virtual void RefreshMemoryUse() = 0;

		/// Return either vertex or pixel shader source code.
		const YumeString& GetSourceCode(ShaderType type) const { return type == VS ? vsSourceCode_ : psSourceCode_; }

		/// Return the latest timestamp of the shader code and its includes.
		unsigned GetTimeStamp() const { return timeStamp_; }

	protected:
		/// Process source code and include files. Return true if successful.
		bool ProcessSource(YumeString& code,YumeFile& file);
		/// Sort the defines and strip extra spaces to prevent creation of unnecessary duplicate shader variations.
		YumeString NormalizeDefines(const YumeString& defines);
		/// Recalculate the memory used by the shader.
		
	protected:
		/// Source code adapted for vertex shader.
		YumeString vsSourceCode_;
		/// Source code adapted for pixel shader.
		YumeString psSourceCode_;
		/// Vertex shader variations.
		/// Source code timestamp.
		unsigned timeStamp_;
		/// Number of unique variations so far.
		unsigned numVariations_;

	public:
		static YumeHash GetType();
		static YumeHash shaderHash_;
	};
}


//----------------------------------------------------------------------------
#endif
