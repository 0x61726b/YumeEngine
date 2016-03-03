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
#include "YumeD3D11IndexBuffer.h"
#include "YumeD3D11RendererImpl.h"
#include "YumeD3D11Renderer.h"

#include "Logging/logging.h"

namespace YumeEngine
{
	YumeD3D11IndexBuffer::YumeD3D11IndexBuffer(YumeRHI* rhi)
		: YumeD3D11Resource(static_cast<YumeD3D11Renderer*>(rhi))
	{

	}

	YumeD3D11IndexBuffer::~YumeD3D11IndexBuffer()
	{
		Release();
	}

	void YumeD3D11IndexBuffer::Release()
	{
		Unlock();

		if(rhi_ && rhi_->GetIndexBuffer() == this)
			rhi_->SetIndexBuffer(0);

		D3D_SAFE_RELEASE(object_);
	}

	void YumeD3D11IndexBuffer::SetShadowed(bool enable)
	{
		if(enable != shadowed_)
		{
			if(enable && indexCount_ && indexSize_)
				shadowData_ = boost::shared_array<unsigned char>(new unsigned char[indexCount_ * indexSize_]);
			else
				shadowData_.reset();

			shadowed_ = enable;
		}
	}

	bool YumeD3D11IndexBuffer::SetSize(unsigned indexCount,unsigned largeIndices,bool dynamic)
	{
		Unlock();

		dynamic_ = dynamic;
		indexCount_ = indexCount;
		indexSize_ = (unsigned)(largeIndices ? sizeof(unsigned) : sizeof(unsigned short));

		if(shadowed_ && indexCount_ && indexSize_)
			shadowData_ =  boost::shared_array<unsigned char>(new unsigned char[indexCount_ * indexSize_]);
		else
			shadowData_.reset();

		return Create();
	}

	bool YumeD3D11IndexBuffer::SetData(const void* data)
	{
		if(!data)
		{
			YUMELOG_ERROR("Null pointer for index buffer data");
			return false;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not set index buffer data");
			return false;
		}

		if(shadowData_ && data != shadowData_.get())
			memcpy(shadowData_.get(),data,indexCount_ * indexSize_);

		if(object_)
		{
			if(dynamic_)
			{
				void* hwData = MapBuffer(0,indexCount_,true);
				if(hwData)
				{
					memcpy(hwData,data,indexCount_ * indexSize_);
					UnmapBuffer();
				}
				else
					return false;
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = 0;
				destBox.right = indexCount_ * indexSize_;
				destBox.top = 0;
				destBox.bottom = 1;
				destBox.front = 0;
				destBox.back = 1;

				static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,&destBox,data,0,0);
			}
		}

