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

#include "Core/YumeJsFile.h"

#include "YumeUIElement.h"

#include <cef3d/Cef3D.h>

#include <SDL_syswm.h>

#include "Renderer/YumeMiscRenderer.h"

namespace YumeEngine
{
	YumeUI::YumeUI()
		: cef3d_(0),
		renderUI_(true),
		mouseX_(0),
		mouseY_(0),
		lastTimeStep_(0.0f)
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
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeJsFile* cef3DExtension = rm_->PrepareResource<YumeJsFile>("UI/Cef3D.js");
		cef3d_->SetCustomExtensionSourceCode(cef3DExtension->GetContent().c_str());

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
		if(!cef3d_->Initialize(binaryRoot.generic_string().c_str()))
			return false;
		return true;
	}

	void YumeUI::OnBrowserReady(unsigned index)
	{
		YUMELOG_DEBUG("CEF browser " << index << " is ready.");
		for(UIListeners::iterator l = listeners_.begin(); l != listeners_.end(); ++l)
			(*l)->OnBrowserReady(index);


	}

	int YumeUI::CreateBrowser(YumeUIElement* element)
	{
		CefUI::CefRect rect;
		rect.x = element->GetRect().left_;
		rect.y = element->GetRect().top_;
		rect.width = element->GetRect().right_ - element->GetRect().left_;;
		rect.height = element->GetRect().bottom_ - element->GetRect().top_;

		YUMELOG_DEBUG("Creating CEF browser (" << rect.x << "," << rect.y << "," << rect.width << "," << rect.height << ") with URL " << element->GetURL().c_str());

		int index = cef3d_->CreateBrowser(rect,element->GetURL().c_str());

		return index;
	}

	void YumeUI::Update()
	{
		UIElements::iterator It = uiElements_.begin();
		for(;It != uiElements_.end(); ++It)
		{
			if((*It)->GetVisible())
				(*It)->Update();
		}
	}

	void YumeUI::Render()
	{
		if(!renderUI_)
			return;

		UIElements::iterator It = uiElements_.begin();
		for(;It != uiElements_.end(); ++It)
		{
			if((*It)->GetVisible())
				gYume->pRenderer->RenderFullScreenTexture((*It)->GetRect(),(*It)->GetTexture());
		}

	}

	void YumeUI::OnDomEvent(const std::string& str,const std::string& event,const std::string& data)
	{
		DOMEvents domEvent;
		if(event == "InputChecked")
		{
			domEvent = DOMEvents::InputChecked;

			FireDOMEvent(YumeString(str.c_str()),domEvent,YumeString(data.c_str()));
		}
		if(event == "InputValueChanged")
		{
			domEvent = DOMEvents::InputValueChanged;

			FireDOMEvent(YumeString(str.c_str()),domEvent,YumeString(data.c_str()));
		}
	}


	void YumeUI::AddUIElement(YumeUIElement* element)
	{
		uiElements_.push_back(element);
		element->PrepareResources();
	}

	void YumeUI::SendEvent(const YumeString& name,const YumeString& data)
	{
		cef3d_->SendEvent(1,name.c_str(),data.c_str());
	}

	void YumeUI::SetCppProperty(int browserIndex,const YumeString& name,const YumeString& data)
	{
		cef3d_->SetCppProperty(browserIndex,name.c_str(),data.c_str());
	}

	void YumeUI::OnRendererContextReady()
	{
		UIElements::iterator It = uiElements_.begin();
		for(;It != uiElements_.end(); ++It)
		{
			if((*It)->GetVisible())
				(*It)->OnContextReady();
		}

	}

	void YumeUI::OnPaint(int browserIndex,std::vector<CefUI::CefRect>& rectList,const void* buffer,
		int width,
		int height)
	{
		if(uiElements_.size())
		{
			YumeUIElement* element = uiElements_[browserIndex];
			if(element && element->GetType() == Element)
			{
				element->GetTexture()->SetData(0,0,0,width,height,buffer);
			}
			else if(element->GetType() == Overlay && renderUI_)
				element->GetTexture()->SetData(0,0,0,width,height,buffer);
		}
	}

	void YumeUI::HandleBeginFrame(int frameNumber)
	{
		if(!gYume->pEngine->IsExiting())
			cef3d_->RunCefLoop();
	}

	void YumeUI::HandlePostRenderUpdate(float timeStep)
	{
		Update();
	}

	void YumeUI::HandleKeyDown(unsigned key,unsigned mouseButton,int repeat)
	{
		if(!cef3d_->IsInitialized())
			return;

		int modifiers = gYume->pInput->GetModifiers();
		cef3d_->HandleKeyEvent(0,modifiers,key);

		if(!(gYume->pInput->GetKeyDown(KEY_CAPSLOCK)))
		{
			cef3d_->HandleKeyEvent(1,modifiers,key);
		}
	}


	void YumeUI::HandleKeyUp(unsigned key,unsigned mouseButton,int repeat)
	{
		if(!cef3d_->IsInitialized())
			return;
		int modifiers = gYume->pInput->GetModifiers();


		cef3d_->HandleKeyEvent(2,modifiers,key);
	}

	void YumeUI::HandleMouseButtonDown(int native,int button,unsigned buttons)
	{
		if(!cef3d_->IsInitialized())
			return;

		cef3d_->HandleMouseButtonDown(mouseX_,mouseY_,native,button,buttons);
	}

	void YumeUI::HandleMouseButtonUp(int native,int button,unsigned buttons)
	{
		if(!cef3d_->IsInitialized())
			return;
		cef3d_->HandleMouseButtonUp(mouseX_,mouseY_,native,button,buttons);
	}

	void YumeUI::HandleMouseMove(int mouseX,int mouseY,int native,unsigned buttons)
	{
		if(!cef3d_->IsInitialized())
			return;
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

	//
	void YumeUI::FireDOMEvent(const YumeString& domElem,DOMEvents event,const YumeString& data)
	{
		for(DOMListeners::iterator l = domListeners_.begin(); l != domListeners_.end(); ++l)
		{
			(*l)->OnDomEvent(domElem,event,data);
		}
	}

	void YumeUI::AddDOMListener(YumeDOMListener* listener)
	{
		domListeners_.push_back(listener);
	}

	void YumeUI::RemoveDOMListener(YumeDOMListener* listener)
	{
		for(DOMListeners::iterator l = domListeners_.begin(); l != domListeners_.end(); ++l)
		{
			if(*l == listener)
			{
				domListeners_.erase(l);
				break;
			}
		}
	}

	void YumeUI::Shutdown()
	{
		cef3d_->Shutdown();
	}

}
