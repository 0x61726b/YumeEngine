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
#ifndef __YumeScene_h__
#define __YumeScene_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeSceneNode.h"
#include "Core/YumeEventHub.h"

#include "Core/YumeMutex.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	static const unsigned FIRST_LOCAL_ID = 0x01000000;
	static const unsigned LAST_LOCAL_ID = 0xffffffff;
	class YumeAPIExport YumeScene : public YumeSceneNode,public EngineEventListener
	{
	public:
		using YumeSceneNode::GetComponent;
	public:
		YumeScene();
		virtual ~YumeScene();

		virtual YumeHash GetType() { return type_; };
		static YumeHash GetTypeStatic() { return type_; };
		static YumeHash type_;

		void Clear();
		void SetUpdateEnabled(bool enable);
		void SetTimeScale(float scale);
		void SetElapsedTime(float time);
		void SetSmoothingConstant(float constant);
		void SetSnapThreshold(float threshold);

		YumeSceneNode* GetNode(unsigned id) const;
		YumeSceneComponent* GetComponent(unsigned id) const;

		bool IsUpdateEnabled() const { return updateEnabled_; }

		float GetTimeScale() const { return timeScale_; }
		float GetElapsedTime() const { return elapsedTime_; }

		void Update(float timeStep);

		void BeginThreadedUpdate();
		void EndThreadedUpdate();
		void DelayedMarkedDirty(YumeSceneComponent* component);

		bool IsThreadedUpdate() const { return threadedUpdate_; }

		unsigned GetFreeNodeID();
		unsigned GetFreeComponentID();


		void NodeAdded(YumeSceneNode* node);
		void NodeRemoved(YumeSceneNode* node);
		void ComponentAdded(YumeSceneComponent* component);
		void ComponentRemoved(YumeSceneComponent* component);
		void SetVarNamesAttr(const YumeString& value);
		YumeString GetVarNamesAttr() const;

		void HandleUpdate(float timeStep);

	private:

		YumeMap<unsigned,YumeSceneNode*>::type localNodes_;
		YumeMap<unsigned,YumeSceneComponent*>::type localComponents_;

		YumeVector<YumeSceneComponent*>::type delayedDirtyComponents_;
		Mutex sceneMutex_;
		VariantMap smoothingData_;

		unsigned localNodeID_;
		unsigned localComponentID_;

		float timeScale_;
		float elapsedTime_;
		float snapThreshold_;
		bool updateEnabled_;
		bool threadedUpdate_;
		float smoothingConstant_;
	};
}


//----------------------------------------------------------------------------
#endif
