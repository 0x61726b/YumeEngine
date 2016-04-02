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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "Math/YumeMath.h"
#include "YumeImage.h"

#include "YumeRHI.h"
#include "YumeGpuResource.h"

#include "Renderer/YumeShaderVariation.h"
#include "Renderer/YumeConstantBuffer.h"
#include "Renderer/YumeTexture.h"

#include "YumeRenderable.h"
#include "YumeTexture.h"
#include "YumeTexture2D.h"

#include "Logging/logging.h"

#include <boost/algorithm/string.hpp>

namespace YumeEngine
{
	RHIEvent::RHIEvent(const YumeString& event)
	{
		gYume->pRHI->BeginEvent(event);
	}
	RHIEvent::~RHIEvent()
	{
		gYume->pRHI->EndEvent();
	}

	YumeRHI::YumeRHI():
		windowIcon_(0),
		window_(0),
		maxScratchBufferRequest_(0),
		useClipPlane_(false),
		defaultTextureFilterMode_(FILTER_TRILINEAR),
		flushGpu_(false),
		multiSample_(1),
		fullscreen_(false),
		borderless_(false),
		resizeable_(false),
		vsync_(false),
		tripleBuffer_(false),
		numPrimitives_(0),
		numBatches_(0),
		windowWidth_(0),
		windowHeight_(0),
		windowTitle_("Yume Engine"),
		windowPos_(SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED),
		sRGB_(false),
		lightPrepassSupport_(false),
		deferredSupport_(false),
		instancingSupport_(false),
		sRGBSupport_(false),
		sRGBWriteSupport_(false),
		anisotropySupport_(false),
		orientations_("LandscapeLeft")
	{
		firstDirtyVB_ = lastDirtyVB_ = M_MAX_UNSIGNED;
	}
	YumeRHI::~YumeRHI()
	{

	}

