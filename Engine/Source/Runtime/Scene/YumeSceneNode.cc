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
#include "YumeSceneNode.h"
#include "YumeSceneComponent.h"
#include "YumeScene.h"

#include "Core/YumeDefaults.h"
#include "Logging/logging.h"



namespace YumeEngine
{
	YumeSceneNode::YumeSceneNode():
		worldTransform_(Matrix3x4::IDENTITY),
		dirty_(false),
		enabled_(true),
		enabledPrev_(true),
		parent_(0),
		scene_(0),
		id_(0),
		position_(Vector3::ZERO),
		rotation_(Quaternion::IDENTITY),
		scale_(Vector3::ONE),
		worldRotation_(Quaternion::IDENTITY),
		name_(EmptyString)
	{

	}

	YumeHash YumeSceneNode::type_ = "SceneNode";
	YumeSceneNode::~YumeSceneNode()
	{
		RemoveAllChildren();
		RemoveAllComponents();

		// Remove from the scene
		if(scene_)
			scene_->NodeRemoved(this);
	}

	void YumeSceneNode::SetName(const YumeString& name)
	{
		if(name != name_)
		{
			name_ = name;
			nameHash_ = (name_);

			// Send change event
			if(scene_)
			{

			}
		}
	}


	void YumeSceneNode::SetPosition(const Vector3& position)
	{
		position_ = position;
		MarkDirty();
	}

	void YumeSceneNode::SetRotation(const Quaternion& rotation)
	{
		rotation_ = rotation;
		MarkDirty();
	}

	void YumeSceneNode::SetDirection(const Vector3& direction)
	{
		SetRotation(Quaternion(Vector3::FORWARD,direction));
	}

	void YumeSceneNode::SetScale(float scale)
	{
		SetScale(Vector3(scale,scale,scale));
	}

	void YumeSceneNode::SetScale(const Vector3& scale)
	{
		scale_ = scale;
		// Prevent exact zero scale e.g. from momentary edits as this may cause division by zero
		// when decomposing the world transform matrix
		if(scale_.x_ == 0.0f)
			scale_.x_ = M_EPSILON;
		if(scale_.y_ == 0.0f)
			scale_.y_ = M_EPSILON;
		if(scale_.z_ == 0.0f)
			scale_.z_ = M_EPSILON;

		MarkDirty();
	}

	void YumeSceneNode::SetTransform(const Vector3& position,const Quaternion& rotation)
	{
		position_ = position;
		rotation_ = rotation;
		MarkDirty();
	}

	void YumeSceneNode::SetTransform(const Vector3& position,const Quaternion& rotation,float scale)
	{
		SetTransform(position,rotation,Vector3(scale,scale,scale));
	}

	void YumeSceneNode::SetTransform(const Vector3& position,const Quaternion& rotation,const Vector3& scale)
	{
		position_ = position;
		rotation_ = rotation;
		scale_ = scale;
		MarkDirty();
	}

	void YumeSceneNode::SetWorldPosition(const Vector3& position)
	{
		SetPosition((parent_ == scene_ || !parent_) ? position : parent_->GetWorldTransform().Inverse() * position);
	}

	void YumeSceneNode::SetWorldRotation(const Quaternion& rotation)
	{
		SetRotation((parent_ == scene_ || !parent_) ? rotation : parent_->GetWorldRotation().Inverse() * rotation);
	}

	void YumeSceneNode::SetWorldDirection(const Vector3& direction)
	{
		Vector3 localDirection = (parent_ == scene_ || !parent_) ? direction : parent_->GetWorldRotation().Inverse() * direction;
		SetRotation(Quaternion(Vector3::FORWARD,localDirection));
	}

	void YumeSceneNode::SetWorldScale(float scale)
	{
		SetWorldScale(Vector3(scale,scale,scale));
	}

	void YumeSceneNode::SetWorldScale(const Vector3& scale)
	{
		SetScale((parent_ == scene_ || !parent_) ? scale : scale / parent_->GetWorldScale());
	}

