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
#include "YumePostProcess.h"

#include "YumeRHI.h"
#include "YumeMiscRenderer.h"

namespace YumeEngine
{
	YumePostProcess::YumePostProcess(YumeMiscRenderer* misc)
		: ssao_enabled(false),
		ssao_scale(1),
		misc_(misc)
	{
	}

	YumePostProcess::~YumePostProcess()
	{

	}

	void YumePostProcess::Setup()
	{
		temporaryRt_ = gYume->pRHI->CreateTexture2D();
		temporaryRt_->SetSize(gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight(),gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,1,10);

		ssaoPs_ = gYume->pRHI->GetShader(PS,"PostFX/pp_ssao");
	}

	void YumePostProcess::Render()
	{
		gYume->pRHI->GenerateMips(misc_->GetRenderTarget());

		YumeTexture2D* in = misc_->GetRenderTarget();
		YumeTexture2D* out = temporaryRt_;

		
		SSAO(0);
		std::swap(in,out);
	}

	void YumePostProcess::Render(YumeShaderVariation* ps,YumeTexture2D* target)
	{
		YumeShaderVariation* triangle = gYume->pRHI->GetShader(VS,"LPV/fs_triangle");

		gYume->pRHI->SetShaders(triangle,ps,0);
		
	}

	void YumePostProcess::SSAO(YumeTexture2D* target)
	{
		Render(ssaoPs_,target);
		
	}
}
