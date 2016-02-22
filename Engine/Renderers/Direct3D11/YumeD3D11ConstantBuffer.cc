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
#include "YumeD3D11ConstantBuffer.h"
#include "YumeD3D11RendererImpl.h"
#include "YumeD3D11Renderer.h"
#include "Engine/YumeEngine.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	YumeConstantBuffer::YumeConstantBuffer(YumeD3D11RendererImpl* impl)
		: impl_(impl)
	{
	}

	YumeConstantBuffer::~YumeConstantBuffer()
	{
		Release();
	}

	void YumeConstantBuffer::Release()
	{
		D3D_SAFE_RELEASE(object_);

		shadowData_.reset();
		size_ = 0;
	}

	bool YumeConstantBuffer::SetSize(unsigned size)
	{
		Release();

		if(!size)
		{
			YUMELOG_ERROR("Can not create zero-sized constant buffer");
			return false;
		}

		// Round up to next 16 bytes
		size += 15;
		size &= 0xfffffff0;

		size_ = size;
		dirty_ = false;
		shadowData_ = boost::shared_array<unsigned char>(new unsigned char[size_]);
		memset(shadowData_.get(),0,size_);

		if(YumeEngine3D::Get()->GetRenderer())
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc,0,sizeof bufferDesc);

			bufferDesc.ByteWidth = size_;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;

			
			HRESULT hr = impl_->GetDevice()->CreateBuffer(&bufferDesc,0,(ID3D11Buffer**)&object_);
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(object_);
				YUMELOG_ERROR("Failed to create constant buffer " << hr);
				return false;
			}
		}

		return true;
	}

	void YumeConstantBuffer::SetParameter(unsigned offset,unsigned size,const void* data)
	{
		if(offset + size > size_)
			return; // Would overflow the buffer

		memcpy(&shadowData_[offset],data,size);
		dirty_ = true;
	}

	void YumeConstantBuffer::SetVector3ArrayParameter(unsigned offset,unsigned rows,const void* data)
	{
		if(offset + rows * 4 * sizeof(float) > size_)
			return; // Would overflow the buffer

		float* dest = (float*)&shadowData_[offset];
		const float* src = (const float*)data;

		while(rows--)
		{
			*dest++ = *src++;
			*dest++ = *src++;
			*dest++ = *src++;
			++dest; // Skip over the w coordinate
		}

		dirty_ = true;
	}

	void YumeConstantBuffer::Apply()
	{
		if(dirty_ && object_)
		{
			impl_->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,0,shadowData_.get(),0,0);
			dirty_ = false;
		}
	}
}
