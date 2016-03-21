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

#define REGISTER_ENGINE_LISTENER gYume->pEngine->AddListener(this)
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

	class YumeAPIExport YumeEngine3D : public YumeBase
	{
	public:
		YumeEngine3D();

		virtual ~YumeEngine3D();
		bool Initialize(const VariantMap::type& variants);

		void Run();

		void Exit();

		void Update();
		void Render();

		bool LoadExternalLibrary(const YumeString& libName);
		void UnloadExternalLibrary(const YumeString& libName);

		void UnloadExternalLibraries();

		bool IsExiting() const { return exiting_; }

		void SetRenderer(YumeRHI* renderer);

		void FireEvent(YumeEngineEvents evt);
		
		void RegisterFactories();

	public:
		const YumeString& GetRendererName() const;


		log4cplus::Initializer* log4cplusinitializer_;

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
