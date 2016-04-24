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
#ifndef __RenderPass_h__
#define __RenderPass_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "RenderCall.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport RenderPass : public YumeBase
	{
	public:
		RenderPass();
		virtual ~RenderPass();

		void AddRenderCall(RenderCall* call);
		void RemoveRenderCall(RenderCall* call);

		RenderCallPtr GetRenderCall(unsigned index) const { return calls_[index]; }

		RenderCallPtr GetCallByName(const YumeString& name);

		void SetShaderParameter(YumeHash param,const Variant& variant);

		typedef YumeVector<RenderCall*> RenderCalls;
		RenderCalls::type calls_;
	};
}


//----------------------------------------------------------------------------
#endif

