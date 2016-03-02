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
#include "YumeD3D11Texture.h"
#include "YumeD3D11RendererImpl.h"
#include "YumeD3D11Renderer.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	static const char* addressModeNames[] =
	{
		"wrap",
		"mirror",
		"clamp",
		"border",
		0
	};

	static const char* filterModeNames[] =
	{
		"nearest",
		"bilinear",
		"trilinear",
		"anisotropic",
		"default",
		0
	};

	static const D3D11_FILTER d3dFilterMode[] =
	{
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_FILTER_ANISOTROPIC,
		D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		D3D11_FILTER_COMPARISON_ANISOTROPIC
	};

	static const D3D11_TEXTURE_ADDRESS_MODE d3dAddressMode[] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_BORDER
	};

	YumeD3D11Texture::YumeD3D11Texture(YumeRHI* rhi)
		: YumeD3D11Resource(static_cast<YumeD3D11Renderer*>(rhi))
	{
		format_ = (DXGI_FORMAT_UNKNOWN);
	}

	YumeD3D11Texture::~YumeD3D11Texture()
	{
	}

	unsigned YumeD3D11Texture::GetRowDataSize(int width) const
	{
		switch(format_)
		{
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_A8_UNORM:
			return (unsigned)width;

		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_R16_TYPELESS:
			return (unsigned)(width * 2);

		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
			return (unsigned)(width * 4);

		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return (unsigned)(width * 8);

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return (unsigned)(width * 16);

		case DXGI_FORMAT_BC1_UNORM:
			return (unsigned)(((width + 3) >> 2) * 8);

		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
			return (unsigned)(((width + 3) >> 2) * 16);

		default:
			return 0;
		}
	}
	void YumeD3D11Texture::UpdateParameters()
	{
		if((!parametersDirty_ && sampler_) || !object_)
			return;

		// Release old sampler
		D3D_SAFE_RELEASE(sampler_);

		D3D11_SAMPLER_DESC samplerDesc;
		memset(&samplerDesc,0,sizeof samplerDesc);
		unsigned filterModeIndex = filterMode_ != FILTER_DEFAULT ? filterMode_ : rhi_->GetDefaultTextureFilterMode();
		if(shadowCompare_)
			filterModeIndex += 4;
		samplerDesc.Filter = d3dFilterMode[filterModeIndex];
		samplerDesc.AddressU = d3dAddressMode[addressMode_[0]];
		samplerDesc.AddressV = d3dAddressMode[addressMode_[1]];
		samplerDesc.AddressW = d3dAddressMode[addressMode_[2]];
		samplerDesc.MaxAnisotropy = rhi_->GetTextureAnisotropy();
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.MinLOD = Math::NEG_INFINITY;
		samplerDesc.MaxLOD = Math::POS_INFINITY;
		memcpy(&samplerDesc.BorderColor,borderColor_.Data(),4 * sizeof(float));

		HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDevice()->CreateSamplerState(&samplerDesc,(ID3D11SamplerState**)&sampler_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(sampler_);
			YUMELOG_ERROR("Failed to create sampler state" << hr);
		}

		parametersDirty_ = false;
	}

	void YumeD3D11Texture::CheckTextureBudget(YumeHash type)
	{

	}
}
