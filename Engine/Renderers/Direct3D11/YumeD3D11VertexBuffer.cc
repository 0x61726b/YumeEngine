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
#include "YumeD3D11VertexBuffer.h"
#include "YumeD3D11Renderer.h"
#include "YumeD3D11RendererImpl.h"



#include "Engine/YumeEngine.h"

#include "Logging/logging.h"


namespace YumeEngine
{
	const char* YumeD3D11VertexBuffer::elementSemantics[] =
	{
		"POSITION",
		"NORMAL",
		"COLOR",
		"TEXCOORD",
		"TEXCOORD",
		"TEXCOORD",
		"TEXCOORD",
		"TANGENT",
		"BLENDWEIGHT",
		"BLENDINDICES",
		"TEXCOORD",
		"TEXCOORD",
		"TEXCOORD",
		"OBJECTINDEX"
	};

	const unsigned YumeD3D11VertexBuffer::elementSemanticIndices[] =
	{
		0,
		0,
		0,
		0,
		1,
		0,
		1,
		0,
		0,
		0,
		2,
		3,
		4,
		0
	};

	const unsigned YumeD3D11VertexBuffer::elementFormats[] =
	{
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32_SINT
	};


	YumeD3D11VertexBuffer::YumeD3D11VertexBuffer(YumeRHI* rhi)
		: YumeD3D11Resource(static_cast<YumeD3D11Renderer*>(rhi))
	{
		UpdateOffsets();
	}

	YumeD3D11VertexBuffer::~YumeD3D11VertexBuffer()
	{
		Release();
	}

	void YumeD3D11VertexBuffer::Release()
	{
		Unlock();

		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			if(rhi_->GetVertexBuffer(i) == this)
				rhi_->SetVertexBuffer(0);
		}


