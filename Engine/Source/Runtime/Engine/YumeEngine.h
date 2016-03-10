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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeEngine_h__
#define __YumeEngine_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeVariant.h"
#include "Core/YumeTimer.h"
#include "Core/YumeEventHub.h"

#include <boost/shared_ptr.hpp>

#define REGISTER_ENGINE_LISTENER YumeEngine3D::Get()->AddListener(this)
//----------------------------------------------------------------------------
namespace log4cplus {
	class Initializer;
}

namespace YumeEngine
{
	class YumeRHI;
	class YumeEnvironment;
	class YumeDynamicLibrary;
	class YumeTime;
	class YumeIO;
	class YumeResourceManager;
	class YumeObjectFactory;
	class YumeRenderer;
	class YumeWorkQueue;
	class YumeDebugRenderer;
	class YumeInput;

	class YumeAPIExport YumeEngine3D
	{
	public:
		YumeEngine3D();

		virtual ~YumeEngine3D();
		bool Initialize(const VariantMap& variants);

		static YumeEngine3D* Get();

		void Run();

		void Exit();

		void Update();
		void Render();

		bool LoadExternalLibrary(const YumeString& libName);
		void UnloadExternalLibrary(const YumeString& libName);

		void UnloadExternalLibraries();

		bool IsExiting() const { return exiting_; }

		void SetRenderer(YumeRHI* renderer);
		YumeRHI* GetRenderer();

		void FireEvent(YumeEngineEvents evt);
		
		void RegisterFactories();

	public:
		boost::shared_ptr<YumeIO> GetIO() const;
		SharedPtr<YumeTime> GetTimer() const { return timer_; }
		SharedPtr<YumeEnvironment> GetEnvironment() const { return env_; }
		YumeResourceManager* GetResourceManager() const { return resourceManager_; }
		SharedPtr<YumeObjectFactory> GetObjFactory() const { return factory_; }
		SharedPtr<YumeWorkQueue> GetWorkQueue() const { return workQueue_; }
		SharedPtr<YumeDebugRenderer> GetDebugRenderer() const { return debugRenderer_; }
		SharedPtr<YumeRenderer> GetRenderLogic() const { return renderer_; }
		SharedPtr<YumeInput> GetInput() const { return input_; }
		const YumeString& GetRendererName() const;
	private:
		YumeRHI* graphics_;


		boost::shared_ptr<YumeEnvironment> env_;
		boost::shared_ptr<YumeTime> timer_;
		boost::shared_ptr<YumeIO> io_;
		boost::shared_ptr<YumeObjectFactory> factory_;
		boost::shared_ptr<YumeRenderer> renderer_;
		boost::shared_ptr<YumeWorkQueue> workQueue_;
		SharedPtr<YumeDebugRenderer> debugRenderer_;
		SharedPtr<YumeInput> input_;

		log4cplus::Initializer* log4cplusinitializer_;

		YumeResourceManager* resourceManager_;
	private:
		void LimitFrames();
		unsigned inactiveFps_;
		unsigned maxFps_;
		unsigned minFps_;
		float timeStep_;
		float timeStepSmoothing_;
		YumeVector<float>::type lastTimeSteps_;
		YumeHiresTimer frameTimer_;

		typedef YumeVector<YumeDynamicLibrary*>::type ExtLibList;
		ExtLibList extLibs_;

		typedef YumeVector<EngineEventListener*>::type EngineEventListeners;
		EngineEventListeners engineListeners_;
	public:
		void AddListener(EngineEventListener* listener);
		void RemoveListener(EngineEventListener* listener);
	private:
		bool initialized_;
		bool exiting_;

		YumeString rendererName_;
	};
}


//----------------------------------------------------------------------------
#endif
