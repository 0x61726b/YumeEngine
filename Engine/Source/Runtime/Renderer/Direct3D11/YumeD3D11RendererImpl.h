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
#ifndef __YumeD3D11RendererImpl_h__
#define __YumeD3D11RendererImpl_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeRendererDefs.h"

#include <boost/unordered_map.hpp>

#include <D3D11.h>
#include <DXGI.h>

#include <SDL.h>

#define D3D_SAFE_RELEASE(p) if (p) { ((IUnknown*)p)->Release();  p = 0; }
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGraphics;

	typedef boost::unordered_map<unsigned,ID3D11BlendState*> BlendStatesMap;
	typedef boost::unordered_map<unsigned,ID3D11DepthStencilState*> DepthStatesMap;
	typedef boost::unordered_map<unsigned,ID3D11RasterizerState*> RasterizerStatesMap;

	class YumeAPIExport YumeRendererImpl
	{
		friend class YumeGraphics;
	public:
		YumeRendererImpl();


		/// Return Direct3D device.
		ID3D11Device* GetDevice() const { return device_; }

		/// Return Direct3D immediate device context.
		ID3D11DeviceContext* GetDeviceContext() const { return deviceContext_; }

		/// Return swapchain.
		IDXGISwapChain* GetSwapChain() const { return swapChain_; }

		/// Return window.
		SDL_Window* GetWindow() const { return window_; }

	private:
		SDL_Window* window_;

		ID3D11Device* device_;

		ID3D11Debug* debug_;

		ID3D11DeviceContext* deviceContext_;

		IDXGISwapChain* swapChain_;

		ID3D11RenderTargetView* defaultRenderTargetView_;

		ID3D11Texture2D* defaultDepthTexture_;

		ID3D11DepthStencilView* defaultDepthStencilView_;

		ID3D11RenderTargetView* renderTargetViews_[MAX_RENDERTARGETS];

		ID3D11DepthStencilView* depthStencilView_;

		BlendStatesMap blendStates_;

		DepthStatesMap depthStates_;

		RasterizerStatesMap rasterizerStates_;

		ID3D11Texture2D* resolveTexture_;

		ID3D11ShaderResourceView* shaderResourceViews_[MAX_TEXTURE_UNITS];

		ID3D11SamplerState* samplers_[MAX_TEXTURE_UNITS];

		ID3D11Buffer* vertexBuffers_[MAX_VERTEX_STREAMS];

		ID3D11Buffer* constantBuffers_[2][MAX_SHADER_PARAMETER_GROUPS];

		unsigned vertexSizes_[MAX_VERTEX_STREAMS];

		unsigned vertexOffsets_[MAX_VERTEX_STREAMS];

	};
}


//----------------------------------------------------------------------------
#endif
