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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "Engine/YumeEngine.h"
#include "Renderer/YumeGraphicsApi.h"
#include "Renderer/YumeRenderer.h"

#include "Core/YumeEnvironment.h"

#include "Math/YumeVector2.h"
#include "Logging/logging.h"

#include <log4cplus/initializer.h>

#include <boost/filesystem.hpp>

#include <SDL.h>

YumeEngine::YumeEngine3D* YumeEngineGlobal = 0;

namespace YumeEngine
{
	typedef void(*DLL_LOAD_MODULE)(YumeEngine3D*);

	YumeEngine3D::YumeEngine3D()
		: exiting_(false),
		initialized_(false)
	{
		YumeEngineGlobal = this;

		log4cplus::Initializer initialize;
	}

	YumeEngine3D* YumeEngine3D::Get()
	{
		return YumeEngineGlobal;
	}

	bool YumeEngine3D::Initialize()
	{
		if(initialized_)
			return true;

		YumeEngine::Log::InitLogging("D:/Arken/C++/Yume/v2/YumeEngine/logs/Yume.log");

		YUMELOG_INFO("Initializing Yume Engine...");

		env_ = boost::shared_ptr<YumeEnvironment>(YumeAPINew YumeEnvironment);


		

		LoadLibraryEx(L"YumeDirect3D11.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		HINSTANCE hInst = GetModuleHandle(L"YumeDirect3D11.dll");
		DLL_LOAD_MODULE pFunc = (DLL_LOAD_MODULE)GetProcAddress(hInst, "LoadGraphicsModule");
		pFunc(this);


		graphics_->SetWindowTitle("Yume Engine");
		graphics_->SetWindowPos(Vector2(250,250));

		if(!graphics_->SetGraphicsMode(1280,720,false,false,true,true,false,1))
			return false;

		initialized_ = true;

		YUMELOG_INFO("Engine initializing succesfull..");

		return initialized_;
	}

	void YumeEngine3D::Render()
	{
		if(!graphics_->BeginFrame())
			return;

		//Renderer
		Vector4 clearColor(0,0,0,0);

		graphics_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL, clearColor);

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

		Update();
		Render();
	}

	void YumeEngine3D::SetRenderer(YumeRenderer* renderer)
	{
		graphics_ = boost::shared_ptr<YumeRenderer>(renderer);
	}

	boost::shared_ptr<YumeRenderer> YumeEngine3D::GetRenderer()
	{
		return graphics_;
	}

	void YumeEngine3D::Exit()
	{
		YumeEngine::Log::StopLogging();


#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		HINSTANCE hInst = GetModuleHandle(L"YumeDirect3D11.dll");
		DLL_LOAD_MODULE pFunc = (DLL_LOAD_MODULE)GetProcAddress(hInst, "UnloadGraphicsModule");
		pFunc(this);
#endif

		exiting_ = true;
	}
}
