///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

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
#include "YumeCentrum.h"
#include "YumeRenderer.h"
#include "YumeRenderWindow.h"
#include "YumeLogManager.h"
#include "YumeWindowMessageListener.h"

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
#include <Windows.h>
#endif
namespace YumeEngine
{
	typedef void(*DLL_START_PLUGIN)(void);
	///--------------------------------------------------------------------------------
	template<> YumeCentrum* Singleton<YumeCentrum>::msSingleton = 0;
	YumeCentrum* YumeCentrum::GetPtr(void)
	{
		return msSingleton;
	}
	///--------------------------------------------------------------------------------
	YumeCentrum& YumeCentrum::Get(void)
	{
		assert(msSingleton);  return (*msSingleton);
	}
	///--------------------------------------------------------------------------------
	YumeCentrum::YumeCentrum()
		: m_bStopRendering(false)
	{
		if (YumeLogManager::GetPtr() == 0)
		{
			m_pLogManager = YumeAPINew YumeLogManager();
			m_pLogManager->createLog("Yume.log", true, true);
		}


		ShowConfigDialog();
		
	}
	///--------------------------------------------------------------------------------
	YumeRenderWindow* YumeCentrum::Initialize(bool Auto, const YumeString& Title)
	{
		if(m_pActiveRenderer)
		{
		    YumeRenderWindow* p = m_pActiveRenderer->Initialize(Auto, Title);
		    return p;
		}
		else
		{
		    YumeLogManager::Get().Log("No Renderer found.! Abort!");
		}
		return 0;
	}
	///--------------------------------------------------------------------------------
	bool YumeCentrum::ShowConfigDialog()
	{
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		LoadLibraryEx("YUME_DIRECT3D11.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		HINSTANCE hInst = GetModuleHandle("YUME_DIRECT3D11.dll");
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)GetProcAddress(hInst, "dllStartPlugin");
		pFunc();
#else

#endif
		return true;
	}
	///--------------------------------------------------------------------------------
	void YumeCentrum::StartRendering()
	{
		while (!m_bStopRendering)
		{
			YumeWindowEvents::Run();
		}
	}
	///--------------------------------------------------------------------------------
	void YumeCentrum::StopRendering()
	{
		m_bStopRendering = true;
	}
	///--------------------------------------------------------------------------------
	void YumeCentrum::AddRenderer(YumeRenderer* r)
	{
		m_Renderers.push_back(r);

		m_pActiveRenderer = r;
	}
	///--------------------------------------------------------------------------------
	YumeCentrum::~YumeCentrum()
	{

	}
	///--------------------------------------------------------------------------------
}