		D3D_SAFE_RELEASE(object_);
	}

	void YumeD3D11VertexBuffer::SetShadowed(bool enable)
	{
		if(enable != shadowed_)
		{
			if(enable && vertexSize_ && vertexCount_)
				shadowData_ = boost::shared_array<unsigned char>(new unsigned char[vertexCount_ * vertexSize_]);
			else
				shadowData_.reset();

			shadowed_ = enable;
		}
	}

	bool YumeD3D11VertexBuffer::SetSize(unsigned vertexCount,unsigned elementMask,bool dynamic)
	{
		Unlock();

		dynamic_ = dynamic;
		vertexCount_ = vertexCount;
		elementMask_ = elementMask;

		UpdateOffsets();

		if(shadowed_ && vertexCount_ && vertexSize_)
			shadowData_ = boost::shared_array<unsigned char>(new unsigned char[vertexCount_ * vertexSize_]);
		else
			shadowData_.reset();

		return Create();
	}

	bool YumeD3D11VertexBuffer::SetData(const void* data)
	{
		if(!data)
		{
			YUMELOG_ERROR("Null pointer for vertex buffer data");
			return false;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if(shadowData_ && data != shadowData_.get())
			memcpy(shadowData_.get(),data,vertexCount_ * vertexSize_);

		if(object_)
		{
			if(dynamic_)
			{
				void* hwData = MapBuffer(0,vertexCount_,true);
				if(hwData)
				{
					memcpy(hwData,data,vertexCount_ * vertexSize_);
					UnmapBuffer();
				}
				else
					return false;
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = 0;
				destBox.right = vertexCount_ * vertexSize_;
				destBox.top = 0;
				destBox.bottom = 1;
				destBox.front = 0;
				destBox.back = 1;

				static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,&destBox,data,0,0);
			}
		}

		return true;
	}

	bool YumeD3D11VertexBuffer::SetDataRange(const void* data,unsigned start,unsigned count,bool discard)
	{
		if(start == 0 && count == vertexCount_)
			return SetData(data);

		if(!data)
		{
			YUMELOG_ERROR("Null pointer for vertex buffer data");
			return false;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if(start + count > vertexCount_)
		{
			YUMELOG_ERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if(!count)
			return true;

		if(shadowData_ && shadowData_.get() + start * vertexSize_ != data)
			memcpy(shadowData_.get() + start * vertexSize_,data,count * vertexSize_);

		if(object_)
		{
			if(dynamic_)
			{
				void* hwData = MapBuffer(start,count,discard);
				if(hwData)
				{
					memcpy(hwData,data,count * vertexSize_);
					UnmapBuffer();
				}
				else
					return false;
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = start * vertexSize_;
				destBox.right = destBox.left + count * vertexSize_;
				destBox.top = 0;
				destBox.bottom = 1;
				destBox.front = 0;
				destBox.back = 1;

				static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,&destBox,data,0,0);
			}
		}

		return true;
	}

	void* YumeD3D11VertexBuffer::Lock(unsigned start,unsigned count,bool discard)
	{
		if(lockState_ != LOCK_NONE)
		{
			YUMELOG_ERROR("Vertex buffer already locked");
			return 0;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not lock vertex buffer");
			return 0;
		}

		if(start + count > vertexCount_)
		{
			YUMELOG_ERROR("Illegal range for locking vertex buffer");
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
			return shadowData_.get() + start * vertexSize_;
		}
		else if(rhi_)
		{
			lockState_ = LOCK_SCRATCH;
			lockScratchData_ = rhi_->ReserveScratchBuffer(count * vertexSize_);
			return lockScratchData_;
		}
		else
			return 0;
	}

	void YumeD3D11VertexBuffer::Unlock()
	{
		switch(lockState_)
		{
		case LOCK_HARDWARE:
			UnmapBuffer();
			break;

		case LOCK_SHADOW:
			SetDataRange(shadowData_.get() + lockStart_ * vertexSize_,lockStart_,lockCount_);
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

	void YumeD3D11VertexBuffer::UpdateOffsets()
	{
		unsigned elementOffset = 0;
		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask_ & (1 << i))
			{
				elementOffset_[i] = elementOffset;
				elementOffset += elementSize[i];
			}
			else
				elementOffset_[i] = NO_ELEMENT;
		}
		vertexSize_ = elementOffset;
	}

	unsigned long long YumeD3D11VertexBuffer::GetBufferHash(unsigned streamIndex,unsigned useMask)
	{
		unsigned long long bufferHash = elementMask_;
		unsigned long long maskHash;
		if(useMask == MASK_DEFAULT)
			maskHash = ((unsigned long long)elementMask_) * 0x100000000ULL;
		else
			maskHash = ((unsigned long long)useMask) * 0x100000000ULL;

		bufferHash |= maskHash;
		bufferHash <<= streamIndex * MAX_VERTEX_ELEMENTS;

		return bufferHash;
	}

	unsigned YumeD3D11VertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned vertexSize = 0;

		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask & (1 << i))
				vertexSize += elementSize[i];
		}

		return vertexSize;
	}

	unsigned YumeD3D11VertexBuffer::GetElementOffset(unsigned elementMask,VertexElement element)
	{
		unsigned offset = 0;

		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(i == element)
				break;

			if(elementMask & (1 << i))
				offset += elementSize[i];
		}

		return offset;
	}

	bool YumeD3D11VertexBuffer::Create()
	{
		Release();

		if(!vertexCount_ || !elementMask_)
			return true;

		if(rhi_)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc,0,sizeof bufferDesc);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = dynamic_ ? D3D11_CPU_ACCESS_WRITE : 0;
			bufferDesc.Usage = dynamic_ ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = (UINT)(vertexCount_ * vertexSize_);

			HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDevice()->CreateBuffer(&bufferDesc,0,(ID3D11Buffer**)&object_);
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(object_);
				YUMELOG_ERROR("Failed to create vertex buffer" << hr);
				return false;
			}
		}

		return true;
	}

	bool YumeD3D11VertexBuffer::UpdateToGPU()
	{
		if(object_ && shadowData_)
			return SetData(shadowData_.get());
		else
			return false;
	}

	void* YumeD3D11VertexBuffer::MapBuffer(unsigned start,unsigned count,bool discard)
	{
		void* hwData = 0;

		if(object_)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = 0;

			HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->Map((ID3D11Buffer*)object_,0,discard ? D3D11_MAP_WRITE_DISCARD :
				D3D11_MAP_WRITE,0,&mappedData);
			if(FAILED(hr) || !mappedData.pData)
				YUMELOG_ERROR("Failed to map vertex buffer" << hr);
			else
			{
				hwData = mappedData.pData;
				lockState_ = LOCK_HARDWARE;
			}
		}

		return hwData;
	}

	void YumeD3D11VertexBuffer::UnmapBuffer()
	{
		if(object_ && lockState_ == LOCK_HARDWARE)
		{
			static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->Unmap((ID3D11Buffer*)object_,0);
			lockState_ = LOCK_NONE;
		}
	}
}
