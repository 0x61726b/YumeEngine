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
#include "Renderer/YumeRenderer.h"

#include "YumeVariant.h"
#include "Core/YumeDynamicLibrary.h"
#include "Core/YumeEnvironment.h"

#include "Core/YumeTimer.h"

#include "Math/YumeVector2.h"
#include "Logging/logging.h"

#include <log4cplus/initializer.h>

#include <boost/filesystem.hpp>

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

		log4cplus::Initializer initialize;
	}

	YumeEngine3D::~YumeEngine3D()
	{

	}
	YumeEngine3D* YumeEngine3D::Get()
	{
		return YumeEngineGlobal;
	}

	bool YumeEngine3D::Initialize()
	{
		if(initialized_)
			return true;

		initialized_ = true;

		env_ = boost::shared_ptr<YumeEnvironment>(YumeAPINew YumeEnvironment);
		timer_ = boost::shared_ptr<YumeTime>(YumeAPINew YumeTime);

		YumeEngine::Log::InitLogging(env_->GetLogFile().generic_string().c_str());

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

		std::string fullRendererLibName;
		YumeString renderer = env_->GetVariant<YumeString>("Renderer");

		if(env_->GetVariant<bool>("testing"))
		{
			renderer = "Null";
		}

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		fullRendererLibName = "Yume" + renderer;
#else
		fullRendererLibName = "libYume" + renderer;
#endif

		//This is where renderer library is getting loaded!
		initialized_ = LoadExternalLibrary(fullRendererLibName);

		if(!initialized_)
			return false;

		graphics_->SetWindowTitle("Yume Engine");
		graphics_->SetWindowPos(Vector2(250,250));

		if(!graphics_->SetGraphicsMode(1280,
			720,
			false,
			false,
			false,
			false,
			false,
			1))
			return false;

		frameTimer_.Reset();

		YUMELOG_INFO("Initialized Yume Engine...");


		return initialized_;
	}

	void YumeEngine3D::Render()
	{
		if(!graphics_->BeginFrame())
			return;

		//Renderer
		Vector4 clearColor(0,0,0,0);

		graphics_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL,clearColor);

		graphics_->EndFrame();
	}


	void YumeEngine3D::Update()
	{
		SDL_Event evt;
		while(SDL_PollEvent(&evt))
			if(evt.type == SDL_QUIT)
				exiting_ = true;
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

	void YumeEngine3D::SetRenderer(YumeRenderer* renderer)
	{
		graphics_ = renderer;
	}

	YumeRenderer* YumeEngine3D::GetRenderer()
	{
		return graphics_;
	}

	void YumeEngine3D::Exit()
	{
		exiting_ = true;

		YUMELOG_INFO("Exiting Yume Engine...");

		UnloadExternalLibraries();

		YUMELOG_INFO("Engine stats: ");
		YUMELOG_INFO("Total frames: " << timer_->GetFrameNumber());
		YUMELOG_INFO("Time elapsed since start: " << timer_->GetElapsedTime());

		YUMELOG_INFO("Exited at time " << timer_->GetTimeStamp());
		YumeEngine::Log::StopLogging();


	}
}
