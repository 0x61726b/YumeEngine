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
/// File : YumeRenderer.cpp
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeHeaders.h"
#include "Graphics/YumeRenderer.h"
#include "Graphics/YumeRenderWindow.h"
#include "Graphics/YumeGraphics.h"

namespace YumeEngine
{
	YumeRenderer::YumeRenderer()
		: m_pCurrentCaps(0),
		m_pRealCapabilities(0)
	{
	}
	YumeRenderer::~YumeRenderer()
	{
		Shutdown();

		YumeAPIDelete m_pRealCapabilities;
		m_pRealCapabilities = 0;

		m_pCurrentCaps = 0;
	}
	const YumeString& YumeRenderer::GetName()
	{
		return "Hehehe";
	}
	YumeRenderWindow* YumeRenderer::Initialize(bool autoCreate, const YumeString& Title )
	{
		
		return 0;
	}
	//---------------------------------------------------------------------
	void YumeRenderer::UpdateAllRenderTargets(bool swapBuffers)
	{
		RenderTargetPriorityMap::iterator itarg, itargend;
		itargend = mPrioritisedRenderTargets.end();
		for (itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg)
		{
			if (itarg->second->IsActive() && itarg->second->IsAutoUpdated())
				itarg->second->Update(swapBuffers);
		}
	}
	//---------------------------------------------------------------------
	void YumeRenderer::PresentAllBuffers(bool waitForVsnyc)
	{
		RenderTargetPriorityMap::iterator itarg, itargend;
		itargend = mPrioritisedRenderTargets.end();
		for (itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg)
		{
			if (itarg->second->IsActive() && itarg->second->IsAutoUpdated())
				itarg->second->Present(waitForVsnyc);
		}
	}
	//---------------------------------------------------------------------
	void YumeRenderer::Shutdown()
	{
		YumeRenderTarget* primary = 0;
		for (RenderTargetMap::iterator it = mRenderTargets.begin(); it != mRenderTargets.end(); ++it)
		{
			if (!primary && it->second->IsPrimary())
				primary = it->second;
			else
				YumeAPIDelete it->second;
		}
		YumeAPIDelete primary;
		mRenderTargets.clear();

		mPrioritisedRenderTargets.clear();
	}
	//---------------------------------------------------------------------
	YumeRenderTarget* YumeRenderer::GetRenderTarget(const YumeString & name)
	{
		RenderTargetMap::iterator it = mRenderTargets.find(name);
		YumeRenderTarget *ret = NULL;

		if (it != mRenderTargets.end())
		{
			ret = it->second;
		}

		return ret;
	}
	//---------------------------------------------------------------------
	void YumeRenderer::AttachRenderTarget(YumeRenderTarget& target)
	{
		assert(target.GetPriority() < 10);

		mRenderTargets.insert(RenderTargetMap::value_type(target.GetName(), &target));
		mPrioritisedRenderTargets.insert(
			RenderTargetPriorityMap::value_type(target.GetPriority(), &target));
	}
	//---------------------------------------------------------------------
}


///////////////////////////////////////////////////////////////////////////////////