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
#include "YumeD3D11Texture3D.h"
#include "YumeD3D11Renderer.h"
#include "YumeD3D11RendererImpl.h"
#include "YumeD3D11Renderable.h"

#include "Renderer/YumeImage.h"
#include "Renderer/YumeRenderer.h"
#include "Engine/YumeEngine.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	YumeD3D11Texture3D::YumeD3D11Texture3D()
	{
		format_ = (DXGI_FORMAT_UNKNOWN);
	}

	YumeD3D11Texture3D::~YumeD3D11Texture3D()
	{
		Release();
	}

	void YumeD3D11Texture3D::Release()
	{
		if(!gYume->pRHI && object_)
		{
			for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			{
				if(gYume->pRHI->GetTexture(i) == this)
					gYume->pRHI->SetTexture(i,0);
			}
		}

		D3D_SAFE_RELEASE(object_);
		D3D_SAFE_RELEASE(shaderResourceView_);
		D3D_SAFE_RELEASE(sampler_);
	}


	bool YumeD3D11Texture3D::SetSize(int width,int height,int depth,unsigned format,TextureUsage usage)
	{
		if(width <= 0 || height <= 0 || depth <= 0)
		{
			YUMELOG_ERROR("Zero or negative 3D texture dimensions");
			return false;
		}
		if(usage >= TEXTURE_RENDERTARGET)
		{
			YUMELOG_ERROR("Rendertarget or depth-stencil usage not supported for 3D textures");
			return false;
		}
		if(usage_ == TEXTURE_DYNAMIC)
			requestedLevels_ = 1;

		usage_ = usage;

		width_ = width;
		height_ = height;
		depth_ = depth;
		format_ = format;

		return Create();
	}

	bool YumeD3D11Texture3D::SetData(unsigned level,int x,int y,int z,int width,int height,int depth,const void* data)
	{
		if(!object_)
		{
			YUMELOG_ERROR("No texture created, can not set data");
			return false;
		}

		if(!data)
		{
			YUMELOG_ERROR("Null source for setting data");
			return false;
		}

		if(level >= levels_)
		{
			YUMELOG_ERROR("Illegal mip level for setting data");
			return false;
		}

		int levelWidth = GetLevelWidth(level);
		int levelHeight = GetLevelHeight(level);
		int levelDepth = GetLevelDepth(level);
		if(x < 0 || x + width > levelWidth || y < 0 || y + height > levelHeight || z < 0 || z + depth > levelDepth || width <= 0 ||
			height <= 0 || depth <= 0)
		{
			YUMELOG_ERROR("Illegal dimensions for setting data");
			return false;
		}

		// If compressed, align the update region on a block
		if(IsCompressed())
		{
			x &= ~3;
			y &= ~3;
			width += 3;
			width &= 0xfffffffc;
			height += 3;
			height &= 0xfffffffc;
		}

		unsigned char* src = (unsigned char*)data;
		unsigned rowSize = GetRowDataSize(width);
		unsigned rowStart = GetRowDataSize(x);
		unsigned subResource = D3D11CalcSubresource(level,0,levels_);

		if(usage_ == TEXTURE_DYNAMIC)
		{
			if(IsCompressed())
			{
				height = (height + 3) >> 2;
				y >>= 2;
			}

			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = 0;

			HRESULT hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->Map((ID3D11Resource*)object_,subResource,D3D11_MAP_WRITE_DISCARD,0,
				&mappedData);
			if(FAILED(hr) || !mappedData.pData)
			{
				YUMELOG_ERROR("Failed to map texture for update",hr);
				return false;
			}
			else
			{
				for(int page = 0; page < depth; ++page)
				{
					for(int row = 0; row < height; ++row)
					{
						memcpy((unsigned char*)mappedData.pData + (page + z) * mappedData.DepthPitch + (row + y) * mappedData.RowPitch +
							rowStart,src + row * rowSize,rowSize);
					}
				}

				static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->Unmap((ID3D11Resource*)object_,subResource);
			}
		}
		else
		{
			if(IsCompressed())
				levelHeight = (levelHeight + 3) >> 2;

			D3D11_BOX destBox;
			destBox.left = (UINT)x;
			destBox.right = (UINT)(x + width);
			destBox.top = (UINT)y;
			destBox.bottom = (UINT)(y + height);
			destBox.front = (UINT)z;
			destBox.back = (UINT)(z + depth);

			static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Resource*)object_,subResource,&destBox,data,
				rowSize,levelHeight * rowSize);
		}

		return true;
	}

	bool YumeD3D11Texture3D::SetData(SharedPtr<YumeImage> image,bool useAlpha)
	{
		if(!image)
		{
			YUMELOG_ERROR("Null image, can not load texture");
			return false;
		}

		unsigned memoryUse = sizeof(YumeD3D11Texture3D);

		int quality = QUALITY_HIGH;
		YumeRenderer* renderer = gYume->pRenderer;
		if(renderer)
			quality = renderer->GetTextureQuality();

		if(!image->IsCompressed())
		{
			// Convert unsuitable formats to RGBA
			unsigned components = image->GetComponents();
			if((components == 1 && !useAlpha) || components == 2 || components == 3)
			{
				image = image->ConvertToRGBA();
				if(!image)
					return false;
				components = image->GetComponents();
			}

			unsigned char* levelData = image->GetData();
			int levelWidth = image->GetWidth();
			int levelHeight = image->GetHeight();
			int levelDepth = image->GetDepth();
			unsigned format = 0;

			// Discard unnecessary mip levels
			for(unsigned i = 0; i < mipsToSkip_[quality]; ++i)
			{
				image = image->GetNextLevel();
				levelData = image->GetData();
				levelWidth = image->GetWidth();
				levelHeight = image->GetHeight();
				levelDepth = image->GetDepth();
			}

			switch(components)
			{
			case 1:
				format = YumeD3D11Renderer::GetAlphaFormat();
				break;

			case 4:
				format = YumeD3D11Renderer::GetRGBAFormat();
				break;

			default: break;
			}

			// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
			if(IsCompressed() && requestedLevels_ > 1)
				requestedLevels_ = 0;
			SetSize(levelWidth,levelHeight,levelDepth,format);

			for(unsigned i = 0; i < levels_; ++i)
			{
				SetData(i,0,0,0,levelWidth,levelHeight,levelDepth,levelData);
				memoryUse += levelWidth * levelHeight * levelDepth * components;

				if(i < levels_ - 1)
				{
					image = image->GetNextLevel();
					levelData = image->GetData();
					levelWidth = image->GetWidth();
					levelHeight = image->GetHeight();
					levelDepth = image->GetDepth();
				}
			}
		}
		else
		{
			int width = image->GetWidth();
			int height = image->GetHeight();
			int depth = image->GetDepth();
			unsigned levels = image->GetNumCompressedLevels();
			unsigned format = gYume->pRHI->GetFormat(image->GetCompressedFormat());
			bool needDecompress = false;

			if(!format)
			{
				format = YumeD3D11Renderer::GetRGBAFormat();
				needDecompress = true;
			}

			unsigned mipsToSkip = mipsToSkip_[quality];
			if(mipsToSkip >= levels)
				mipsToSkip = levels - 1;
			while(mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4 || depth / (1 << mipsToSkip) < 4))
				--mipsToSkip;
			width /= (1 << mipsToSkip);
			height /= (1 << mipsToSkip);
			depth /= (1 << mipsToSkip);

			SetNumLevels(std::max((levels - mipsToSkip),1U));
			SetSize(width,height,depth,format);

			for(unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
			{
				CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
				if(!needDecompress)
				{
					SetData(i,0,0,0,level.width_,level.height_,level.depth_,level.data_);
					memoryUse += level.depth_ * level.rows_ * level.rowSize_;
				}
				else
				{
					unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * level.depth_ * 4];
					level.Decompress(rgbaData);
					SetData(i,0,0,0,level.width_,level.height_,level.depth_,rgbaData);
					memoryUse += level.width_ * level.height_ * level.depth_ * 4;
					delete[] rgbaData;
				}
			}
		}

		SetMemoryUsage(memoryUse);
		return true;
	}

	bool YumeD3D11Texture3D::GetData(unsigned level,void* dest) const
	{
		if(!object_)
		{
			YUMELOG_ERROR("No texture created, can not get data");
			return false;
		}

		if(!dest)
		{
			YUMELOG_ERROR("Null destination for getting data");
			return false;
		}

		if(level >= levels_)
		{
			YUMELOG_ERROR("Illegal mip level for getting data");
			return false;
		}

		int levelWidth = GetLevelWidth(level);
		int levelHeight = GetLevelHeight(level);
		int levelDepth = GetLevelDepth(level);

		D3D11_TEXTURE3D_DESC textureDesc;
		memset(&textureDesc,0,sizeof textureDesc);
		textureDesc.Width = (UINT)levelWidth;
		textureDesc.Height = (UINT)levelHeight;
		textureDesc.Depth = (UINT)levelDepth;
		textureDesc.MipLevels = 1;
		textureDesc.Format = (DXGI_FORMAT)format_;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ID3D11Texture3D* stagingTexture = 0;
		HRESULT hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDevice()->CreateTexture3D(&textureDesc,0,&stagingTexture);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(stagingTexture);
			YUMELOG_ERROR("Failed to create staging texture for GetData",hr);
			return false;
		}

		unsigned srcSubResource = D3D11CalcSubresource(level,0,levels_);
		D3D11_BOX srcBox;
		srcBox.left = 0;
		srcBox.right = (UINT)levelWidth;
		srcBox.top = 0;
		srcBox.bottom = (UINT)levelHeight;
		srcBox.front = 0;
		srcBox.back = (UINT)levelDepth;
		static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->CopySubresourceRegion(stagingTexture,0,0,0,0,(ID3D11Resource*)object_,
			srcSubResource,&srcBox);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		mappedData.pData = 0;
		unsigned rowSize = GetRowDataSize(levelWidth);
		unsigned numRows = (unsigned)(IsCompressed() ? (levelHeight + 3) >> 2 : levelHeight);

		hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->Map((ID3D11Resource*)stagingTexture,0,D3D11_MAP_READ,0,&mappedData);
		if(FAILED(hr) || !mappedData.pData)
		{
			YUMELOG_ERROR("Failed to map staging texture for GetData",hr);
			stagingTexture->Release();
			return false;
		}
		else
		{
			for(int page = 0; page < levelDepth; ++page)
			{
				for(unsigned row = 0; row < numRows; ++row)
				{
					memcpy((unsigned char*)dest + (page * numRows + row) * rowSize,
						(unsigned char*)mappedData.pData + page * mappedData.DepthPitch + row * mappedData.RowPitch,rowSize);
				}
			}
			static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDeviceContext()->Unmap((ID3D11Resource*)stagingTexture,0);
			stagingTexture->Release();
			return true;
		}
	}

	bool YumeD3D11Texture3D::Create()
	{
		Release();

		if(!gYume->pRHI || !width_ || !height_ || !depth_)
			return false;

		levels_ = CheckMaxLevels(width_,height_,depth_,requestedLevels_);

		D3D11_TEXTURE3D_DESC textureDesc;
		memset(&textureDesc,0,sizeof textureDesc);
		textureDesc.Width = (UINT)width_;
		textureDesc.Height = (UINT)height_;
		textureDesc.Depth = (UINT)depth_;
		textureDesc.MipLevels = levels_;
		textureDesc.Format = (DXGI_FORMAT)(sRGB_ ? GetSRGBFormat(format_) : format_);
		textureDesc.Usage = usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		HRESULT hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDevice()->CreateTexture3D(&textureDesc,0,(ID3D11Texture3D**)&object_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(object_);
			YUMELOG_ERROR("Failed to create texture",hr);
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		memset(&resourceViewDesc,0,sizeof resourceViewDesc);
		resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		resourceViewDesc.Texture3D.MipLevels = (UINT)levels_;

		hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDevice()->CreateShaderResourceView((ID3D11Resource*)object_,&resourceViewDesc,
			(ID3D11ShaderResourceView**)&shaderResourceView_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(shaderResourceView_);
			YUMELOG_ERROR("Failed to create shader resource view for texture",hr);
			return false;
		}

		return true;
	}

	void YumeD3D11Texture3D::UpdateParameters()
	{
		if((!parametersDirty_ && sampler_) || !object_)
			return;

		// Release old sampler
		D3D_SAFE_RELEASE(sampler_);

		D3D11_SAMPLER_DESC samplerDesc;
		memset(&samplerDesc,0,sizeof samplerDesc);
		unsigned filterModeIndex = filterMode_ != FILTER_DEFAULT ? filterMode_ : gYume->pRHI->GetDefaultTextureFilterMode();
		if(shadowCompare_)
			filterModeIndex += 4;
		samplerDesc.Filter = d3dFilterMode[filterModeIndex];
		samplerDesc.AddressU = d3dAddressMode[addressMode_[0]];
		samplerDesc.AddressV = d3dAddressMode[addressMode_[1]];
		samplerDesc.AddressW = d3dAddressMode[addressMode_[2]];
		samplerDesc.MaxAnisotropy = gYume->pRHI->GetTextureAnisotropy();
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.MinLOD = -M_INFINITY;
		samplerDesc.MaxLOD = M_INFINITY;
		memcpy(&samplerDesc.BorderColor,borderColor_.Data(),4 * sizeof(float));

		HRESULT hr = static_cast<YumeD3D11Renderer*>(gYume->pRHI)->GetImpl()->GetDevice()->CreateSamplerState(&samplerDesc,(ID3D11SamplerState**)&sampler_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(sampler_);
			YUMELOG_ERROR("Failed to create sampler state" << hr);
		}

		parametersDirty_ = false;
	}

	bool YumeD3D11Texture3D::IsCompressed() const
	{
		return format_ == DXGI_FORMAT_BC1_UNORM || format_ == DXGI_FORMAT_BC2_UNORM || format_ == DXGI_FORMAT_BC3_UNORM;
	}

	unsigned YumeD3D11Texture3D::GetDataSize(int width,int height) const
	{
		return 0;
	}

	unsigned YumeD3D11Texture3D::GetRowDataSize(int width) const
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

	void YumeD3D11Texture3D::CheckTextureBudget(YumeHash type)
	{

	}


	unsigned YumeD3D11Texture3D::GetSRVFormat(unsigned format)
	{
		if(format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		else if(format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_R16_UNORM;
		else if(format == DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_R32_FLOAT;
		else
			return format;
	}

	unsigned YumeD3D11Texture3D::GetDSVFormat(unsigned format)
	{
		if(format == DXGI_FORMAT_R24G8_TYPELESS)
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		else if(format == DXGI_FORMAT_R16_TYPELESS)
			return DXGI_FORMAT_D16_UNORM;
		else if(format == DXGI_FORMAT_R32_TYPELESS)
			return DXGI_FORMAT_D32_FLOAT;
		else
			return format;
	}

	unsigned YumeD3D11Texture3D::GetSRGBFormat(unsigned format)
	{
		if(format == DXGI_FORMAT_R8G8B8A8_UNORM)
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		else if(format == DXGI_FORMAT_BC1_UNORM)
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		else if(format == DXGI_FORMAT_BC2_UNORM)
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		else if(format == DXGI_FORMAT_BC3_UNORM)
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		else
			return format;
	}
}
