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
		void SetVar(YumeHash key,const YumeVariant& value);
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

		/// Return direction in world space.
		Vector3 GetWorldDirection() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::FORWARD;
		}

		/// Return node's up vector in world space.
		Vector3 GetWorldUp() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::UP;
		}

		/// Return node's right vector in world space.
		Vector3 GetWorldRight() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldRotation_ * Vector3::RIGHT;
		}

		/// Return scale in world space.
		Vector3 GetWorldScale() const
		{
			if(dirty_)
				UpdateWorldTransform();

			return worldTransform_.Scale();
		}

		/// Return scale in world space (for Urho2D).
		Vector2 GetWorldScale2D() const
		{
			Vector3 worldScale = GetWorldScale();
			return Vector2(worldScale.x_,worldScale.y_);
		}

		/// Return world space transform matrix.
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
		const YumeVector<SharedPtr<YumeSceneComponent> >::type GetListeners() const { return listeners_; }

		/// Return a user variable.
		const YumeVariant& GetVar(YumeHash key) const;

		const HashVariantMap& GetVars() const { return vars_; }


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
		void AddComponent(YumeSceneComponent* component,unsigned id);
		unsigned GetNumPersistentChildren() const;
		unsigned GetNumPersistentComponents() const;

	protected:
		HashVariantMap vars_;

	private:
		/// Set enabled/disabled state with optional recursion. Optionally affect the remembered enable state.
		void SetEnabled(bool enable,bool recursive,bool storeSelf);
		/// Create component, allowing UnknownComponent if actual type is not supported. Leave typeName empty if not known.
		YumeSceneComponent* SafeCreateComponent(const YumeString& typeName,YumeHash type,unsigned id);
		/// Recalculate the world transform.
		void UpdateWorldTransform() const;
		/// Remove child node by iterator.
		void RemoveChild(YumeVector<SharedPtr<YumeSceneNode> >::iterator i);
		/// Return child nodes recursively.
		void GetChildrenRecursive(YumeVector<YumeSceneNode*>::type& dest) const;
		/// Return child nodes with a specific component recursively.
		void GetChildrenWithComponentRecursive(YumeVector<YumeSceneNode*>::type& dest,YumeHash  type) const;
		/// Return child nodes with a specific tag recursively.
		void GetChildrenWithTagRecursive(YumeVector<YumeSceneNode*>::type& dest,const YumeString& tag) const;
		/// Return specific components recursively.
		void GetComponentsRecursive(YumeVector<YumeSceneComponent*>::type& dest,YumeHash  type) const;
		/// Clone node recursively.
		YumeSceneNode* CloneRecursive(YumeSceneNode* parent);
		/// Remove a component from this node with the specified iterator.
		void RemoveComponent(YumeVector<SharedPtr<YumeSceneComponent> >::iterator i);

	protected:
		/// World-space transform matrix.
		mutable Matrix3x4 worldTransform_;
		/// World transform needs update flag.
		mutable bool dirty_;
		/// Enabled flag.
		bool enabled_;
		/// Last SetEnabled flag before any SetDeepEnabled.
		bool enabledPrev_;
		/// Parent scene node.
		YumeSceneNode* parent_;
		/// Scene (root node.)
		YumeScene* scene_;
		/// Unique ID within the scene.
		unsigned id_;
		/// Position.
		Vector3 position_;
		/// Rotation.
		Quaternion rotation_;
		/// Scale.
		Vector3 scale_;
		/// World-space rotation.
		mutable Quaternion worldRotation_;
		/// Components.
		YumeVector<SharedPtr<YumeSceneComponent> >::type components_;
		/// Child scene nodes.
		YumeVector<SharedPtr<YumeSceneNode> >::type children_;
		/// Node listeners.
		YumeVector<SharedPtr<YumeSceneComponent> >::type listeners_;
		/// Nodes this node depends on for network updates.
		YumeVector<YumeSceneNode*> dependencyNodes_;
		/// Name.
		YumeString name_;
		/// Tag strings.
		StringVector tags_;
		/// Name hash.
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
			for(Vector<SharedPtr<Node> >::ConstIterator i = children_.Begin(); i != children_.End(); ++i)
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
