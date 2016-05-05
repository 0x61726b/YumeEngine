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
#include "AssimpMesh.h"



namespace YumeEngine
{
	UINT count(const D3D11_INPUT_ELEMENT_DESC* desc)
	{
		UINT array_size = 0;
		while(desc->SemanticName != nullptr) { array_size++; desc++; }
		return array_size;
	}

	bool is_visible(const aabb<DirectX::XMFLOAT3>& bbox,const DirectX::XMFLOAT4X4& to_clip)
	{
		DirectX::XMFLOAT3 mi = bbox.bb_min();
		DirectX::XMFLOAT3 ma = bbox.bb_max();

		DirectX::XMFLOAT3 aabb[8] =
		{
			mi,ma,
			DirectX::XMFLOAT3(mi.x,ma.y,ma.z),
			DirectX::XMFLOAT3(mi.x,mi.y,ma.z),
			DirectX::XMFLOAT3(mi.x,ma.y,mi.z),
			DirectX::XMFLOAT3(ma.x,mi.y,mi.z),
			DirectX::XMFLOAT3(ma.x,mi.y,ma.z),
			DirectX::XMFLOAT3(ma.x,ma.y,mi.z),
		};

		DirectX::XMVECTOR aabb_trans[8];

		DirectX::XMMATRIX m_to_clip = DirectX::XMLoadFloat4x4(&to_clip);

		for(size_t i = 0; i < 8; ++i)
			aabb_trans[i] = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&aabb[i]),m_to_clip);

		bool inside;

		for(size_t i = 0; i < 6; ++i)
		{
			for(size_t k = 0; k < 3; ++k)
			{
				inside = false;

				for(size_t j = 0; j < 8; ++j)
				{
					if(DirectX::XMVectorGetByIndex(aabb_trans[j],k) > -DirectX::XMVectorGetW(aabb_trans[j]))
					{
						inside = true;
						break;
					}
				}

				if(!inside) break;

				inside = false;

				for(size_t j = 0; j < 8; ++j)
				{
					if(DirectX::XMVectorGetByIndex(aabb_trans[j],k) < DirectX::XMVectorGetW(aabb_trans[j]))
					{
						inside = true;
						break;
					}
				}

				if(!inside) break;
			}

			if(!inside) break;
		}

		return inside;
	}
}
