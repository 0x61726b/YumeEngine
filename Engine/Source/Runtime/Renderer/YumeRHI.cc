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

#include "Logging/logging.h"

namespace YumeEngine
{
	YumeRHI::YumeRHI():
		windowIcon_(0),
		window_(0),
		maxScratchBufferRequest_(0),
		useClipPlane_(false),
		defaultTextureFilterMode_(FILTER_TRILINEAR)
	{
		firstDirtyVB_ = lastDirtyVB_ = Math::M_MAX_UNSIGNED;
	}
	YumeRHI::~YumeRHI()
	{

	}

	bool YumeRHI::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
		int multiSample)
	{
		CreateWindowIcon();

		return true;
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

	void YumeRHI::SetTextureParametersDirty()
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		for(YumeVector<YumeGpuResource*>::iterator i = gpuResources_.begin(); i != gpuResources_.end(); ++i)
		{
			YumeTexture* texture = dynamic_cast<YumeTexture*>(*i);
			if(texture)
				texture->SetParametersDirty();
		}
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
		return YumeString();
	}

	YumeVertexBuffer* YumeRHI::GetVertexBuffer(unsigned index) const
	{
		return index < MAX_VERTEX_STREAMS ? vertexBuffers_[index] : 0;
	}
}
