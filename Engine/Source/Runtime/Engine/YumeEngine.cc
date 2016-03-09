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
#include "YumeHeaders.h"
#include "Engine/YumeEngine.h"
#include "Renderer/YumeGraphicsApi.h"
#include "Renderer/YumeRHI.h"

#include "YumeVariant.h"
#include "Core/YumeDynamicLibrary.h"
#include "Core/YumeEnvironment.h"

#include "Renderer/YumeImage.h"
#include "Core/YumeFile.h"

#include "Renderer/YumeResourceManager.h"

#include "Core/YumeTimer.h"

#include "Math/YumeVector2.h"
#include "Logging/logging.h"

#include "Core/YumeIO.h"
#include "Core/YumeDefaults.h"
#include "Core/YumeBase.h"

#include "Renderer/YumeRenderer.h"

#include "Scene/YumeOctree.h"


#include "Renderer/YumeShader.h"
#include "Renderer/YumeShaderVariation.h"
#include "Renderer/YumeTexture2D.h"
#include "Renderer/YumeRenderPass.h"

#include <boost/filesystem.hpp>
#include <log4cplus/initializer.h>
#include <SDL.h>

YumeEngine::YumeEngine3D* YumeEngineGlobal = 0;

namespace YumeEngine
{
	typedef void(*DLL_LOAD_MODULE)(YumeEngine3D*);
	typedef void(*DLL_UNLOAD_MODULE)(YumeEngine3D*);

	YumeEngine3D::YumeEngine3D()
		: exiting_(false),
		initialized_(false),
		inactiveFps_(60),
		maxFps_(200),
		minFps_(10),
		timeStepSmoothing_(2),
		timeStep_(0)
	{
		YumeEngineGlobal = this;
	}

	YumeEngine3D::~YumeEngine3D()
	{

	}
	YumeEngine3D* YumeEngine3D::Get()
	{
		return YumeEngineGlobal;
	}

	bool YumeEngine3D::Initialize(const VariantMap& variants)
	{

		if(initialized_)
			return true;

		initialized_ = true;

		RegisterFactories();


		io_ = boost::shared_ptr<YumeIO>(YumeAPINew YumeIO);
		timer_ = boost::shared_ptr<YumeTime>(YumeAPINew YumeTime);
		env_ = boost::shared_ptr<YumeEnvironment>(YumeAPINew YumeEnvironment);
		resourceManager_ = YumeAPINew YumeResourceManager;
		VariantMap::const_iterator It = variants.begin();

		for(It;It != variants.end(); ++It)
		{
			env_->AddParameter(It->first,It->second);
		}



		if(!env_->GetVariant("turnOffLogging").Get<bool>())
		{
			log4cplusinitializer_ = new log4cplus::Initializer;
			YumeEngine::Log::InitLogging(env_->GetLogFile().generic_string().c_str());
		}
		else
		{
			YumeEngine::Log::ToggleLogging(false);
		}



		YUMELOG_INFO("Initialized environment...Current system time " << timer_->GetTimeStamp());

		std::string currentOs;

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		currentOs = "Win32";
#elif YUME_PLATFORM == YUME_PLATFORM_LINUX
		currentOs = "Linux";
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
		currentOs = "Osx";
#endif

		YUMELOG_INFO("Engine Config Path: " << env_->GetRoot().generic_string().c_str());
		YUMELOG_INFO("Running on: " << currentOs.c_str());

		YumeString renderer = env_->GetVariant("Renderer").GetString();

		if(env_->GetVariant("testing").Get<bool>())
		{
			renderer = "Null";
		}

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		rendererName_ = "Yume" + renderer;
#else
		rendererName_ = "libYume" + renderer;
#endif

		//This is where renderer library is getting loaded!
		initialized_ = LoadExternalLibrary(rendererName_);
		renderer_ = boost::shared_ptr<YumeRenderer>(YumeAPINew YumeRenderer(graphics_));

		if(!initialized_)
			return false;

		graphics_->SetWindowTitle("Yume Engine");
		graphics_->SetWindowPos(Vector2(250,250));

		FsPath resourceTree = FsPath(env_->GetVariant("ResourceTree").Get<YumeString>());
		resourceTree = io_->GetBinaryRoot() / "Yume" / resourceTree;

		resourceManager_->AddResourcePath(resourceTree);

		YumeThreadWrapper::SetMainThread();

		SharedPtr<YumeImage> appIcon = resourceManager_->PrepareResource<YumeImage>("Textures/appIcon.png");
		graphics_->SetWindowIcon(appIcon.get());

		if(!graphics_->SetGraphicsMode(env_->GetVariant("WindowWidth").Get<int>(),
			env_->GetVariant("WindowHeight").Get<int>(),
			env_->GetVariant("Fullscreen").Get<bool>(),
			env_->GetVariant("Borderless").Get<bool>(),
			true,
			env_->GetVariant("Vsync").Get<bool>(),
			env_->GetVariant("TripleBuffer").Get<bool>(),
			env_->GetVariant("MultiSample").Get<int>()))
			return false;
		frameTimer_.Reset();

		//SharedPtr<YumeTexture2D> earth = resourceManager_->PrepareResource<YumeTexture2D>("Textures/Earth_Diffuse.dds");

		renderer_->Initialize();

		YUMELOG_INFO("Initialized Yume Engine...");


		return initialized_;
	}

