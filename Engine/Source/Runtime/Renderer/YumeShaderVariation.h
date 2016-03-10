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
	struct ShaderParameter;
	
	class YumeAPIExport YumeShaderVariation
	{
	public:
		
		YumeShaderVariation();
		
		virtual ~YumeShaderVariation();

		virtual void Release() { };

		
		virtual bool Create() = 0;
		
		void SetName(const YumeString& name);
		
		void SetDefines(const YumeString& defines) ;

		
		YumeShader* GetOwner() const;

		unsigned GetElementMask() const { return elementMask_; }

		
		ShaderType GetShaderType() const { return type_; }

		
		const YumeString& GetName() const { return name_; }

		
		const YumeString& GetDefines() const { return defines_; }

		
		YumeString GetFullName() const { return name_ + "(" + defines_ + ")"; }

		
		const YumeString& GetCompilerOutput() const { return compilerOutput_; }

		bool HasTextureUnit(TextureUnit unit) const { return useTextureUnit_[unit]; }

		virtual const YumeMap<YumeHash,ShaderParameter>::type& GetParameters() const = 0;

		virtual const unsigned* GetConstantBufferSizes() const = 0;

		const YumeVector<unsigned char>::type& GetByteCode() const { return byteCode_; }

	protected:
		
		YumeShader* owner_;
		
		ShaderType type_;
		
		YumeString name_;
		
		YumeString defines_;
		
		YumeString compilerOutput_;

		unsigned elementMask_;

		bool useTextureUnit_[MAX_TEXTURE_UNITS];

		YumeVector<unsigned char>::type byteCode_;
	};
}


//----------------------------------------------------------------------------
#endif
