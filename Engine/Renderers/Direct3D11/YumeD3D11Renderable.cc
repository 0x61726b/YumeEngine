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
#include "YumeD3D11Renderable.h"
#include "YumeD3D11Renderer.h"
#include "Renderer/YumeTexture.h"

#include "Engine/YumeEngine.h"


namespace YumeEngine
{

	YumeD3D11Renderable::YumeD3D11Renderable(YumeTexture* parentTexture):
		YumeRenderable(parentTexture)
	{
	}

	YumeD3D11Renderable::~YumeD3D11Renderable()
	{
		Release();
	}


	void YumeD3D11Renderable::Release()
	{
		YumeRHI* graphics = YumeEngine3D::Get()->GetRenderer();
		if(!graphics)
			return;
		if(graphics && renderTargetView_)
		{
			for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			{
				if(graphics->GetRenderTarget(i) == this)
					graphics->ResetRenderTarget(i);
			}

			if(graphics->GetDepthStencil() == this)
				graphics->ResetDepthStencil();
		}

		D3D_SAFE_RELEASE(renderTargetView_);
		D3D_SAFE_RELEASE(readOnlyView_);
	}
}
