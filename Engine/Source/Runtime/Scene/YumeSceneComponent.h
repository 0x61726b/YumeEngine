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
#ifndef __YumeSceneComponent_h__
#define __YumeSceneComponent_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeBase.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeSceneNode;
	class YumeScene;
	class DebugRenderer;

	class YumeAPIExport YumeSceneComponent : public YumeBase
	{
		friend class YumeSceneNode;
		friend class YumeScene;
	public:
		YumeSceneComponent();

		virtual ~YumeSceneComponent();

		virtual void OnSetEnabled() { }

		void SetEnabled(bool enabled);
		void Remove();

		//virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

		bool IsEnabledEffective() const;
		YumeSceneComponent* GetComponent(YumeHash type) const;
		void GetComponents(YumeVector<YumeSceneComponent*>::type& dest,YumeHash type) const;
		template <class T> T* GetComponent() const;
		template <class T> void GetComponents(std::vector<T*>& dest) const;

		void SetID(unsigned id);
		void SetNode(YumeSceneNode* node);

		unsigned GetID() const { return id_; }
		YumeSceneNode* GetNode() const { return node_; }
		YumeScene* GetScene() const;
		bool IsEnabled() const { return enabled_; }


		virtual void OnNodeSet(YumeSceneNode* node);
		virtual void OnSceneSet(YumeScene* scene);
		virtual void OnMarkedDirty(YumeSceneNode* node);
		virtual void OnNodeSetEnabled(YumeSceneNode* node);

		static YumeHash GetType();
		const YumeString& GetTypeName();
		static YumeHash type_;
		YumeString typeName_;

	protected:
		unsigned id_;
		bool enabled_;
		YumeSceneNode* node_;
	};

	template <class T> T* YumeSceneComponent::GetComponent() const { return static_cast<T*>(GetComponent(T::GetType())); }

	template <class T> void YumeSceneComponent::GetComponents(std::vector<T*>& dest) const
	{
		GetComponents(reinterpret_cast<YumeVector<YumeSceneComponent*>::type&>(dest),T::GetType());
	}
}


//----------------------------------------------------------------------------
#endif
