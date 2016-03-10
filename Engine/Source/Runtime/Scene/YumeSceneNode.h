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
#ifndef __YumeSceneNode_h__
#define __YumeSceneNode_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Core/YumeVariant.h"
#include "Math/YumeMatrix3x4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeSceneComponent;
	class YumeScene;

	enum TransformSpace
	{
		TS_LOCAL = 0,
		TS_PARENT,
		TS_WORLD
	};

	class YumeAPIExport YumeSceneNode
	{
	public:
		YumeSceneNode();
		virtual ~YumeSceneNode();

		static YumeHash GetType();
		static YumeHash type_;

		void SetName(const YumeString& name);

		//Parent - space
		void SetPosition(const Vector3& position);
		void SetRotation(const Quaternion& rotation);
		void SetDirection(const Vector3& direction);
		void SetScale(float scale);
		void SetScale(const Vector3& scale);

		void SetTransform(const Vector3& position,const Quaternion& rotation);
		void SetTransform(const Vector3& position,const Quaternion& rotation,float scale);
		void SetTransform(const Vector3& position,const Quaternion& rotation,const Vector3& scale);

		//World - space
		void SetWorldPosition(const Vector3& position);
		void SetWorldRotation(const Quaternion& rotation);

		void SetWorldDirection(const Vector3& direction);
		void SetWorldScale(float scale);
		void SetWorldScale(const Vector3& scale);

		void SetWorldTransform(const Vector3& position,const Quaternion& rotation);
		void SetWorldTransform(const Vector3& position,const Quaternion& rotation,float scale);
		void SetWorldTransform(const Vector3& position,const Quaternion& rotation,const Vector3& scale);

		void Translate(const Vector3& delta,TransformSpace space = TS_LOCAL);
		void Rotate(const Quaternion& delta,TransformSpace space = TS_LOCAL);

		void RotateAround(const Vector3& point,const Quaternion& delta,TransformSpace space = TS_LOCAL);



		void Pitch(float angle,TransformSpace space = TS_LOCAL);
		void Yaw(float angle,TransformSpace space = TS_LOCAL);
		void Roll(float angle,TransformSpace space = TS_LOCAL);
		bool LookAt(const Vector3& target,const Vector3& up = Vector3::UP,TransformSpace space = TS_WORLD);
		void Scale(float scale);
		void Scale(const Vector3& scale);

		void SetEnabled(bool enable);
		void SetDeepEnabled(bool enable);
		void ResetDeepEnabled();

		void SetEnabledRecursive(bool enable);

		void MarkDirty();

		YumeSceneNode* CreateChild(const YumeString& name,unsigned id = 0);

		void AddChild(YumeSceneNode* node,unsigned index = M_MAX_UNSIGNED);
		void RemoveChild(YumeSceneNode* node);
		void RemoveAllChildren();
		void RemoveChildren(bool recursive);


		YumeSceneComponent* CreateComponent(YumeHash type,unsigned id = 0);
		YumeSceneComponent* GetOrCreateComponent(YumeHash type,unsigned id = 0);
		YumeSceneComponent* CloneComponent(YumeSceneComponent* component,unsigned id = 0);


		void RemoveComponent(YumeSceneComponent* component);
		void RemoveComponent(YumeHash type);
		void RemoveComponents();
		void RemoveComponents(YumeHash type);
		void RemoveAllComponents();
		YumeSceneNode* Clone();
		void Remove();
		void SetParent(YumeSceneNode* parent);
		void SetVar(YumeHash key,const Variant& value);
		void AddListener(YumeSceneComponent* component);
		void RemoveListener(YumeSceneComponent* component);

		template <class T> T* CreateComponent(unsigned id = 0);
		template <class T> T* GetOrCreateComponent(unsigned id = 0);
		template <class T> void RemoveComponent();
		template <class T> void RemoveComponents();


		unsigned GetID() const { return id_; }


		const YumeString& GetName() const { return name_; }


		YumeHash GetNameHash() const { return nameHash_; }


		YumeSceneNode* GetParent() const { return parent_; }


		YumeScene* GetScene() const { return scene_; }


		bool IsEnabled() const { return enabled_; }


		bool IsEnabledSelf() const { return enabledPrev_; }


		const Vector3& GetPosition() const { return position_; }

		const Quaternion& GetRotation() const { return rotation_; }

		Vector3 GetDirection() const { return rotation_ * Vector3::FORWARD; }

		Vector3 GetUp() const { return rotation_ * Vector3::UP; }

		Vector3 GetRight() const { return rotation_ * Vector3::RIGHT; }

		const Vector3& GetScale() const { return scale_; }


		Matrix3x4 GetTransform() const { return Matrix3x4(position_,rotation_,scale_); }

		Vector3 GetWorldPosition() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldTransform_.Translation();
		}

		Quaternion GetWorldRotation() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_;
		}

		
		Vector3 GetWorldDirection() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::FORWARD;
		}

		
		Vector3 GetWorldUp() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::UP;
		}

		
		Vector3 GetWorldRight() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::RIGHT;
		}

		
		Vector3 GetWorldScale() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldTransform_.Scale();
		}

		
		Vector2 GetWorldScale2D() const
		{
			Vector3 worldScale = GetWorldScale();
			return Vector2(worldScale.x_,worldScale.y_);
		}

		
		const Matrix3x4& GetWorldTransform() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldTransform_;
		}


		Vector3 LocalToWorld(const Vector3& position) const;
		Vector3 LocalToWorld(const Vector4& vector) const;
		Vector3 WorldToLocal(const Vector3& position) const;
		Vector3 WorldToLocal(const Vector4& vector) const;


		bool IsDirty() const { return dirty_; }


		unsigned GetNumChildren(bool recursive = false) const;


		const YumeVector<SharedPtr<YumeSceneNode> >::type& GetChildren() const { return children_; }

		void GetChildren(YumeVector<YumeSceneNode*>::type& dest,bool recursive = false) const;
		void GetChildrenWithComponent(YumeVector<YumeSceneNode*>::type& dest,YumeHash type,bool recursive = false) const;


		YumeSceneNode* GetChild(unsigned index) const;
		YumeSceneNode* GetChild(const YumeString& name,bool recursive = false) const;
		YumeSceneNode* GetChild(const char* name,bool recursive = false) const;
		YumeSceneNode* GetChild(YumeHash nameHash,bool recursive = false) const;

		unsigned GetNumComponents() const { return components_.size(); }


		const YumeVector<SharedPtr<YumeSceneComponent> >::type& GetComponents() const { return components_; }


		void GetComponents(YumeVector<YumeSceneComponent*>::type& dest,YumeHash type,bool recursive = false) const;
		YumeSceneComponent* GetComponent(YumeHash type,bool recursive = false) const;
		YumeSceneComponent* GetParentComponent(YumeHash type,bool fullTraversal = false) const;
		bool HasComponent(YumeHash type) const;
		const YumeVector<YumeSceneComponent*>::type GetListeners() const { return listeners_; }

		
		const Variant& GetVar(YumeHash key) const;

		const VariantMap& GetVars() const { return vars_; }


		template <class T> T* GetDerivedComponent(bool recursive = false) const;
		template <class T> T* GetParentDerivedComponent(bool fullTraversal = false) const;
		template <class T> void GetDerivedComponents(std::vector<T*>& dest,bool recursive = false,bool clearVector = true) const;
		template <class T> void GetChildrenWithComponent(YumeVector<YumeSceneNode*>::type& dest,bool recursive = false) const;
		template <class T> T* GetComponent(bool recursive = false) const;
		template <class T> T* GetParentComponent(bool fullTraversal = false) const;
		template <class T> void GetComponents(std::vector<T*>& dest,bool recursive = false) const;
		template <class T> bool HasComponent() const;

		void SetID(unsigned id);
		void SetScene(YumeScene* scene);
		void ResetScene();

		YumeSceneNode* CreateChild(unsigned id);
		void AddComponent(SharedPtr<YumeSceneComponent> component,unsigned id);
		unsigned GetNumPersistentChildren() const;
		unsigned GetNumPersistentComponents() const;

	protected:
		VariantMap vars_;

	private:
		
		void SetEnabled(bool enable,bool recursive,bool storeSelf);
		
		YumeSceneComponent* SafeCreateComponent(const YumeString& typeName,YumeHash type,unsigned id);
		
		void UpdateWorldTransform() const;
		
		void RemoveChild(YumeVector<SharedPtr<YumeSceneNode> >::iterator i);
		
		void GetChildrenRecursive(YumeVector<YumeSceneNode*>::type& dest) const;
		
		void GetChildrenWithComponentRecursive(YumeVector<YumeSceneNode*>::type& dest,YumeHash  type) const;
		
		void GetChildrenWithTagRecursive(YumeVector<YumeSceneNode*>::type& dest,const YumeString& tag) const;
		
		void GetComponentsRecursive(YumeVector<YumeSceneComponent*>::type& dest,YumeHash  type) const;
		
		YumeSceneNode* CloneRecursive(YumeSceneNode* parent);
		
		void RemoveComponent(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i);

	protected:
		
		mutable Matrix3x4 worldTransform_;
		
		mutable bool dirty_;
		
		bool enabled_;
		
		bool enabledPrev_;
		
		YumeSceneNode* parent_;
		
		YumeScene* scene_;
		
		unsigned id_;
		
		Vector3 position_;
		
		Quaternion rotation_;
		
		Vector3 scale_;
		
		mutable Quaternion worldRotation_;
		
		YumeVector<SharedPtr<YumeSceneComponent> >::type components_;
		
		YumeVector<SharedPtr<YumeSceneNode> >::type children_;
		
		YumeVector<YumeSceneComponent*>::type listeners_;
		
		YumeVector<YumeSceneNode*> dependencyNodes_;
		
		YumeString name_;
		
		StringVector tags_;
		
		YumeHash nameHash_;
	};


	template <class T> T* YumeSceneNode::CreateComponent(unsigned id)
	{
		return static_cast<T*>(CreateComponent(T::GetType(),id));
	}

	template <class T> T* YumeSceneNode::GetOrCreateComponent(unsigned id)
	{
		return static_cast<T*>(GetOrCreateComponent(T::GetTypeStatic(),id));
	}

	template <class T> void YumeSceneNode::RemoveComponent() { RemoveComponent(T::GetType()); }

	template <class T> void YumeSceneNode::RemoveComponents() { RemoveComponents(T::GetType()); }

	template <class T> void YumeSceneNode::GetChildrenWithComponent(std::vector<YumeSceneNode*>& dest,bool recursive) const
	{
		GetChildrenWithComponent(dest,T::GetType(),recursive);
	}

	template <class T> T* YumeSceneNode::GetComponent(bool recursive) const { return static_cast<T*>(GetComponent(T::GetType(),recursive)); }

	template <class T> T* YumeSceneNode::GetParentComponent(bool fullTraversal) const { return static_cast<T*>(GetParentComponent(T::GetType(),fullTraversal)); }

	template <class T> void YumeSceneNode::GetComponents(std::vector<T*>& dest,bool recursive) const
	{
		GetComponents(reinterpret_cast<YumeVector<YumeSceneComponent*>::type&>(dest),T::GetType(),recursive);
	}

	template <class T> bool YumeSceneNode::HasComponent() const { return HasComponent(T::GetType()); }

	template <class T> T* YumeSceneNode::GetDerivedComponent(bool recursive) const
	{
		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			T* component = dynamic_cast<T*>(i->get());
			if(component)
				return component;
		}

		if(recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
			{
				T* component = (*i)->GetDerivedComponent<T>(true);
				if(component)
					return component;
			}
		}

		return 0;
	}

	template <class T> T* YumeSceneNode::GetParentDerivedComponent(bool fullTraversal) const
	{
		YumeSceneNode* current = GetParent();
		while(current)
		{
			T* soughtComponent = current->GetDerivedComponent<T>();
			if(soughtComponent)
				return soughtComponent;

			if(fullTraversal)
				current = current->GetParent();
			else
				break;
		}
		return 0;
	}

	template <class T> void YumeSceneNode::GetDerivedComponents(std::vector<T*>& dest,bool recursive,bool clearVector) const
	{
		if(clearVector)
			dest.clear();

		for(YumeVector<SharedPtr<YumeSceneComponent> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
		{
			T* component = dynamic_cast<T*>(i->get());
			if(component)
				dest.push_back(component);
		}

		if(recursive)
		{
			for(YumeVector<SharedPtr<YumeSceneNode> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
				(*i)->GetDerivedComponents<T>(dest,true,false);
		}
	}
}


//----------------------------------------------------------------------------
#endif