		return true;
	}

	bool YumeD3D11IndexBuffer::SetDataRange(const void* data,unsigned start,unsigned count,bool discard)
	{
		if(start == 0 && count == indexCount_)
			return SetData(data);

		if(!data)
		{
			YUMELOG_ERROR("Null pointer for index buffer data");
			return false;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not set index buffer data");
			return false;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal range for setting new index buffer data");
			return false;
		}

		if(!count)
			return true;

		if(shadowData_ && shadowData_.get() + start * indexSize_ != data)
			memcpy(shadowData_.get() + start * indexSize_,data,count * indexSize_);

		if(object_)
		{
			if(dynamic_)
			{
				void* hwData = MapBuffer(start,count,discard);
				if(hwData)
				{
					memcpy(hwData,data,count * indexSize_);
					UnmapBuffer();
				}
				else
					return false;
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = start * indexSize_;
				destBox.right = destBox.left + count * indexSize_;
				destBox.top = 0;
				destBox.bottom = 1;
				destBox.front = 0;
				destBox.back = 1;

				static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,&destBox,data,0,0);
			}
		}

		return true;
	}

	void* YumeD3D11IndexBuffer::Lock(unsigned start,unsigned count,bool discard)
	{
		if(lockState_ != LOCK_NONE)
		{
			YUMELOG_ERROR("Index buffer already locked");
			return 0;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not lock index buffer");
			return 0;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal range for locking index buffer");
			return 0;
		}

		if(!count)
			return 0;

		lockStart_ = start;
		lockCount_ = count;

		// Because shadow data must be kept in sync, can only lock hardware buffer if not shadowed
		if(object_ && !shadowData_ && dynamic_)
			return MapBuffer(start,count,discard);
		else if(shadowData_)
		{
			lockState_ = LOCK_SHADOW;
			return shadowData_.get() + start * indexSize_;
		}
		else if(rhi_)
		{
			lockState_ = LOCK_SCRATCH;
			lockScratchData_ = rhi_->ReserveScratchBuffer(count * indexSize_);
			return lockScratchData_;
		}
		else
			return 0;
	}

	void YumeD3D11IndexBuffer::Unlock()
	{
		switch(lockState_)
		{
		case LOCK_HARDWARE:
			UnmapBuffer();
			break;

		case LOCK_SHADOW:
			SetDataRange(shadowData_.get() + lockStart_ * indexSize_,lockStart_,lockCount_);
			lockState_ = LOCK_NONE;
			break;

		case LOCK_SCRATCH:
			SetDataRange(lockScratchData_,lockStart_,lockCount_);
			if(rhi_)
				rhi_->FreeScratchBuffer(lockScratchData_);
			lockScratchData_ = 0;
			lockState_ = LOCK_NONE;
			break;

		default: break;
		}
	}

	bool YumeD3D11IndexBuffer::GetUsedVertexRange(unsigned start,unsigned count,unsigned& minVertex,unsigned& vertexCount)
	{
		if(!shadowData_)
		{
			YUMELOG_ERROR("Used vertex range can only be queried from an index buffer with shadow data");
			return false;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal index range for querying used vertices");
			return false;
		}

		minVertex = Math::M_MAX_UNSIGNED;
		unsigned maxVertex = 0;

		if(indexSize_ == sizeof(unsigned))
		{
			unsigned* indices = ((unsigned*)shadowData_.get()) + start;

			for(unsigned i = 0; i < count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}
		else
		{
			unsigned short* indices = ((unsigned short*)shadowData_.get()) + start;

			for(unsigned i = 0; i < count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}

		vertexCount = maxVertex - minVertex + 1;
		return true;
	}

	bool YumeD3D11IndexBuffer::Create()
	{
		Release();

		if(!indexCount_)
			return true;

		if(rhi_)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc,0,sizeof bufferDesc);
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = dynamic_ ? D3D11_CPU_ACCESS_WRITE : 0;
			bufferDesc.Usage = dynamic_ ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = (UINT)(indexCount_ * indexSize_);

			HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDevice()->CreateBuffer(&bufferDesc,0,(ID3D11Buffer**)&object_);
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(object_);
				YUMELOG_ERROR("Failed to create index buffer" << hr);
				return false;
			}
		}

		return true;
	}

	bool YumeD3D11IndexBuffer::UpdateToGPU()
	{
		if(object_ && shadowData_)
			return SetData(shadowData_.get());
		else
			return false;
	}

	void* YumeD3D11IndexBuffer::MapBuffer(unsigned start,unsigned count,bool discard)
	{
		void* hwData = 0;

		if(object_)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = 0;

			HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->Map((ID3D11Buffer*)object_,0,discard ? D3D11_MAP_WRITE_DISCARD :
				D3D11_MAP_WRITE,0,&mappedData);
			if(FAILED(hr) || !mappedData.pData)
				YUMELOG_ERROR("Failed to map index buffer",hr);
			else
			{
				hwData = mappedData.pData;
				lockState_ = LOCK_HARDWARE;
			}
		}

		return hwData;
	}

	void YumeD3D11IndexBuffer::UnmapBuffer()
	{
		if(object_ && lockState_ == LOCK_HARDWARE)
		{
			static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->Unmap((ID3D11Buffer*)object_,0);
			lockState_ = LOCK_NONE;
		}
	}
}
