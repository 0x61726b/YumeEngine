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
#include "YumeHeaders.h"
#include "YumeUI.h"
#include "Engine/YumeEngine.h"

#include "Renderer/YumeRHI.h"
#include "Renderer/YumeTexture2D.h"
#include "Logging/logging.h"

#include <cef3d/Cef3D.h>

#include <SDL_syswm.h>

namespace YumeEngine
{
	YumeUI::YumeUI()
		: cef3d_(0),
		renderUI_(true),
		mouseX_(0),
		mouseY_(0)
	{
		gYume->pTimer->AddTimeEventListener(this);
		gYume->pEngine->AddListener(this);

		SDL_SysWMinfo sysInfo;

		SDL_VERSION(&sysInfo.version);
		SDL_GetWindowWMInfo(gYume->pRHI->GetWindow(),&sysInfo);

		void* windowHandle = 0;
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		windowHandle = sysInfo.info.win.window;
		cef3d_ = new CefUI::Cef3D(this,(HWND)windowHandle);
#else
		cef3d_ = new CefUI::Cef3D(this,0);
#endif

		gYume->pInput->AddListener(this);
	}

	YumeUI::~YumeUI()
	{
		delete cef3d_;
	}

	bool YumeUI::Initialize()
	{
		FsPath binaryRoot = gYume->pIO->GetBinaryRoot();

		YUMELOG_INFO("Initializing Cef...");
		if(cef3d_->Initialize(binaryRoot.generic_string().c_str()))
		{
			if(renderUI_)
			{
				IntVector2 screen = gYume->pRHI->GetRenderTargetDimensions();
				CefUI::CefRect rect(0,0,screen.x_,screen.y_);
				CreateBrowser(rect,"http://www.google.com",true);

				return true;
			}
			else
				return true;
		}
		else
			return false;
		return false;
	}

	void YumeUI::OnContextReady()
	{

	}

	void YumeUI::OnBrowserReady(unsigned index)
	{
		YUMELOG_DEBUG("CEF browser " << index << " is ready.");
		for(UIListeners::iterator l = listeners_.begin(); l != listeners_.end(); ++l)
			(*l)->OnBrowserReady(index);
	}

	int YumeUI::CreateBrowser(const CefUI::CefRect& rect,const std::string& url,bool isUiElement)
	{
		YUMELOG_DEBUG("Creating CEF browser (" << rect.x << "," << rect.y << "," << rect.width << "," << rect.height << ") with URL " << url.c_str());
		SharedPtr<YumeTexture2D> texture(gYume->pRHI->CreateTexture2D());
		texture->SetSize(rect.width,rect.height,gYume->pRHI->GetBGRAFormatNs(),TEXTURE_DYNAMIC);
		int index = cef3d_->CreateBrowser(rect,url);


		browserRects_.insert(MakePair(MakePair(index,isUiElement),MakePair(rect,texture)));
		return browserRects_.size() - 1;
	}

	void YumeUI::Render()
	{
		if(!renderUI_)
			return;

		BrowserElements::iterator It = browserRects_.begin();
		for(;It != browserRects_.end(); ++It)
		{
			if(!It->first.second) //if not UI element continue
				continue;

			CefUI::CefRect rect = It->second.first;

			gYume->pDebugRenderer->RenderInternalTexture(IntRect(rect.x,rect.y,rect.width + rect.x,rect.height + rect.y),It->second.second);
		}
	}

	void YumeUI::OnPaint(int browserIndex,std::vector<CefUI::CefRect>& rectList,const void* buffer,
		int width,
		int height)
	{
		if(browserRects_.size())
		{
			BrowserElements::iterator It = browserRects_.find(MakePair(browserIndex,true));
			if(It != browserRects_.end())
				It->second.second->SetData(0,0,0,width,height,buffer);
		}
	}

	void YumeUI::HandleBeginFrame(int frameNumber)
	{
		if(!gYume->pEngine->IsExiting())
			cef3d_->RunCefLoop();
	}

	void YumeUI::HandleKeyDown(unsigned key,unsigned mouseButton,int repeat)
	{
		int modifiers = gYume->pInput->GetModifiers();
		cef3d_->HandleKeyEvent(0,modifiers,key);

		if(!(gYume->pInput->GetKeyDown(KEY_CAPSLOCK)))
		{
			cef3d_->HandleKeyEvent(1,modifiers,key);
		}
	}


	void YumeUI::HandleKeyUp(unsigned key,unsigned mouseButton,int repeat)
	{
		int modifiers = gYume->pInput->GetModifiers();


		cef3d_->HandleKeyEvent(2,modifiers,key);
	}

	void YumeUI::HandleMouseButtonDown(int native,int button,unsigned buttons)
	{
		cef3d_->HandleMouseButtonDown(mouseX_,mouseY_,native,button,buttons);
	}

	void YumeUI::HandleMouseButtonUp(int native,int button,unsigned buttons)
	{
		cef3d_->HandleMouseButtonUp(mouseX_,mouseY_,native,button,buttons);
	}

	void YumeUI::HandleMouseMove(int mouseX,int mouseY,int native,unsigned buttons)
	{
		mouseX_ = mouseX;
		mouseY_ = mouseY;
		cef3d_->HandleMouseMove(mouseX,mouseY,native,buttons);
	}

	void YumeUI::SetUIEnabled(bool enable)
	{
		renderUI_ = enable;
	}


	void YumeUI::AddListener(UIEventListener* listener)
	{
		listeners_.push_back(listener);
	}

	void YumeUI::RemoveListener(UIEventListener* listener)
	{
		for(UIListeners::iterator l = listeners_.begin(); l != listeners_.end(); ++l)
		{
			if(*l == listener)
			{
				listeners_.erase(l);
				break;
			}
		}
	}


	void YumeUI::Shutdown()
	{
		cef3d_->Shutdown();
	}

}
