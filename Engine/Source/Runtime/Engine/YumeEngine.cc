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
#include "Core/YumeXmlFile.h"
#include "Core/YumeJsFile.h"
#include "Core/YumeWorkQueue.h"

#include "Renderer/YumeRenderer.h"

#include "Scene/YumeOctree.h"
#include "Renderer/YumeCamera.h"


#include "Renderer/YumeShader.h"
#include "Renderer/YumeShaderVariation.h"
#include "Renderer/YumeTexture2D.h"
#include "Renderer/YumeRenderPass.h"
#include "Renderer/YumeRenderPipeline.h"
#include "Renderer/YumeLight.h"
#include "Renderer/YumeMaterial.h"
#include "Renderer/YumeModel.h"
#include "Renderer/YumeStaticModel.h"
#include "Renderer/YumeAuxRenderer.h"
#include "Renderer/YumeSkybox.h"
#include "Renderer/YumeRendererEnv.h"
#include "Input/YumeInput.h"
#include "UI/YumeUI.h"

#include <boost/filesystem.hpp>
#include <log4cplus/initializer.h>

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

		gYume = (new GlobalSystems);
		gYume->pEngine = this;
	}

	YumeEngine3D::~YumeEngine3D()
	{

	}

	bool YumeEngine3D::Initialize(const VariantMap::type& variants)
	{

		if(initialized_)
			return true;

		initialized_ = true;

		RegisterFactories();

		assert(gYume);

		gYume->pIO = (YumeAPINew YumeIO);
		gYume->pTimer = (YumeAPINew YumeTime);
		gYume->pEnv = (YumeAPINew YumeEnvironment);
		gYume->pWorkSystem = (YumeAPINew YumeWorkQueue);
		gYume->pResourceManager = YumeAPINew YumeResourceManager;

		VariantMap::const_iterator It = variants.begin();

		for(It;It != variants.end(); ++It)
		{
			gYume->pEnv->AddParameter(It->first,It->second);
		}



		if(!gYume->pEnv->GetVariant("turnOffLogging").Get<bool>())
		{
			log4cplusinitializer_ = new log4cplus::Initializer;
			YumeEngine::Log::InitLogging(gYume->pEnv->GetLogFile().generic_string().c_str());
		}
		else
		{
			YumeEngine::Log::ToggleLogging(false);
		}

		unsigned NumThreads = 0;
		gYume->pWorkSystem->CreateThreads(NumThreads);

		YUMELOG_INFO("Initialized environment...Current system time " << gYume->pTimer->GetTimeStamp().c_str());

		String currentOs;

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		currentOs = "Win32";
#elif YUME_PLATFORM == YUME_PLATFORM_LINUX
		currentOs = "Linux";
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
		currentOs = "Osx";
#endif

		YUMELOG_INFO("Engine Config Path: " << gYume->pEnv->GetRoot().generic_string().c_str());
		YUMELOG_INFO("Running on: " << currentOs.c_str());

		YumeString renderer = gYume->pEnv->GetVariant("Renderer").GetString();

		if(gYume->pEnv->GetVariant("testing").Get<bool>())
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
		rendererName_ = renderer;
		if(!initialized_)
			return false;


		gYume->pRHI->SetWindowTitle("Yume Engine");
		gYume->pRHI->SetWindowPos(Vector2(100,50));

		FsPath resourceTree = FsPath(gYume->pEnv->GetVariant("ResourceTree").Get<YumeString>().c_str());
		resourceTree = gYume->pIO->GetBinaryRoot() / resourceTree;

		gYume->pResourceManager->AddResourcePath(resourceTree);

		YumeThreadWrapper::SetMainThread();


		YumeImage* appIcon = gYume->pResourceManager->PrepareResource<YumeImage>("Textures/appIcon.png");
		gYume->pRHI->SetWindowIcon(appIcon);

		if(!gYume->pRHI->SetGraphicsMode(gYume->pEnv->GetVariant("WindowWidth").Get<int>(),
			gYume->pEnv->GetVariant("WindowHeight").Get<int>(),
			gYume->pEnv->GetVariant("Fullscreen").Get<bool>(),
			gYume->pEnv->GetVariant("Borderless").Get<bool>(),
			true,
			gYume->pEnv->GetVariant("Vsync").Get<bool>(),
			gYume->pEnv->GetVariant("TripleBuffer").Get<bool>(),
			gYume->pEnv->GetVariant("MultiSample").Get<int>()))
			return false;
		frameTimer_.Reset();

		gYume->pInput = (YumeAPINew YumeInput);

		gYume->pRenderer = (YumeAPINew YumeRenderer(renderer));
		gYume->pRenderer->Initialize();

		gYume->pDebugRenderer = (new YumeDebugRenderer);


		{
			gYume->pUI = new YumeUI;
			if(!gYume->pUI->Initialize())
				return false;
		}

		YUMELOG_INFO("Initialized Yume Engine...");

		assert(gYume->pEnv);
		assert(gYume->pIO);
		assert(gYume->pRenderer);
		assert(gYume->pResourceManager);
		assert(gYume->pRHI);
		assert(gYume->pTimer);
		assert(gYume->pWorkSystem);
		return initialized_;
	}

	void YumeEngine3D::AddListener(EngineEventListener* listener)
	{
		if(engineListeners_.Contains(listener))
			return;
		engineListeners_.push_back(listener);
	}

	void YumeEngine3D::RemoveListener(EngineEventListener* listener)
	{
		EngineEventListeners::Iterator i = engineListeners_.find(listener);
		if(i != engineListeners_.end())
			engineListeners_.erase(i);
	}

	void YumeEngine3D::FireEvent(YumeEngineEvents evt)
	{
		switch(evt)
		{
		case E_UPDATE:
			for(EngineEventListeners::Iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandleUpdate(gYume->pTimer->GetTimeStep());
			break;
		case E_POSTUPDATE:
			for(EngineEventListeners::Iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandlePostUpdate(gYume->pTimer->GetTimeStep());
			break;
		case R_UPDATE:
			for(EngineEventListeners::Iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandleRenderUpdate(gYume->pTimer->GetTimeStep());
			break;
		case R_POSTUPDATE:
			for(EngineEventListeners::Iterator i = engineListeners_.begin(); i != engineListeners_.end(); ++i)
				(*i)->HandlePostRenderUpdate(gYume->pTimer->GetTimeStep());
			break;
		}
	}

	void YumeEngine3D::RegisterFactories()
	{
		gYume->pObjFactory = (new YumeObjectFactory);


		YumeObjectRegistrar<YumeBase> baseObj(("Base"));
		YumeObjectRegistrar<YumeResource> resourceObj(("Resource"));
		YumeObjectRegistrar<YumeImage> imageObj(("Image"));
		YumeObjectRegistrar<YumeRenderTechnique> techObj(("RenderTechnique"));
		YumeObjectRegistrar<Octree> octreeObj(("Octree"));
		YumeObjectRegistrar<YumeCamera> cameraObj(("Camera"));
		YumeObjectRegistrar<YumeXmlFile> xmlFile(("XmlFile"));
		YumeObjectRegistrar<YumeJsFile> jsFile(("JsFile"));
		YumeObjectRegistrar<YumeLight> light(("Light"));
		YumeObjectRegistrar<YumeMaterial> material(("Material"));
		YumeObjectRegistrar<YumeModel> model(("Model"));
		YumeObjectRegistrar<YumeStaticModel> staticmodel(("StaticModel"));
		YumeObjectRegistrar<YumeDebugRenderer> debugRenderer(("DebugRenderer"));
		YumeObjectRegistrar<YumeSkybox> skybox(("Skybox"));
		YumeObjectRegistrar<YumeRendererEnvironment> rendererEnv(("RendererEnvironment"));
	}
	void YumeEngine3D::Render()
	{
		if(exiting_ || !gYume->pRHI || !gYume->pRHI->BeginFrame())
			return;

		//Renderer
		gYume->pRenderer->Render();
		gYume->pUI->Render();

		gYume->pRHI->EndFrame();
	}


	void YumeEngine3D::Update()
	{
		if(exiting_)
			return;
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

		gYume->pTimer->BeginFrame(timeStep_);



		Update();
		Render();

		LimitFrames();

		gYume->pTimer->EndFrame();
	}

	void YumeEngine3D::LimitFrames()
	{
		if(!initialized_)
			return;

		unsigned maxFps = maxFps_;
		if(gYume->pInput && !gYume->pInput->HasFocus())
			maxFps = Min(60,maxFps);

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

			lastTimeSteps_.erase(0,lastTimeSteps_.size() - timeStepSmoothing_);
			for(unsigned i = 0; i < lastTimeSteps_.size(); ++i)
				timeStep_ += lastTimeSteps_[i];
			timeStep_ /= lastTimeSteps_.size();
		}
		else
			timeStep_ = lastTimeSteps_.back();
	}

	bool YumeEngine3D::LoadExternalLibrary(const YumeString& lib)
	{
		YumeDynamicLibrary* dynLib = gYume->pEnv->LoadDynLib(lib);

		if(dynLib)
		{
			if(!extLibs_.Contains(dynLib))
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
		ExtLibList::Iterator i;

		for(i = extLibs_.begin(); i != extLibs_.end(); ++i)
		{
			if((*i)->GetName() == lib)
			{
				// Call plugin shutdown
				DLL_UNLOAD_MODULE pFunc = (DLL_UNLOAD_MODULE)(*i)->GetSymbol("UnloadModule");
				// this must call uninstallPlugin
				pFunc(this);
				// Unload library (destroyed by DynLibManager)
				gYume->pEnv->UnloadDynLib(*i);
				extLibs_.erase(i);
				return;
			}
		}
	}

	void YumeEngine3D::UnloadExternalLibraries()
	{
		ExtLibList::Iterator i;

		for(i = extLibs_.begin(); i != extLibs_.end(); ++i)
		{
			DLL_UNLOAD_MODULE pFunc = (DLL_UNLOAD_MODULE)(*i)->GetSymbol("UnloadModule");
			// this must call uninstallPlugin
			pFunc(this);
			// Unload library (destroyed by DynLibManager)
			gYume->pEnv->UnloadDynLib(*i);
		}
		extLibs_.clear();
	}


	const YumeString& YumeEngine3D::GetRendererName() const
	{
		return rendererName_;
	}


	void YumeEngine3D::SetRenderer(YumeRHI* renderer)
	{
		gYume->pRHI = (renderer);
	}


	void YumeEngine3D::Exit()
	{
		if(exiting_) //Not normally exited,return
			return;
		exiting_ = true;

		YUMELOG_INFO("Exiting Yume Engine...");


		if(gYume->pRHI)
			gYume->pRHI->Close();

		gYume->pUI->Shutdown();



		//UnloadExternalLibraries();
		//YumeAPIDelete resourceManager_;
		/*graphics_.reset();
		renderer_.reset();*/


		YUMELOG_INFO("Engine stats: ");
		YUMELOG_INFO("Total frames: " << gYume->pTimer->GetFrameNumber());
		YUMELOG_INFO("Time elapsed since start: " << gYume->pTimer->GetElapsedTime());

		YUMELOG_INFO("Exited at time " << gYume->pTimer->GetTimeStamp().c_str());

		if(!gYume->pEnv->GetVariant("turnOffLogging").Get<bool>())
		{
			Log::ToggleLogging(false);
			delete log4cplusinitializer_;
		}

	}
}
