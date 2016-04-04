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

		virtual YumeShaderVariation* GetVariation(ShaderType type,const YumeString& defines) = 0;
		virtual YumeShaderVariation* GetVariation(ShaderType type,const char* defines) = 0;
		virtual void RefreshMemoryUse() = 0;

		const YumeString& GetSourceCode(ShaderType type) const
		{
			if(type == VS)
				return vsSourceCode_;
			if(type == PS)
				return psSourceCode_;
			if(type == GS)
				return gsSourceCode_;
		}		
		unsigned GetTimeStamp() const { return timeStamp_; }

	protected:
		bool ProcessSource(YumeString& code,YumeFile& file);
		YumeString NormalizeDefines(const YumeString& defines);	
	protected:	
		YumeString vsSourceCode_;	
		YumeString psSourceCode_;
		YumeString gsSourceCode_;

		unsigned timeStamp_;	
		unsigned numVariations_;

	public:
		static YumeHash type_;
		static YumeHash GetTypeStatic() { return type_; }
		virtual YumeHash GetType() { return type_; }
	};
}


//----------------------------------------------------------------------------
#endif
