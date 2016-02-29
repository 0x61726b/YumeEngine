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

	YumeD3D11ConstantBuffer::YumeD3D11ConstantBuffer(YumeRenderer* impl)
		: YumeD3D11Resource(static_cast<YumeD3D11Renderer*>(impl))
	{
	}

	YumeD3D11ConstantBuffer::~YumeD3D11ConstantBuffer()
	{
		Release();
	}

	void YumeD3D11ConstantBuffer::Release()
	{
		D3D_SAFE_RELEASE(object_);

		shadowData_.reset();
		size_ = 0;
	}

	bool YumeD3D11ConstantBuffer::SetSize(unsigned size)
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

		if(rhi_)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc,0,sizeof bufferDesc);

			bufferDesc.ByteWidth = size_;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;

			
			HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDevice()->CreateBuffer(&bufferDesc,0,(ID3D11Buffer**)&object_);
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(object_);
				YUMELOG_ERROR("Failed to create constant buffer " << hr);
				return false;
			}
		}

		return true;
	}

	void YumeD3D11ConstantBuffer::Apply()
	{
		if(dirty_ && object_)
		{
			static_cast<YumeD3D11Renderer*>(rhi_)->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Buffer*)object_,0,0,shadowData_.get(),0,0);
			dirty_ = false;
		}
	}
}