	bool YumeRHI::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
		int multiSample)
	{
		CreateWindowIcon();

		for(int i=0; i < listeners_.size(); ++i)
			listeners_[i]->HandleGraphicsMode();

		return true;
	}

	void YumeRHI::AddListener(RHIEventListener* listener)
	{
		RHIEventListeners::Iterator i = listeners_.find(listener);

		if(i == listeners_.end())
			listeners_.push_back(listener);
	}

	void YumeRHI::RemoveListener(RHIEventListener* listener)
	{
		RHIEventListeners::Iterator i = listeners_.find(listener);

		if(i != listeners_.end())
			listeners_.erase(i);
	}

	void YumeRHI::SetWindowIcon(YumeImage* image)
	{
		windowIcon_ = image;

		if(window_)
			CreateWindowIcon();
	}

	void YumeRHI::CreateWindowIcon()
	{
		if(windowIcon_)
		{
			SDL_Surface* surface = windowIcon_->GetSDLSurface();
			if(surface)
			{
				SDL_SetWindowIcon(window_,surface);
				SDL_FreeSurface(surface);
			}
		}
	}

	void YumeRHI::SetTextureAnisotropy(unsigned level)
	{
		if(level != textureAnisotropy_)
		{
			textureAnisotropy_ = level;
			SetTextureParametersDirty();
		}
	}

	void YumeRHI::SetDefaultTextureFilterMode(TextureFilterMode mode)
	{
		if(mode != defaultTextureFilterMode_)
		{
			defaultTextureFilterMode_ = mode;
			SetTextureParametersDirty();
		}
	}

	void YumeRHI::SetTextureParametersDirty()
	{
		for(YumeVector<YumeGpuResource*>::iterator i = gpuResources_.begin(); i != gpuResources_.end(); ++i)
		{
			YumeTexture* texture = dynamic_cast<YumeTexture*>(*i);
			if(texture)
				texture->SetParametersDirty();
		}
	}

	bool YumeRHI::ToggleFullscreen()
	{
		return SetGraphicsMode(windowWidth_,windowHeight_,!fullscreen_,borderless_,resizeable_,vsync_,tripleBuffer_,multiSample_);
	}

	void* YumeRHI::ReserveScratchBuffer(unsigned size)
	{
		if(!size)
			return 0;

		if(size > maxScratchBufferRequest_)
			maxScratchBufferRequest_ = size;

		// First check for a free buffer that is large enough
		for(YumeVector<ScratchBuffer>::iterator i = scratchBuffers_.begin(); i != scratchBuffers_.end(); ++i)
		{
			if(!i->reserved_ && i->size_ >= size)
			{
				i->reserved_ = true;
				return i->data_.get();
			}
		}

		// Then check if a free buffer can be resized
		for(YumeVector<ScratchBuffer>::iterator i = scratchBuffers_.begin(); i != scratchBuffers_.end(); ++i)
		{
			if(!i->reserved_)
			{
				i->data_ = boost::shared_array<unsigned char>(new unsigned char[size]);
				i->size_ = size;
				i->reserved_ = true;

				YUMELOG_DEBUG("Resized scratch buffer to size " << std::to_string(size));

				return i->data_.get();
			}
		}

		// Finally allocate a new buffer
		ScratchBuffer newBuffer;
		newBuffer.data_ = boost::shared_array<unsigned char>(new unsigned char[size]);
		newBuffer.size_ = size;
		newBuffer.reserved_ = true;
		scratchBuffers_.push_back(newBuffer);
		YUMELOG_DEBUG("Allocated scratch buffer with size " + std::to_string(size));
		return newBuffer.data_.get();


	}

	void YumeRHI::FreeScratchBuffer(void* buffer)
	{
		if(!buffer)
			return;

		for(YumeVector<ScratchBuffer>::iterator i = scratchBuffers_.begin(); i != scratchBuffers_.end(); ++i)
		{
			if(i->reserved_ && i->data_.get() == buffer)
			{
				i->reserved_ = false;
				return;
			}
		}

		YUMELOG_WARN("Reserved scratch buffer not found");
	}



	void YumeRHI::CleanupScratchBuffers()
	{
		for(YumeVector<ScratchBuffer>::iterator i = scratchBuffers_.begin(); i != scratchBuffers_.end(); ++i)
		{
			if(!i->reserved_ && i->size_ > maxScratchBufferRequest_ * 2)
			{
				i->data_ = maxScratchBufferRequest_ > 0 ? boost::shared_array<unsigned char>(new unsigned char[maxScratchBufferRequest_]) : 0;
				i->size_ = maxScratchBufferRequest_;

				YUMELOG_DEBUG("Resized scratch buffer to size " + std::to_string(maxScratchBufferRequest_));
			}
		}

		maxScratchBufferRequest_ = 0;
	}


	void YumeRHI::ResetRenderTargets()
	{
		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			SetRenderTarget(i,(YumeRenderable*)0);
		SetDepthStencil((YumeRenderable*)0);
		SetViewport(IntRect(0,0,windowWidth_,windowHeight_));
	}

	void YumeRHI::ResetRenderTarget(unsigned index)
	{
		SetRenderTarget(index,(YumeRenderable*)0);
	}

	void YumeRHI::ResetDepthStencil()
	{
		SetDepthStencil((YumeRenderable*)0);
	}

	void YumeRHI::SetRenderTarget(unsigned index,YumeTexture2D* texture)
	{
		YumeRenderable* renderTarget = 0;
		if(texture)
			renderTarget = texture->GetRenderSurface();

		SetRenderTarget(index,renderTarget);
	}

	bool YumeRHI::HasTextureUnit(TextureUnit unit)
	{
		return (vertexShader_ && vertexShader_->HasTextureUnit(unit)) || (pixelShader_ && pixelShader_->HasTextureUnit(unit));
	}


	TextureUnit YumeRHI::GetTextureUnit(const YumeString& name)
	{
		YumeMap<YumeString,TextureUnit>::iterator i = textureUnits_.find(name);
		if(i != textureUnits_.end())
			return i->second;
		else
			return MAX_TEXTURE_UNITS;
	}


	const YumeString& YumeRHI::GetTextureUnitName(TextureUnit unit)
	{
		for(YumeMap<YumeString,TextureUnit>::iterator i = textureUnits_.begin(); i != textureUnits_.end(); ++i)
		{
			if(i->second == unit)
				return i->first;
		}
		return EmptyString;
	}
	YumeTexture* YumeRHI::GetTexture(unsigned index) const
	{
		return index < MAX_TEXTURE_UNITS ? textures_[index] : 0;
	}

	YumeRenderable* YumeRHI::GetRenderTarget(unsigned index) const
	{
		return index < MAX_RENDERTARGETS ? renderTargets_[index] : 0;
	}


	YumeVertexBuffer* YumeRHI::GetVertexBuffer(unsigned index) const
	{
		return index < MAX_VERTEX_STREAMS ? vertexBuffers_[index] : 0;
	}


	unsigned YumeRHI::GetFormatNs(const YumeString& formatName)
	{
		YumeString nameLower = formatName.ToLower().Trimmed();

		if(nameLower == "a")
			return GetAlphaFormatNs();
		if(nameLower == "l")
			return GetLuminanceFormatNs();
		if(nameLower == "la")
			return GetLuminanceAlphaFormatNs();
		if(nameLower == "rgb")
			return GetRGBFormatNs();
		if(nameLower == "rgba")
			return GetRGBAFormatNs();
		if(nameLower == "rgba16")
			return GetRGBA16FormatNs();
		if(nameLower == "rgba16f")
			return GetRGBAFloat16FormatNs();
		if(nameLower == "rgba32f")
			return GetRGBAFloat32FormatNs();
		if(nameLower == "rg16")
			return GetRG16FormatNs();
		if(nameLower == "rg16f")
			return GetRGFloat16FormatNs();
		if(nameLower == "rg32f")
			return GetRGFloat32FormatNs();
		if(nameLower == "r16f")
			return GetFloat16FormatNs();
		if(nameLower == "r32f" || nameLower == "float")
			return GetFloat32FormatNs();
		if(nameLower == "lineardepth" || nameLower == "depth")
			return GetLinearDepthFormatNs();
		if(nameLower == "d24s8")
			return GetDepthStencilFormatNs();
		if(nameLower == "readabledepth" || nameLower == "hwdepth")
			return GetReadableDepthFormatNs();

		return GetRGBFormatNs();
	}
}
