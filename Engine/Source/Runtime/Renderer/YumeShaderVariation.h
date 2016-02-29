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
#ifndef __YumeShaderVariation_h__
#define __YumeShaderVariation_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "YumeRendererDefs.h"
#include "YumeGpuResource.h"
#include "YumeShader.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeShader;
	/// Vertex or pixel shader on the GPU.
	class YumeAPIExport YumeShaderVariation
	{
	public:
		/// Construct.
		YumeShaderVariation();
		/// Destruct.
		virtual ~YumeShaderVariation();

		virtual void Release() { };

		/// Compile the shader. Return true if successful.
		virtual bool Create() = 0;
		/// Set name.
		void SetName(const YumeString& name);
		/// Set defines.
		void SetDefines(const YumeString& defines) ;

		/// Return the owner resource.
		YumeShader* GetOwner() const;

		/// Return shader type.
		ShaderType GetShaderType() const { return type_; }

		/// Return name.
		const YumeString& GetName() const { return name_; }

		/// Return defines.
		const YumeString& GetDefines() const { return defines_; }

		/// Return full shader name.
		YumeString GetFullName() const { return name_ + "(" + defines_ + ")"; }

		/// Return compile error/warning string.
		const YumeString& GetCompilerOutput() const { return compilerOutput_; }

		bool HasTextureUnit(TextureUnit unit) const { return useTextureUnit_[unit]; }

	protected:
		/// Shader this variation belongs to.
		YumeShader* owner_;
		/// Shader type.
		ShaderType type_;
		/// Shader name.
		YumeString name_;
		/// Defines to use in compiling.
		YumeString defines_;
		/// Shader compile error string.
		YumeString compilerOutput_;

		bool useTextureUnit_[MAX_TEXTURE_UNITS];
	};
}


//----------------------------------------------------------------------------
#endif
