///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeCentrum.cpp
/// Date : 3.9.2015
/// Comments : 
//
///////////////////////////////////////////////////////////////////////////////////

#include "YumeHeaders.h"


#include "Core/YumeCentrum.h"
#include "Graphics/YumeRenderer.h"
#include "Graphics/YumeRenderWindow.h"
#include "Logging/logging.h"
#include "Core/YumeWindowMessageListener.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <log4cplus/initializer.h>

namespace YumeEngine
{
	typedef void(*DLL_START_PLUGIN)(void);
	///--------------------------------------------------------------------------------
	Yume::Yume()
		: m_bStopRendering(false)
	{
		log4cplus::Initializer init;

		

		YUMELOG_DEBUG("Initializing Yume Engine...");

		gYume->pYume = this;


		ShowConfigDialog();
		
		
		
		

	}
	///--------------------------------------------------------------------------------
	YumeRenderWindow* Yume::Initialize(bool Auto, const YumeString& Title)
	{
		Log::InitLogging("D:/Arken/C++/Yume/v2/YumeEngine/logs/Yume.log");
		if(m_pActiveRenderer)
		{
		    YumeRenderWindow* p = m_pActiveRenderer->Initialize(Auto, Title);
		    return p;
		}
		else
		{
		    YUMELOG_DEBUG("No Renderer found.! Abort!");
		}
		return 0;
	}
	///--------------------------------------------------------------------------------
	YumeRenderer * Yume::GetRenderer()
	{
		return m_pActiveRenderer;
	}
	///--------------------------------------------------------------------------------
	bool Yume::ShowConfigDialog()
	{
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		LoadLibraryEx(L"YUME_DIRECT3D11.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		HINSTANCE hInst = GetModuleHandle(L"YUME_DIRECT3D11.dll");
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)GetProcAddress(hInst, "dllStartPlugin");
		pFunc();
#else

#endif
		return true;
	}
	///--------------------------------------------------------------------------------
	bool Yume::Render()
	{
		m_pActiveRenderer->Clear(FBT_COLOUR, new float[4]{ 0,1,0,0 }, 0, 0);
		m_pActiveRenderer->UpdateAllRenderTargets(false);
		m_pActiveRenderer->PresentAllBuffers(true);

		return true;
	}
	///--------------------------------------------------------------------------------
	void Yume::StartRendering()
	{
		while (!m_bStopRendering)
		{
			YumeWindowEvents::Run();

			if (!Render())
				break;
		}
	}
	///--------------------------------------------------------------------------------
	void Yume::StopRendering()
	{
		m_bStopRendering = true;
	}
	///--------------------------------------------------------------------------------
	void Yume::AddRenderer(YumeRenderer* r)
	{
		m_Renderers.push_back(r);

		m_pActiveRenderer = r;
	}
	///--------------------------------------------------------------------------------
	Yume::~Yume()
	{
		YUMELOG_DEBUG("Destroying Yume...");
		m_pAutoWindow = 0;

		

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		HINSTANCE hInst = GetModuleHandle(L"YUME_DIRECT3D11.dll");
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)GetProcAddress(hInst, "dllStop");
		pFunc();
#endif

		Log::StopLogging();
	}
	///--------------------------------------------------------------------------------
}
