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
#include "RenderPass.h"
#include "RenderCall.h"
#include "YumeRHI.h"


namespace YumeEngine
{
	RenderPass::RenderPass()
	{
	}

	RenderPass::~RenderPass()
	{
	}

	void RenderPass::AddRenderCall(RenderCall* call)
	{
		calls_.push_back(call);
	}

	void RenderPass::RemoveRenderCall(RenderCall* call)
	{
		calls_.Remove(call);
	}

	RenderCallPtr RenderPass::GetCallByName(const YumeString& name)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			RenderCallPtr c = calls_[i];

			if(!c->GetPassName().Compare(name))
				return c;
		}
		return 0;
	}

	void RenderPass::SetShaderParameter(YumeHash param,const Variant& variant)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(calls_[i]->ContainsParameter(param))
			{
				calls_[i]->SetShaderParameter(param,variant);
			}
		}
	}


}
