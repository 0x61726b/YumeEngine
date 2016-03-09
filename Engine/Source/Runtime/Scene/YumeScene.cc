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
#include "YumeScene.h"
#include "YumeSceneNode.h"
#include "YumeSceneComponent.h"
#include "Engine/YumeEngine.h"
#include "Logging/logging.h"

namespace YumeEngine
{


	const char* SCENE_CATEGORY = "Scene";
	const char* LOGIC_CATEGORY = "Logic";
	const char* SUBSYSTEM_CATEGORY = "Subsystem";

	static const float DEFAULT_SMOOTHING_CONSTANT = 50.0f;
	static const float DEFAULT_SNAP_THRESHOLD = 5.0f;



	YumeScene::YumeScene():
		localComponentID_(0x01000000),
		timeScale_(1.0f),
		elapsedTime_(0),
		smoothingConstant_(DEFAULT_SMOOTHING_CONSTANT),
		snapThreshold_(DEFAULT_SNAP_THRESHOLD),
		updateEnabled_(true),
		threadedUpdate_(false)
	{
		// Assign an ID to self so that nodes can refer to this node as a parent
		SetID(GetFreeNodeID());
		NodeAdded(this);

		REGISTER_ENGINE_LISTENER;
	}

	YumeScene::~YumeScene()
	{
		// Remove root-level components first, so that scene subsystems such as the octree destroy themselves. This will speed up
		// the removal of child nodes' components
		RemoveAllComponents();
		RemoveAllChildren();

		// Remove scene reference and owner from all nodes that still exist
		for(YumeMap<unsigned,YumeSceneNode*>::iterator i = localNodes_.begin(); i != localNodes_.end(); ++i)
			i->second->ResetScene();
	}


	void YumeScene::Clear()
	{
		RemoveChildren(true);
		RemoveComponents();

		SetName(YumeString());
		localNodeID_ = 0x01000000;
		localComponentID_ = 0x01000000;

	}

	void YumeScene::SetUpdateEnabled(bool enable)
	{
		updateEnabled_ = enable;
	}

	void YumeScene::SetTimeScale(float scale)
	{
		timeScale_ = Max(scale,M_EPSILON);
	}

	void YumeScene::SetSmoothingConstant(float constant)
	{
		smoothingConstant_ = Max(constant,M_EPSILON);
	}

	void YumeScene::SetSnapThreshold(float threshold)
	{
		snapThreshold_ = Max(threshold,0.0f);
	}

	void YumeScene::SetElapsedTime(float time)
	{
		elapsedTime_ = time;
	}


	YumeSceneNode* YumeScene::GetNode(unsigned id) const
	{
		YumeMap<unsigned,YumeSceneNode*>::const_iterator i = localNodes_.find(id);
		return i != localNodes_.end() ? i->second : 0;
	}

	YumeSceneComponent* YumeScene::GetComponent(unsigned id) const
	{
		YumeMap<unsigned,YumeSceneComponent*>::const_iterator i = localComponents_.find(id);
		return i != localComponents_.end() ? i->second : 0;
	}


	void YumeScene::Update(float timeStep)
	{
		timeStep *= timeScale_;


		elapsedTime_ += timeStep;
	}

	void YumeScene::BeginThreadedUpdate()
	{
		threadedUpdate_ = true;
	}

	void YumeScene::EndThreadedUpdate()
	{
		if(!threadedUpdate_)
			return;

		threadedUpdate_ = false;

		if(!delayedDirtyComponents_.empty())
		{
			for(YumeVector<YumeSceneComponent*>::const_iterator i = delayedDirtyComponents_.begin(); i != delayedDirtyComponents_.end(); ++i)
				(*i)->OnMarkedDirty((*i)->GetNode());
			delayedDirtyComponents_.clear();
		}
	}

	void YumeScene::DelayedMarkedDirty(YumeSceneComponent* component)
	{
		boost::mutex::scoped_lock lock(sceneMutex_);
		delayedDirtyComponents_.push_back(component);
	}


	unsigned YumeScene::GetFreeNodeID()
	{
		for(;;)
		{
			unsigned ret = localNodeID_;
			if(localNodeID_ < LAST_LOCAL_ID)
				++localNodeID_;
			else
				localNodeID_ = FIRST_LOCAL_ID;

			if(localNodes_.find(ret) == localNodes_.end())
				return ret;
		}

	}

	unsigned YumeScene::GetFreeComponentID()
	{
		for(;;)
		{
			unsigned ret = localComponentID_;
			if(localComponentID_ < LAST_LOCAL_ID)
				++localComponentID_;
			else
				localComponentID_ = FIRST_LOCAL_ID;

			if(localComponents_.find(ret) == localComponents_.end())
				return ret;
		}
	}


	void YumeScene::NodeAdded(YumeSceneNode* node)
	{
		if(!node || node->GetScene() == this)
			return;

		// Remove from old scene first
		YumeScene* oldScene = node->GetScene();
		if(oldScene)
			oldScene->NodeRemoved(node);

		node->SetScene(this);

		// If the new node has an ID of zero (default), assign a replicated ID now
		unsigned id = node->GetID();
		if(!id)
		{
			id = GetFreeNodeID();
			node->SetID(id);
		}

		YumeMap<unsigned,YumeSceneNode*>::iterator i = localNodes_.find(id);
		if(i != localNodes_.end() && i->second != node)
		{
			YUMELOG_WARN("Overwriting node with ID " + std::to_string(id));
			NodeRemoved(i->second);
		}
		localNodes_[id] = node;


		// Add already created components and child nodes now
		const YumeVector<SharedPtr<YumeSceneComponent> >::type& components = node->GetComponents();
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components.begin(); i != components.end(); ++i)
			ComponentAdded((*i).get());
		const YumeVector<SharedPtr<YumeSceneNode> >::type& children = node->GetChildren();
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children.begin(); i != children.end(); ++i)
			NodeAdded((*i).get());
	}


	void YumeScene::NodeRemoved(YumeSceneNode* node)
	{
		if(!node || node->GetScene() != this)
			return;

		unsigned id = node->GetID();

		localNodes_.erase(id);

		node->ResetScene();


		// Remove components and child nodes as well
		const YumeVector<SharedPtr<YumeSceneComponent> >::type& components = node->GetComponents();
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components.begin(); i != components.end(); ++i)
			ComponentAdded((*i).get());
		const YumeVector<SharedPtr<YumeSceneNode> >::type& children = node->GetChildren();
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children.begin(); i != children.end(); ++i)
			NodeAdded((*i).get());
	}

	void YumeScene::ComponentAdded(YumeSceneComponent* component)
	{
		if(!component)
			return;

		unsigned id = component->GetID();

		// If the new component has an ID of zero (default), assign a replicated ID now
		if(!id)
		{
			id = GetFreeComponentID();
			component->SetID(id);
		}



		YumeMap<unsigned,YumeSceneComponent*>::iterator i = localComponents_.find(id);
		if(i != localComponents_.end() && i->second != component)
		{
			YUMELOG_WARN("Overwriting component with ID " + std::to_string(id));
			ComponentRemoved(i->second);
		}

		localComponents_[id] = component;


		component->OnSceneSet(this);
	}

	void YumeScene::ComponentRemoved(YumeSceneComponent* component)
	{
		if(!component)
			return;

		unsigned id = component->GetID();

		localComponents_.erase(id);

		component->SetID(0);
		component->OnSceneSet(0);
	}

	void YumeScene::HandleUpdate(float timeStep)
	{
		if(!updateEnabled_)
			return;

		
		Update(timeStep);
	}


}