	void YumeSceneNode::SetWorldTransform(const Vector3& position,const Quaternion& rotation)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
	}

	void YumeSceneNode::SetWorldTransform(const Vector3& position,const Quaternion& rotation,float scale)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	void YumeSceneNode::SetWorldTransform(const Vector3& position,const Quaternion& rotation,const Vector3& scale)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	void YumeSceneNode::Translate(const Vector3& delta,TransformSpace space)
	{
		switch(space)
		{
		case TS_LOCAL:
			// Note: local space translation disregards local scale for scale-independent movement speed
			position_ += rotation_ * delta;
			break;

		case TS_PARENT:
			position_ += delta;
			break;

		case TS_WORLD:
			position_ += (parent_ == scene_ || !parent_) ? delta : parent_->GetWorldTransform().Inverse() * Vector4(delta,0.0f);
			break;
		}

		MarkDirty();
	}

	void YumeSceneNode::Rotate(const Quaternion& delta,TransformSpace space)
	{
		switch(space)
		{
		case TS_LOCAL:
			rotation_ = (rotation_ * delta).Normalized();
			break;

		case TS_PARENT:
			rotation_ = (delta * rotation_).Normalized();
			break;

		case TS_WORLD:
			if(parent_ == scene_ || !parent_)
				rotation_ = (delta * rotation_).Normalized();
			else
			{
				Quaternion worldRotation = GetWorldRotation();
				rotation_ = rotation_ * worldRotation.Inverse() * delta * worldRotation;
			}
			break;
		}

		MarkDirty();
	}

	void YumeSceneNode::RotateAround(const Vector3& point,const Quaternion& delta,TransformSpace space)
	{
		Vector3 parentSpacePoint;
		Quaternion oldRotation = rotation_;

		switch(space)
		{
		case TS_LOCAL:
			parentSpacePoint = GetTransform() * point;
			rotation_ = (rotation_ * delta).Normalized();
			break;

		case TS_PARENT:
			parentSpacePoint = point;
			rotation_ = (delta * rotation_).Normalized();
			break;

		case TS_WORLD:
			if(parent_ == scene_ || !parent_)
			{
				parentSpacePoint = point;
				rotation_ = (delta * rotation_).Normalized();
			}
			else
			{
				parentSpacePoint = parent_->GetWorldTransform().Inverse() * point;
				Quaternion worldRotation = GetWorldRotation();
				rotation_ = rotation_ * worldRotation.Inverse() * delta * worldRotation;
			}
			break;
		}

		Vector3 oldRelativePos = oldRotation.Inverse() * (position_ - parentSpacePoint);
		position_ = rotation_ * oldRelativePos + parentSpacePoint;

		MarkDirty();
	}

	void YumeSceneNode::Yaw(float angle,TransformSpace space)
	{
		Rotate(Quaternion(angle,Vector3::UP),space);
	}

	void YumeSceneNode::Pitch(float angle,TransformSpace space)
	{
		Rotate(Quaternion(angle,Vector3::RIGHT),space);
	}

	void YumeSceneNode::Roll(float angle,TransformSpace space)
	{
		Rotate(Quaternion(angle,Vector3::FORWARD),space);
	}

	bool YumeSceneNode::LookAt(const Vector3& target,const Vector3& up,TransformSpace space)
	{
		Vector3 worldSpaceTarget;

		switch(space)
		{
		case TS_LOCAL:
			worldSpaceTarget = GetWorldTransform() * target;
			break;

		case TS_PARENT:
			worldSpaceTarget = (parent_ == scene_ || !parent_) ? target : parent_->GetWorldTransform() * target;
			break;

		case TS_WORLD:
			worldSpaceTarget = target;
			break;
		}

		Vector3 lookDir = worldSpaceTarget - GetWorldPosition();
		// Check if target is very close, in that case can not reliably calculate lookat direction
		if(lookDir.Equals(Vector3::ZERO))
			return false;
		Quaternion newRotation;
		// Do nothing if setting look rotation failed
		if(!newRotation.FromLookRotation(lookDir,up))
			return false;

		SetWorldRotation(newRotation);
		return true;
	}

	void YumeSceneNode::Scale(float scale)
	{
		Scale(Vector3(scale,scale,scale));
	}

	void YumeSceneNode::Scale(const Vector3& scale)
	{
		scale_ *= scale;
		MarkDirty();
	}

	void YumeSceneNode::SetEnabled(bool enable)
	{
		SetEnabled(enable,false,true);
	}

	void YumeSceneNode::SetDeepEnabled(bool enable)
	{
		SetEnabled(enable,true,false);
	}

	void YumeSceneNode::ResetDeepEnabled()
	{
		SetEnabled(enabledPrev_,false,false);

		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
			(*i)->ResetDeepEnabled();
	}

	void YumeSceneNode::SetEnabledRecursive(bool enable)
	{
		SetEnabled(enable,true,true);
	}


	void YumeSceneNode::MarkDirty()
	{
		YumeSceneNode *cur = this;
		for(;;)
		{
			if(cur->dirty_)
				return;
			cur->dirty_ = true;
			for(YumeVector<YumeSceneComponent*>::iterator i = cur->listeners_.begin(); i != cur->listeners_.end();)
			{
				YumeSceneComponent *c = (*i);
				if(c)
				{
					c->OnMarkedDirty(cur);
					++i;
				}
				else
				{
					*i = cur->listeners_.back();
					cur->listeners_.pop_back();
				}
			}
			YumeVector<SharedPtr<YumeSceneNode> >::iterator i = cur->children_.begin();
			if(i != cur->children_.end())
			{
				YumeSceneNode *next = (*i);
				for(++i; i != cur->children_.end(); ++i)
					(*i)->MarkDirty();
				cur = next;
			}
			else
				return;
		}
	}


	YumeSceneNode* YumeSceneNode::CreateChild(const YumeString& name,unsigned id)
	{
		YumeSceneNode* newNode = CreateChild(id);
		newNode->SetName(name);
		return newNode;
	}

	void YumeSceneNode::AddChild(YumeSceneNode* node,unsigned index)
	{
		// Check for illegal or redundant parent assignment
		if(!node || node == this || node->parent_ == this)
			return;
		// Check for possible cyclic parent assignment
		YumeSceneNode* parent = parent_;
		while(parent)
		{
			if(parent == node)
				return;
			parent = parent->parent_;
		}

		// Keep a shared ptr to the node while transfering
		SharedPtr<YumeSceneNode> nodeShared(node);
		YumeSceneNode* oldParent = node->parent_;
		if(oldParent)
		{
			// If old parent is in different scene, perform the full removal
			if(oldParent->GetScene() != scene_)
				oldParent->RemoveChild(node);
			else
			{
				if(scene_)
				{
					// Otherwise do not remove from the scene during reparenting, just send the necessary change event
				}

				oldParent->children_.erase(std::find(oldParent->children_.begin(),oldParent->children_.end(),nodeShared));
			}
		}

		// Add to the child vector, then add to the scene if not added yet
		children_.push_back(nodeShared);
		if(scene_ && node->GetScene() != scene_)
			scene_->NodeAdded(node);

		node->parent_ = this;
		node->MarkDirty();

		// Send change event
		if(scene_)
		{

		}
	}

	void YumeSceneNode::RemoveChild(YumeSceneNode* node)
	{
		if(!node)
			return;

		for(YumeVector<SharedPtr<YumeSceneNode> >::iterator i = children_.begin(); i != children_.end(); ++i)
		{
			if((*i) == node)
			{
				RemoveChild(i);
				return;
			}
		}
	}

	void YumeSceneNode::RemoveAllChildren()
	{
		RemoveChildren(true);
	}

	void YumeSceneNode::RemoveChildren(bool recursive)
	{
		unsigned numRemoved = 0;

		for(unsigned i = children_.size() - 1; i < children_.size(); --i)
		{
			bool remove = false;
			YumeSceneNode* childNode = children_[i];
			if(recursive)
				childNode->RemoveChildren(true);

			RemoveChild( children_.begin()+i);
		}
	}

	YumeSceneComponent* YumeSceneNode::CreateComponent(YumeHash type,unsigned id)
	{
		// Check that creation succeeds and that the object in fact is a component
		SharedPtr<YumeSceneComponent> newComponent = DynamicCast<YumeSceneComponent>(YumeObjectFactory::Get()->Create(type));
		if(!newComponent)
		{
			YUMELOG_ERROR("Could not create unknown component type " + type);
			return 0;
		}

		AddComponent(newComponent,id);
		return newComponent;
	}

	YumeSceneComponent* YumeSceneNode::GetOrCreateComponent(YumeHash type,unsigned id)
	{
		YumeSceneComponent* oldComponent = GetComponent(type);
		if(oldComponent)
			return oldComponent;
		else
			return CreateComponent(type,id);
	}

	YumeSceneComponent* YumeSceneNode::CloneComponent(YumeSceneComponent* component,unsigned id)
	{
		if(!component)
		{
			YUMELOG_ERROR("Null source component given for CloneComponent");
			return 0;
		}

		YumeSceneComponent* cloneComponent = SafeCreateComponent(component->GetTypeName(),component->GetType(),0);
		if(!cloneComponent)
		{
			YUMELOG_ERROR("Could not clone component ");
			return 0;
		}

		return cloneComponent;
	}

	void YumeSceneNode::RemoveComponent(YumeSceneComponent* component)
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
		{
			if((*i) == component)
			{
				RemoveComponent(i);
				return;
			}
		}
	}

	void YumeSceneNode::RemoveComponent(YumeHash type)
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
		{
			if((*i)->GetType() == type)
			{
				RemoveComponent(i);
				return;
			}
		}
	}

	void YumeSceneNode::RemoveComponents()
	{
		unsigned numRemoved = 0;

		for(unsigned i = components_.size() - 1; i < components_.size(); --i)
		{

			YumeSceneComponent* component = components_[i];
			RemoveComponent(components_.begin() + i);
			++numRemoved;

		}
	}

	void YumeSceneNode::RemoveComponents(YumeHash type)
	{
		unsigned numRemoved = 0;

		for(unsigned i = components_.size() - 1; i < components_.size(); --i)
		{
			if(components_[i]->GetType() == type)
			{
				RemoveComponent(components_.begin() + i);
				++numRemoved;
			}
		}
	}

	void YumeSceneNode::RemoveAllComponents()
	{
		RemoveComponents();
	}

	YumeSceneNode* YumeSceneNode::CloneRecursive(YumeSceneNode* parent)
	{
		//ToDo(arkenthera) implement

		YumeSceneNode* cloneNode = parent->CreateChild(0);

		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			YumeSceneComponent* component = (*i);

			YumeSceneComponent* cloneComponent = cloneNode->CloneComponent(component,0);
		}

		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			YumeSceneNode* node = (*i);

			node->CloneRecursive(cloneNode);
		}

		return cloneNode;
	}

	YumeSceneNode* YumeSceneNode::Clone()
	{
		// The scene itself can not be cloned
		if(this == scene_ || !parent_)
		{
			YUMELOG_ERROR("Can not clone node without a parent");
			return 0;
		}
		YumeSceneNode* clone = CloneRecursive(parent_);

		return clone;
	}

	void YumeSceneNode::Remove()
	{
		if(parent_)
			parent_->RemoveChild(this);
	}

	void YumeSceneNode::SetParent(YumeSceneNode* parent)
	{
		if(parent)
		{
			Matrix3x4 oldWorldTransform = GetWorldTransform();

			parent->AddChild(this);

			if(parent != scene_)
			{
				Matrix3x4 newTransform = parent->GetWorldTransform().Inverse() * oldWorldTransform;
				SetTransform(newTransform.Translation(),newTransform.Rotation(),newTransform.Scale());
			}
			else
			{
				// The root node is assumed to have identity transform, so can disregard it
				SetTransform(oldWorldTransform.Translation(),oldWorldTransform.Rotation(),oldWorldTransform.Scale());
			}
		}
	}

	void YumeSceneNode::SetVar(YumeHash key,const Variant& value)
	{
		vars_[key] = value;
	}

	void YumeSceneNode::AddListener(YumeSceneComponent* component)
	{
		if(!component)
			return;

		// Check for not adding twice
		for(YumeVector<YumeSceneComponent*>::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
		{
			if((*i) == component)
				return;
		}

		listeners_.push_back(component);
		// If the node is currently dirty, notify immediately
		if(dirty_)
			component->OnMarkedDirty(this);
	}

	void YumeSceneNode::RemoveListener(YumeSceneComponent* component)
	{
		for(YumeVector<YumeSceneComponent*>::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
		{
			if((*i) == component)
			{
				listeners_.erase(i);
				return;
			}
		}
	}


	Vector3 YumeSceneNode::LocalToWorld(const Vector3& position) const
	{
		return GetWorldTransform() * position;
	}

	Vector3 YumeSceneNode::LocalToWorld(const Vector4& vector) const
	{
		return GetWorldTransform() * vector;
	}

	Vector3 YumeSceneNode::WorldToLocal(const Vector3& position) const
	{
		return GetWorldTransform().Inverse() * position;
	}

	Vector3 YumeSceneNode::WorldToLocal(const Vector4& vector) const
	{
		return GetWorldTransform().Inverse() * vector;
	}

	unsigned YumeSceneNode::GetNumChildren(bool recursive) const
	{
		if(!recursive)
			return children_.size();
		else
		{
			unsigned allChildren = children_.size();
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
				allChildren += (*i)->GetNumChildren(true);

			return allChildren;
		}
	}

	void YumeSceneNode::GetChildren(YumeVector<YumeSceneNode*>::type& dest,bool recursive) const
	{
		dest.clear();

		if(!recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
				dest.push_back((*i));
		}
		else
			GetChildrenRecursive(dest);
	}

	void YumeSceneNode::GetChildrenWithComponent(YumeVector<YumeSceneNode*>::type& dest,YumeHash type,bool recursive) const
	{
		dest.clear();

		if(!recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
			{
				if((*i)->HasComponent(type))
					dest.push_back((*i));
			}
		}
		else
			GetChildrenWithComponentRecursive(dest,type);
	}

	YumeSceneNode* YumeSceneNode::GetChild(unsigned index) const
	{
		return index < children_.size() ? children_[index].Get() : 0;
	}

	YumeSceneNode* YumeSceneNode::GetChild(const YumeString& name,bool recursive) const
	{
		return GetChild((name),recursive);
	}

	YumeSceneNode* YumeSceneNode::GetChild(const char* name,bool recursive) const
	{
		return GetChild((name),recursive);
	}

	YumeSceneNode* YumeSceneNode::GetChild(YumeHash nameHash,bool recursive) const
	{
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			if((*i)->GetNameHash() == nameHash)
				return (*i);

			if(recursive)
			{
				YumeSceneNode* node = (*i)->GetChild(nameHash,true);
				if(node)
					return node;
			}
		}

		return 0;
	}

	void YumeSceneNode::GetComponents(YumeVector<YumeSceneComponent*>::type& dest,YumeHash type,bool recursive) const
	{
		dest.clear();

		if(!recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
			{
				if((*i)->GetType() == type)
					dest.push_back((*i));
			}
		}
		else
			GetComponentsRecursive(dest,type);
	}

	bool YumeSceneNode::HasComponent(YumeHash type) const
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			if((*i)->GetType() == type)
				return true;
		}
		return false;
	}

	const Variant& YumeSceneNode::GetVar(YumeHash key) const
	{
		VariantMap::const_iterator i = vars_.find(key);
		return i != vars_.end() ? i->second : Variant::EMPTY;
	}


	YumeSceneComponent* YumeSceneNode::GetComponent(YumeHash type,bool recursive) const
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			if((*i)->GetType() == type)
				return (*i);
		}

		if(recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
			{
				YumeSceneComponent* component = (*i)->GetComponent(type,true);
				if(component)
					return component;
			}
		}

		return 0;
	}

	YumeSceneComponent* YumeSceneNode::GetParentComponent(YumeHash type,bool fullTraversal) const
	{
		YumeSceneNode* current = GetParent();
		while(current)
		{
			YumeSceneComponent* soughtComponent = current->GetComponent(type);
			if(soughtComponent)
				return soughtComponent;

			if(fullTraversal)
				current = current->GetParent();
			else
				break;
		}
		return 0;
	}


	void YumeSceneNode::SetID(unsigned id)
	{
		id_ = id;
	}

	void YumeSceneNode::SetScene(YumeScene* scene)
	{
		scene_ = scene;
	}

	void YumeSceneNode::ResetScene()
	{
		SetID(0);
		SetScene(0);
	}


	YumeSceneNode* YumeSceneNode::CreateChild(unsigned id)
	{
		YumeSceneNode* newNode = new YumeSceneNode;

		// If zero ID specified, or the ID is already taken, let the scene assign
		if(scene_)
		{
			if(!id || scene_->GetNode(id))
				id = scene_->GetFreeNodeID();
			newNode->SetID(id);
		}
		else
			newNode->SetID(id);

		AddChild(newNode);
		return newNode;
	}

	void YumeSceneNode::AddComponent(SharedPtr<YumeSceneComponent> component,unsigned id)
	{
		if(!component)
			return;

		components_.push_back(component);

		if(component->GetNode())
			YUMELOG_WARN("Component " + component->GetTypeName() + " already belongs to a node!");

		component->SetNode(this);

		// If zero ID specified, or the ID is already taken, let the scene assign
		if(scene_)
		{
			if(!id || scene_->GetComponent(id))
				id = scene_->GetFreeComponentID();
			component->SetID(id);
			scene_->ComponentAdded(component);
		}
		else
			component->SetID(id);

		component->OnMarkedDirty(this);

		// Send change event
		if(scene_)
		{

		}
	}

	void YumeSceneNode::SetEnabled(bool enable,bool recursive,bool storeSelf)
	{
		// The enabled state of the whole scene can not be changed. SetUpdateEnabled() is used instead to start/stop updates.
		if(GetType() == YumeScene::GetTypeStatic())
		{
			YUMELOG_ERROR("Can not change enabled state of the Scene");
			return;
		}

		if(storeSelf)
			enabledPrev_ = enable;

		if(enable != enabled_)
		{
			enabled_ = enable;

			// Notify listener components of the state change
			for(YumeVector<YumeSceneComponent*>::iterator i = listeners_.begin(); i != listeners_.end();)
			{
				if(*i)
				{
					(*i)->OnNodeSetEnabled(this);
					++i;
				}
				// If listener has expired, erase from list
				else
					i = listeners_.erase(i);
			}

			// Send change event
			if(scene_)
			{

			}

			for(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
			{
				(*i)->OnSetEnabled();

				// Send change event for the component
				if(scene_)
				{

				}
			}
		}

		if(recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::iterator i = children_.begin(); i != children_.end(); ++i)
				(*i)->SetEnabled(enable,recursive,storeSelf);
		}
	}

	YumeSceneComponent* YumeSceneNode::SafeCreateComponent(const YumeString& typeName,YumeHash type,unsigned id)
	{
		return CreateComponent(type,id);
	}

	void YumeSceneNode::UpdateWorldTransform() const
	{
		Matrix3x4 transform = GetTransform();

		// Assume the root node (scene) has identity transform
		if(parent_ == scene_ || !parent_)
		{
			worldTransform_ = transform;
			worldRotation_ = rotation_;
		}
		else
		{
			worldTransform_ = parent_->GetWorldTransform() * transform;
			worldRotation_ = parent_->GetWorldRotation() * rotation_;
		}

		dirty_ = false;
	}

	void YumeSceneNode::RemoveChild(YumeVector<SharedPtr<YumeSceneNode> >::iterator i)
	{
		// Send change event. Do not send when already being destroyed
		YumeSceneNode* child = (*i);



		child->parent_ = 0;
		child->MarkDirty();
		if(scene_)
			scene_->NodeRemoved(child);

		children_.erase(i);
	}

	void YumeSceneNode::GetChildrenRecursive(YumeVector<YumeSceneNode*>::type& dest) const
	{
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			YumeSceneNode* node = (*i);
			dest.push_back(node);
			if(!node->children_.empty())
				node->GetChildrenRecursive(dest);
		}
	}

	void YumeSceneNode::GetChildrenWithComponentRecursive(YumeVector<YumeSceneNode*>::type& dest,YumeHash type) const
	{
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			YumeSceneNode* node = (*i);
			if(node->HasComponent(type))
				dest.push_back(node);
			if(!node->children_.empty())
				node->GetChildrenWithComponentRecursive(dest,type);
		}
	}

	void YumeSceneNode::GetComponentsRecursive(YumeVector<YumeSceneComponent*>::type& dest,YumeHash type) const
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			if((*i)->GetType() == type)
				dest.push_back((*i));
		}
		for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
			(*i)->GetComponentsRecursive(dest,type);
	}

	void YumeSceneNode::RemoveComponent(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i)
	{
		RemoveListener((*i));
		if(scene_)
			scene_->ComponentRemoved((*i));
		(*i)->SetNode(0);
		components_.erase(i);
	}


}
