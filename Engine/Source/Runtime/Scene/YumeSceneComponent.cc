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
#include "YumeSceneComponent.h"
#include "YumeScene.h"
#include "YumeSceneNode.h"

#include "Core/YumeDefaults.h"

namespace YumeEngine
{
	YumeHash YumeSceneComponent::type_ = GenerateHash("SceneComponent");

	YumeSceneComponent::YumeSceneComponent():
		node_(0),
		id_(0),
		enabled_(true)
	{
		type_ = GenerateHash("SceneComponent");
		typeName_ = "SceneComponent";
	}

	YumeSceneComponent::~YumeSceneComponent()
	{

	}

	YumeHash YumeSceneComponent::GetType()
	{
		return type_;
	}

	const YumeString& YumeSceneComponent::GetTypeName()
	{
		return typeName_;
	}


	void YumeSceneComponent::SetEnabled(bool enable)
	{
		if(enable != enabled_)
		{
			enabled_ = enable;
			OnSetEnabled();

			// Send change event for the component
			YumeScene* scene = GetScene();
			if(scene)
			{

			}
		}
	}

	void YumeSceneComponent::Remove()
	{
		if(node_)
			node_->RemoveComponent(this);
	}

	YumeScene* YumeSceneComponent::GetScene() const
	{
		return node_ ? node_->GetScene() : 0;
	}


	void YumeSceneComponent::OnNodeSet(YumeSceneNode* node)
	{
	}

	void YumeSceneComponent::OnSceneSet(YumeScene* scene)
	{
	}

	void YumeSceneComponent::OnMarkedDirty(YumeSceneNode* node)
	{
	}

	void YumeSceneComponent::OnNodeSetEnabled(YumeSceneNode* node)
	{
	}

	void YumeSceneComponent::SetID(unsigned id)
	{
		id_ = id;
	}

	void YumeSceneComponent::SetNode(YumeSceneNode* node)
	{
		node_ = node;
		OnNodeSet(node_);
	}

	YumeSceneComponent* YumeSceneComponent::GetComponent(YumeHash type) const
	{
		return node_ ? node_->GetComponent(type) : 0;
	}

	bool YumeSceneComponent::IsEnabledEffective() const
	{
		return enabled_ && node_ && node_->IsEnabled();
	}

	void YumeSceneComponent::GetComponents(YumeVector<YumeSceneComponent*>::type& dest,YumeHash type) const
	{
		if(node_)
			node_->GetComponents(dest,type);
		else
			dest.clear();
	}
}
