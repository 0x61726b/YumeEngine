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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeD3D11RendererImpl.h"



namespace YumeEngine
{
	YumeD3D11RendererImpl::YumeD3D11RendererImpl()
		: window_(0),
		device_(0),
		debug_(0),
		deviceContext_(0),
		swapChain_(0),
		defaultRenderTargetView_(0),
		defaultDepthTexture_(0),
		defaultDepthStencilView_(0),
		depthStencilView_(0),
		resolveTexture_(0)
	{
		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			renderTargetViews_[i] = 0;

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			shaderResourceViews_[i] = 0;
			samplers_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			vertexBuffers_[i] = 0;
			vertexSizes_[i] = 0;
			vertexOffsets_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
		{
			constantBuffers_[VS][i] = 0;
			constantBuffers_[PS][i] = 0;
		}
	}
}
