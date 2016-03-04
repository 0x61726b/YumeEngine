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

#include "Renderer/YumeRendererDefs.h"
#include "YumeD3D11InputLayout.h"

#include "YumeD3D11VertexBuffer.h"

#include "Renderer/YumeShaderVariation.h"

#include "YumeD3D11Renderer.h"
#include "YumeD3D11RendererImpl.h"

#include "Logging/logging.h"

namespace YumeEngine
{
	YumeD3D11InputLayout::YumeD3D11InputLayout(YumeRHI* rhi,YumeShaderVariation* vertexShader,YumeVertexBuffer** vertexBuffers,unsigned* elementMasks)
		: YumeInputLayout(rhi,vertexShader,vertexBuffers,elementMasks)
	{
		YumeVector<D3D11_INPUT_ELEMENT_DESC>::type elementDescs;

		unsigned vbElementMask = 0;

		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			if(vertexBuffers[i] && elementMasks[i])
			{
				for(unsigned j = 0; j < MAX_VERTEX_ELEMENTS; ++j)
				{
					if(elementMasks[i] & (1 << j))
					{
						D3D11_INPUT_ELEMENT_DESC newDesc;
						newDesc.SemanticName = YumeD3D11VertexBuffer::elementSemantics[j];
						newDesc.SemanticIndex = YumeD3D11VertexBuffer::elementSemanticIndices[j];
						newDesc.Format = (DXGI_FORMAT)YumeD3D11VertexBuffer::elementFormats[j];
						newDesc.InputSlot = (UINT)i;
						newDesc.AlignedByteOffset = vertexBuffers[i]->GetElementOffset((VertexElement)j);
						newDesc.InputSlotClass = (j >= ELEMENT_INSTANCEMATRIX1 && j <= ELEMENT_INSTANCEMATRIX3) ?
						D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
						newDesc.InstanceDataStepRate = (j >= ELEMENT_INSTANCEMATRIX1 && j <= ELEMENT_INSTANCEMATRIX3) ? 1 : 0;
						elementDescs.push_back(newDesc);
						vbElementMask |= 1 << j;
					}
				}
			}
		}

		if(elementDescs.empty())
			return;

		const YumeVector<unsigned char>::type& byteCode = vertexShader->GetByteCode();

		HRESULT hr = static_cast<YumeD3D11Renderer*>(rhi)->GetImpl()->GetDevice()->CreateInputLayout(&elementDescs[0],(UINT)elementDescs.size(),&byteCode[0],
			byteCode.size(),(ID3D11InputLayout**)&inputLayout_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(inputLayout_);
			YUMELOG_ERROR("Failed to create input layout for shader" <<
				vertexShader->GetFullName().c_str() << vbElementMask << (unsigned)hr);
		}
	}

	YumeD3D11InputLayout::~YumeD3D11InputLayout()
	{
		D3D_SAFE_RELEASE(inputLayout_);
	}
}
