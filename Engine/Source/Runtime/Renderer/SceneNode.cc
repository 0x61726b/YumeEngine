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
#include "SceneNode.h"

using namespace DirectX;

namespace YumeEngine
{
	SceneNode::SceneNode(GeometryType gt)
		: type_(gt),
		pos_(XMFLOAT4(0,0,0,0)),
		rot_(XMFLOAT4(0,0,0,0)),
		dir_(XMFLOAT4(0,0,0,0)),
		geometry_(0),
		name_("SceneNode")
	{
		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4(&World,I);
		XMStoreFloat4x4(&Scale,I);

		pos_ = XMFLOAT4(0,0,0,0);
		rot_ = XMFLOAT4(0,0,0,0);

	}

	SceneNode::~SceneNode()
	{
	}


	DirectX::XMMATRIX SceneNode::GetTransformation()
	{
		XMMATRIX translate = DirectX::XMMatrixTranslationFromVector(XMLoadFloat4(&pos_));
		XMMATRIX scale = XMLoadFloat4x4(&Scale);
		XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&rot_));


		XMMATRIX transformation = scale * rotation * translate;
		return transformation;
	}
	void SceneNode::SetPosition(const DirectX::XMVECTOR& v)
	{
		DirectX::XMStoreFloat4(&pos_,v);
	}

	void SceneNode::SetRotation(const DirectX::XMVECTOR& v)
	{
		DirectX::XMStoreFloat4(&rot_,v);
	}

	void SceneNode::SetDirection(const DirectX::XMVECTOR& v)
	{
		DirectX::XMStoreFloat4(&dir_,v);
	}

	void SceneNode::SetWorld(const DirectX::XMMATRIX& world)
	{
		DirectX::XMStoreFloat4x4(&World,world);
	}

	void SceneNode::SetScale(float x,float y,float z)
	{
		XMMATRIX scale = DirectX::XMMatrixScaling(x,y,z);

		XMStoreFloat4x4(&Scale,scale);
	}
}