	void YumeEngine3D::AddListener(EngineEventListener* listener)
	{
		if(std::find(engineListeners_.begin(),engineListeners_.end(),listener) != engineListeners_.end())
			return;
		engineListeners_.push_back(listener);
	}

	void YumeEngine3D::RemoveListener(EngineEventListener* listener)
	{
		EngineEventListeners::iterator i = std::find(engineListeners_.begin(),engineListeners_.end(),listener);
		if(i != engineListeners_.end())
			engineListeners_.erase(i);
	}

	void YumeEngine3D::FireEvent(YumeEngineEvents evt)
	{
		switch(evt)
		{
		case E_UPDATE:
			for(EngineEventListeners::iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandleUpdate(timer_->GetTimeStep());
			break;
		case E_POSTUPDATE:
			for(EngineEventListeners::iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandlePostUpdate(timer_->GetTimeStep());
			break;
		case R_UPDATE:
			for(EngineEventListeners::iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandleRenderUpdate(timer_->GetTimeStep());
			break;
		case R_POSTUPDATE:
			for(EngineEventListeners::iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandlePostRenderUpdate(timer_->GetTimeStep());
			break;
		}
	}

	void YumeEngine3D::RegisterFactories()
	{
		factory_ = SharedPtr<YumeObjectFactory>(new YumeObjectFactory);


		YumeObjectRegistrar<YumeBase> baseObj(("Base"));
		YumeObjectRegistrar<YumeResource> resourceObj(("Resource"));
		YumeObjectRegistrar<YumeImage> imageObj(("Image"));
		YumeObjectRegistrar<YumeRenderTechnique> techObj(("RenderTechnique"));
		YumeObjectRegistrar<Octree> octreeObj(("Octree"));

	}
	void YumeEngine3D::Render()
	{
		if(!graphics_->BeginFrame())
			return;

		//Renderer
		renderer_->Render();

		graphics_->EndFrame();
	}


	void YumeEngine3D::Update()
	{
		SDL_Event evt;
		while(SDL_PollEvent(&evt))
			if(evt.type == SDL_QUIT)
				exiting_ = true;

		FireEvent(E_UPDATE);
		FireEvent(E_POSTUPDATE);
		FireEvent(R_UPDATE);
		FireEvent(R_POSTUPDATE);
	}

	void YumeEngine3D::Run()
	{
		assert(initialized_);

		if(exiting_)
			return;

		timer_->BeginFrame(timeStep_);

		Update();
		Render();

		LimitFrames();

		timer_->EndFrame();
	}

	void YumeEngine3D::LimitFrames()
	{
		if(!initialized_)
			return;

		long long elapsed = 0;

		if(maxFps_ != 0)
		{
			long long targetMax = 1000000LL / maxFps_;

			for(;;)
			{
				elapsed = frameTimer_.GetUSec(false);
				if(elapsed >= targetMax)
					break;

				if(targetMax - elapsed >= 1000LL)
				{
					unsigned sleepTime = (unsigned)((targetMax - elapsed) / 1000LL);
					YumeTime::Sleep(sleepTime);
				}
			}
		}

		elapsed = frameTimer_.GetUSec(true);

		if(minFps_)
		{
			long long targetMin = 1000000LL / minFps_;
			if(elapsed > targetMin)
				elapsed = targetMin;
		}

		timeStep_ = 0.0f;
		lastTimeSteps_.push_back(elapsed / 1000000.0f);
		if(lastTimeSteps_.size() > timeStepSmoothing_)
		{
			// If the smoothing configuration was changed, ensure correct amount of samples


			lastTimeSteps_.erase(lastTimeSteps_.begin() + lastTimeSteps_.size() - timeStepSmoothing_,lastTimeSteps_.end());
			for(unsigned i = 0; i < lastTimeSteps_.size(); ++i)
				timeStep_ += lastTimeSteps_[i];
			timeStep_ /= lastTimeSteps_.size();
		}
		else
			timeStep_ = lastTimeSteps_.back();

	}

	bool YumeEngine3D::LoadExternalLibrary(const YumeString& lib)
	{
		YumeDynamicLibrary* dynLib = env_->LoadDynLib(lib);

		if(dynLib)
		{
			if(std::find(extLibs_.begin(),extLibs_.end(),dynLib) == extLibs_.end())
			{
				extLibs_.push_back(dynLib);

				DLL_LOAD_MODULE pFunc = (DLL_LOAD_MODULE)dynLib->GetSymbol("LoadModule");

				if(!pFunc)
				{
					YUMELOG_ERROR("Error loading address of LoadModule in an external library..." << lib.c_str());
					return false;
				}

				pFunc(this);
			}
			return true;
		}
		else
		{
			return false;
		}

	}

	void YumeEngine3D::UnloadExternalLibrary(const YumeString& lib)
	{
		ExtLibList::iterator i;

		for(i = extLibs_.begin(); i != extLibs_.end(); ++i)
		{
			if((*i)->GetName() == lib)
			{
				// Call plugin shutdown
				DLL_UNLOAD_MODULE pFunc = (DLL_UNLOAD_MODULE)(*i)->GetSymbol("UnloadModule");
				// this must call uninstallPlugin
				pFunc(this);
				// Unload library (destroyed by DynLibManager)
				env_->UnloadDynLib(*i);
				extLibs_.erase(i);
				return;
			}
		}
	}

	void YumeEngine3D::UnloadExternalLibraries()
	{
		ExtLibList::iterator i;

		for(i = extLibs_.begin(); i != extLibs_.end(); ++i)
		{
			DLL_UNLOAD_MODULE pFunc = (DLL_UNLOAD_MODULE)(*i)->GetSymbol("UnloadModule");
			// this must call uninstallPlugin
			pFunc(this);
			// Unload library (destroyed by DynLibManager)
			env_->UnloadDynLib(*i);
		}
		extLibs_.clear();
	}


	const YumeString& YumeEngine3D::GetRendererName() const
	{
		return rendererName_;
	}


	boost::shared_ptr<YumeIO> YumeEngine3D::GetIO() const
	{
		return io_;
	}

	void YumeEngine3D::SetRenderer(YumeRHI* renderer)
	{
		graphics_ = renderer;
	}

	YumeRHI* YumeEngine3D::GetRenderer()
	{
		return graphics_;
	}

	void YumeEngine3D::Exit()
	{
		exiting_ = true;

		YUMELOG_INFO("Exiting Yume Engine...");

		YumeAPIDelete resourceManager_;

		UnloadExternalLibraries();

		YUMELOG_INFO("Engine stats: ");
		YUMELOG_INFO("Total frames: " << timer_->GetFrameNumber());
		YUMELOG_INFO("Time elapsed since start: " << timer_->GetElapsedTime());

		YUMELOG_INFO("Exited at time " << timer_->GetTimeStamp());

		if(!env_->GetVariant("turnOffLogging").Get<bool>())
		{
			delete log4cplusinitializer_;
		}

	}
}
