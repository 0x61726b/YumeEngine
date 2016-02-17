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
/// File : YumeRenderer.h
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeRenderer_h__
#define __YumeRenderer_h__
//---------------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include "Core/YumeCommon.h"

#include "YumeRendererCapabilities.h"
#include "YumeRenderTarget.h"

#include "Core/YumeConfigOption.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	typedef YumeMap< YumeString, YumeRenderTarget * >::type RenderTargetMap;
	typedef YumeMultiMap<unsigned char, YumeRenderTarget * >::type RenderTargetPriorityMap;
	class YumeAPIExport YumeRenderer : public RenderObjAlloc
	{
	public:
		YumeRenderer();

		virtual ~YumeRenderer();

		virtual const YumeString& GetName();

		virtual YumeRenderWindow* Initialize(bool autoCreate, const YumeString& Title = "Yume Engine");

		virtual YumeRendererCapabilities* CreateRendererCapabilities() const = 0;

		virtual YumeRenderWindow* CreateRenderWindow(const YumeString &name, unsigned int width, unsigned int height,
			bool fullScreen, const StrKeyValuePair *miscParams = 0) = 0;

		virtual void PresentAllBuffers(bool waitForVsnyc);
		virtual void UpdateAllRenderTargets(bool swapBuffers);

		virtual void Shutdown();

		virtual void Clear(unsigned int buffers, float color[], float depth, unsigned short stencil) = 0;

		virtual void SetRenderTarget(YumeRenderTarget* target)=0;
		virtual YumeRenderTarget* GetRenderTarget(const YumeString& name);
		virtual void AttachRenderTarget(YumeRenderTarget& target);
	protected:
		YumeRendererCapabilities* m_pCurrentCaps;
		YumeRendererCapabilities* m_pRealCapabilities;
	
		RenderTargetMap mRenderTargets;
		YumeRenderTarget* m_pActiveRenderTarget;
		RenderTargetPriorityMap mPrioritisedRenderTargets;

		bool mVSync;
		unsigned int mVSyncInterval;
		bool mWBuffer;

		DriverVersion mDriverVersion;
	};
}
//---------------------------------------------------------------------------------
#endif
//~End of YumeRenderer.h