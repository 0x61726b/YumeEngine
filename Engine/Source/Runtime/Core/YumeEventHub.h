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
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeEventHub_h__
#define __YumeEventHub_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum YumeEngineEvents
	{
		E_UPDATE,
		E_POSTUPDATE,
		R_UPDATE,
		R_POSTUPDATE,
		S_UPDATE,
		S_POSTUPDATE,
		I_KEYDOWN,
		I_KEYUP,
		I_MBUTTONDOWN,
		I_MBUTTONUP,
		R_RENDERTARGETUPDATE,
		R_BEGINVIEWUPDATE,
		R_ENDVIEWUPDATE,
		R_BEGINVIEWRENDER,
		R_ENDVIEWRENDER
	};

	class YumeScene;
	class YumeRenderView;
	class YumeTexture;
	class YumeScene;
	class YumeCamera;
	class YumeRenderable;

	class YumeAPIExport EngineEventListener
	{
	public:
		virtual void HandleUpdate(float timeStep) { };
		virtual void HandlePostUpdate(float timeStep) { };
		virtual void HandleRenderUpdate(float timeStep) { };
		virtual void HandlePostRenderUpdate(float timeStep) { };
	};

	class YumeAPIExport RHIEventListener
	{
	public:
		virtual void HandleGraphicsMode() { };
	};

	class YumeAPIExport RendererEventListener
	{
	public:
		virtual void HandleRenderTargetUpdate() { };
		virtual void HandleBeginViewUpdate(YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera) { };
		virtual void HandleEndViewUpdate(YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera) { };
		virtual void HandleBeginViewRender(YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera) { };
		virtual void HandleEndViewRender(YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera) { };
	};

	class YumeAPIExport InputEventListener
	{
	public:
		virtual void HandleKeyDown(unsigned key,unsigned mouseButton,int repeat) { }
		virtual void HandleKeyUp(unsigned key,unsigned mouseButton,int repeat) { }
		virtual void HandleMouseButtonDown(int modifiers,int button,unsigned buttons) { }
		virtual void HandleMouseButtonUp(int modifiers,int button,unsigned buttons) { }
		virtual void HandleMouseMove(int mouseX,int mouseY,int modifiers,unsigned buttons) { }

	};
	class YumeAPIExport SceneEventListener
	{
	public:
		virtual void HandleSceneUpdate(YumeScene* scene,float timeStep) { };
		virtual void HandleScenePostUpdate(YumeScene* scene,float timeStep) { };
	};
	class YumeAPIExport YumeTimerEventListener
	{
	public:
		virtual ~YumeTimerEventListener() { };

		virtual void HandleBeginFrame(int frameNumber) { };
		virtual void HandleEndFrame(int frameNumber) { };
	};

	class YumeAPIExport UIEventListener
	{
	public:
		virtual void OnBrowserReady(unsigned int browserIndex) = 0;

	};
}


//----------------------------------------------------------------------------
#endif
