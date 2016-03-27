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
#include "YumeInput.h"


#include "Engine/YumeEngine.h"
#include "Renderer/YumeRHI.h"

#include "Logging/logging.h"

#include <SDL_syswm.h>


namespace YumeEngine
{
	int ConvertSDLKeyCode(int keySym,int scanCode)
	{
		if(scanCode == SCANCODE_AC_BACK)
			return KEY_ESC;
		else
		{
			return SDL_toupper(keySym);
		}
	}


	YumeInput::YumeInput()
		: mouseButtonDown_(0),
		mouseButtonPress_(0),
		lastVisibleMousePosition_(MOUSE_POSITION_OFFSCREEN),
		mouseMoveWheel_(0),
		windowID_(0),
		toggleFullscreen_(true),
		mouseVisible_(true),
		lastMouseVisible_(false),
		mouseGrabbed_(false),
		mouseMode_(MM_ABSOLUTE),
		inputFocus_(false),
		minimized_(false),
		focusedThisFrame_(false),
		suppressNextMouseMove_(false),
		inResize_(false),
		screenModeChanged_(false),
		initialized_(false)
	{
		gYume->pTimer->AddTimeEventListener(this);

		Initialize();
	}

	YumeInput::~YumeInput()
	{
		gYume->pTimer->RemoveTimeEventListener(this);
	}


	void YumeInput::Update()
	{
		keyPress_.clear();
		scancodePress_.clear();
		mouseButtonPress_ = 0;
		mouseMove_ = IntVector2::ZERO;
		mouseMoveWheel_ = 0;

		SDL_Event evt;
		SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);

		while(SDL_PollEvent(&evt))
			HandleSDLEvent(&evt);

		SDL_Window* window = graphics_->GetWindow();
		unsigned flags = window ? SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS) : 0;

		if(window)
		{
			if(!inputFocus_ && ((mouseVisible_ || mouseMode_ == MM_FREE) || graphics_->GetFullscreen() || screenModeChanged_) && (flags & SDL_WINDOW_INPUT_FOCUS))
			{
				screenModeChanged_ = false;
				focusedThisFrame_ = true;
			}

			if(focusedThisFrame_)
				GainFocus();

			// Check for losing focus. The window flags are not reliable when using an external window, so prevent losing focus in that case
			if(inputFocus_ && (flags & SDL_WINDOW_INPUT_FOCUS) == 0)
				LoseFocus();
		}

		if(!inputFocus_ && (flags & SDL_WINDOW_INPUT_FOCUS))
		{
			screenModeChanged_ = false;
			focusedThisFrame_ = true;
		}

		if(mouseVisible_ && mouseMode_ == MM_WRAP)
		{
			IntVector2 mpos;
			SDL_GetMouseState(&mpos.x_,&mpos.y_);

			const int buffer = 5;
			int width = graphics_->GetWidth() - buffer * 2;
			int height = graphics_->GetHeight() - buffer * 2;

			bool warp = false;
			if(mpos.x_ < buffer)
			{
				warp = true;
				mpos.x_ += width;
			}

			if(mpos.x_ > buffer + width)
			{
				warp = true;
				mpos.x_ -= width;
			}

			if(mpos.y_ < buffer)
			{
				warp = true;
				mpos.y_ += height;
			}

			if(mpos.y_ > buffer + height)
			{
				warp = true;
				mpos.y_ -= height;
			}

			if(warp)
			{
				SetMousePosition(mpos);
				SDL_FlushEvent(SDL_MOUSEMOTION);
			}
		}
		if((!mouseVisible_ && mouseMode_ != MM_FREE) && inputFocus_ && (flags & SDL_WINDOW_MOUSE_FOCUS))
		{
			IntVector2 mousePosition = GetMousePosition();
			mouseMove_ = mousePosition - lastMousePosition_;

			// Recenter the mouse cursor manually after move
			IntVector2 center(graphics_->GetWidth() / 2,graphics_->GetHeight() / 2);
			if(mousePosition != center)
			{
				SetMousePosition(center);
				lastMousePosition_ = center;
			}

			if(mouseMove_ != IntVector2::ZERO)
			{
				if(suppressNextMouseMove_)
				{
					mouseMove_ = IntVector2::ZERO;
					suppressNextMouseMove_ = false;
				}
				else
				{
					if(mouseVisible_)
					{
						FireMouseMove(0,mousePosition.x_,mousePosition.y_,mouseButtonDown_);
					}
					else
						FireMouseMove(0,mouseMove_.x_,mouseMove_.y_,mouseButtonDown_);
				}
			}
		}
	}
	void YumeInput::SetMouseVisible(bool enable,bool suppressEvent)
	{
		if(mouseMode_ == MM_RELATIVE)
			enable = false;
		// SDL Raspberry Pi "video driver" does not have proper OS mouse support yet, so no-op for now
		if(enable != mouseVisible_)
		{
			mouseVisible_ = enable;

			if(initialized_)
			{
				if(!mouseVisible_ && inputFocus_)
				{
					SDL_ShowCursor(SDL_FALSE);
					if(mouseMode_ != MM_FREE)
					{
						lastVisibleMousePosition_ = GetMousePosition();
						IntVector2 center(graphics_->GetWidth() / 2,graphics_->GetHeight() / 2);
						SetMousePosition(center);
						lastMousePosition_ = center;
					}
				}
				else
				{
					SDL_ShowCursor(SDL_TRUE);

					if(lastVisibleMousePosition_.x_ != MOUSE_POSITION_OFFSCREEN.x_ &&
						lastVisibleMousePosition_.y_ != MOUSE_POSITION_OFFSCREEN.y_)
						SetMousePosition(lastVisibleMousePosition_);
					lastMousePosition_ = lastVisibleMousePosition_;
				}
			}

			if(!suppressEvent)
			{

			}
		}

		// Make sure last mouse visible is valid:
		if(!suppressEvent)
			lastMouseVisible_ = mouseVisible_;
	}
	void YumeInput::ResetMouseVisible()
	{
		SetMouseVisible(lastMouseVisible_,true);
	}

	void YumeInput::SetMouseGrabbed(bool grab)
	{
		mouseGrabbed_ = grab;
	}


	void YumeInput::SetMouseMode(MouseMode mode)
	{
		if(mode != mouseMode_)
		{
			MouseMode previousMode = mouseMode_;
			mouseMode_ = mode;
			suppressNextMouseMove_ = true;
			SDL_Window* window = graphics_->GetWindow();
			// Handle changing away from previous mode
			if(previousMode == MM_RELATIVE)
			{
				ResetMouseVisible();

				SDL_SetWindowGrab(window,SDL_FALSE);
			}
			else if(previousMode == MM_WRAP)
				SDL_SetWindowGrab(window,SDL_FALSE);

			// Handle changing to new mode
			if(mode == MM_ABSOLUTE || mode == MM_FREE)
			{
				//
			}
			else
			{
				SetMouseGrabbed(true);

				if(mode == MM_RELATIVE)
				{
					SDL_SetWindowGrab(window,SDL_TRUE);
					SetMouseVisible(false,true);

				}
				else if(mode == MM_WRAP)
				{
					SDL_SetWindowGrab(window,SDL_TRUE);
				}
			}
		}
	}
	void YumeInput::ToggleFullscreen(bool enable)
	{
		toggleFullscreen_ = enable;
	}

	int YumeInput::GetKeyFromName(const YumeString& name) const
	{
		return SDL_GetKeyFromName(name.c_str());
	}


	int YumeInput::GetKeyFromScancode(int scancode) const
	{
		return SDL_GetKeyFromScancode((SDL_Scancode)scancode);
	}

	int YumeInput::GetScancodeFromKey(int key) const
	{
		return SDL_GetScancodeFromKey(key);
	}

	int YumeInput::GetScancodeFromName(const String& name) const
	{
		return SDL_GetScancodeFromName(name.c_str());
	}

	YumeString YumeInput::GetScancodeName(int scancode) const
	{
		return SDL_GetScancodeName((SDL_Scancode)scancode);
	}

	bool YumeInput::GetScancodeDown(int scancode) const
	{
		return scancodeDown_.Contains(scancode);
	}

	bool YumeInput::GetScancodePress(int scancode) const
	{
		return scancodePress_.Contains(scancode);
	}

	YumeString YumeInput::GetKeyName(int key) const
	{
		return YumeString(SDL_GetKeyName(key));
	}


	bool YumeInput::GetKeyDown(int key) const
	{
		return keyDown_.Contains(SDL_toupper(key));
	}

	bool YumeInput::GetKeyPress(int key) const
	{
		return keyPress_.Contains(SDL_toupper(key));
	}


	bool YumeInput::GetMouseButtonDown(int button) const
	{
		return (mouseButtonDown_ & button) != 0;
	}

	bool YumeInput::GetMouseButtonPress(int button) const
	{
		return (mouseButtonPress_ & button) != 0;
	}


	IntVector2 YumeInput::GetMousePosition() const
	{
		IntVector2 ret = IntVector2::ZERO;

		if(!initialized_)
			return ret;

		SDL_GetMouseState(&ret.x_,&ret.y_);

		return ret;
	}


	bool YumeInput::IsMinimized() const
	{
		// Return minimized state also when unfocused in fullscreen
		if(!inputFocus_ && graphics_ && graphics_->GetFullscreen())
			return true;
		else
			return minimized_;
	}


	void YumeInput::Initialize()
	{
		YumeRHI* graphics = gYume->pRHI;
		if(!graphics || !graphics->IsInitialized())
			return;

		graphics_ = graphics;

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);

		SDL_EventState(SDL_SYSWMEVENT,SDL_QUERY);

		// Set the initial activation
		initialized_ = true;
		focusedThisFrame_ = true;
		ResetState();

		YUMELOG_DEBUG("Initialized input");
	}


	void YumeInput::GainFocus()
	{
		ResetState();

		inputFocus_ = true;
		focusedThisFrame_ = false;

		// Restore mouse mode
		MouseMode mm = mouseMode_;
		mouseMode_ = MM_ABSOLUTE;
		SetMouseMode(mm);

		// Re-establish mouse cursor hiding as necessary
		if(!mouseVisible_)
		{
			SDL_ShowCursor(SDL_FALSE);
			suppressNextMouseMove_ = true;
		}
		else
			lastMousePosition_ = GetMousePosition();

		SendInputFocusEvent();
	}

	void YumeInput::LoseFocus()
	{
		ResetState();

		inputFocus_ = false;
		focusedThisFrame_ = false;

		MouseMode mm = mouseMode_;

		// Show the mouse cursor when inactive
		SDL_ShowCursor(SDL_TRUE);

		// Change mouse mode -- removing any cursor grabs, etc.
		SetMouseMode(MM_ABSOLUTE);

		// Restore flags to reflect correct mouse state.
		mouseMode_ = mm;

		SendInputFocusEvent();
	}

	void YumeInput::ResetState()
	{
		keyDown_.clear();
		keyPress_.clear();
		scancodeDown_.clear();
		scancodePress_.clear();

		// Use SetMouseButton() to reset the state so that mouse events will be sent properly
		SetMouseButton(0,MOUSEB_LEFT,false);
		SetMouseButton(0,MOUSEB_RIGHT,false);
		SetMouseButton(0,MOUSEB_MIDDLE,false);

		mouseMove_ = IntVector2::ZERO;
		mouseMoveWheel_ = 0;
		mouseButtonPress_ = 0;
	}
	void YumeInput::SendInputFocusEvent()
	{
		//Todo
	}


	void YumeInput::SetMouseButton(int native,int button,bool newState)
	{
		if(newState)
		{
			if(!(mouseButtonDown_ & button))
				mouseButtonPress_ |= button;

			mouseButtonDown_ |= button;
		}
		else
		{
			if(!(mouseButtonDown_ & button))
				return;

			mouseButtonDown_ &= ~button;
		}
		//ToDo mbutton down event
		FireMouseButtonDown(native,newState,button,mouseButtonDown_);
	}

	void YumeInput::SetKey(int key,int scancode,unsigned raw,bool newState)
	{
		bool repeat = false;

		if(newState)
		{
			scancodeDown_.insert(scancode);
			scancodePress_.insert(scancode);

			if(!keyDown_.Contains((key)))
			{
				keyDown_.insert(key);
				keyPress_.insert(key);
			}
			else
				repeat = true;
		}
		else
		{
			scancodeDown_.erase(scancode);

			if(!keyDown_.erase(key))
				return;
		}
		FireKeyDown(newState,(key),mouseButtonDown_,repeat);

		if((key == KEY_RETURN || key == KEY_RETURN2 || key == KEY_KP_ENTER) && newState && !repeat && toggleFullscreen_ &&
			(GetKeyDown(KEY_LALT) || GetKeyDown(KEY_RALT)))
			graphics_->ToggleFullscreen();
	}

	void YumeInput::SetMouseWheel(int delta)
	{
		if(delta)
		{
			mouseMoveWheel_ += delta;

			//ToDo mwheel event
		}
	}

	void YumeInput::SetMousePosition(const IntVector2& position)
	{
		if(!graphics_)
			return;

		SDL_WarpMouseInWindow(graphics_->GetWindow(),position.x_,position.y_);
	}

	void YumeInput::AddListener(InputEventListener* listener)
	{
		if(listeners_.Contains(listener))
			return;
		listeners_.push_back(listener);
	}

	void YumeInput::RemoveListener(InputEventListener* listener)
	{
		InputEventListeners::Iterator i = listeners_.find(listener);
		if(i != listeners_.end())
			listeners_.erase(i);
	}


	void YumeInput::FireMouseButtonDown(int modifiers,bool state,int button,unsigned buttons)
	{
		if(state)
			for(InputEventListeners::Iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleMouseButtonDown(GetMouseModifiers(),button,buttons);
		else
			for(InputEventListeners::Iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleMouseButtonUp(GetMouseModifiers(),button,buttons);
	}


	void YumeInput::FireKeyDown(bool state,int key,unsigned buttons,int repeat)
	{
		if(state)
			for(InputEventListeners::Iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleKeyDown(key,buttons,repeat);
		else
			for(InputEventListeners::Iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleKeyUp(key,buttons,repeat);
	}

	void YumeInput::FireMouseMove(int modifiers,int mouseX,int mouseY,unsigned buttons)
	{
		for(InputEventListeners::Iterator i = listeners_.begin(); i != listeners_.end(); ++i)
			(*i)->HandleMouseMove(mouseX,mouseY,GetMouseModifiers(),buttons);
	}

	int YumeInput::GetModifiers()
	{
		int modifiers = 0;
		if(GetKeyDown(KEY_SHIFT))
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		if(GetKeyDown(KEY_CTRL))
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		if(GetKeyDown(KEY_ALT))
			modifiers |= EVENTFLAG_ALT_DOWN;

		int mod = SDL_GetModState();
		mod = mod & KMOD_CAPS;

		if(mod == KMOD_CAPS)
			modifiers |= EVENTFLAG_CAPS_LOCK_ON;

		mod = SDL_GetModState();
		mod = mod & KMOD_NUM;

		if(GetKeyDown(KEY_LCTRL))
			modifiers |= EVENTFLAG_IS_LEFT;

		if(GetKeyDown(KEY_RCTRL))
			modifiers |= EVENTFLAG_IS_RIGHT;

		if(GetKeyDown(KEY_LSHIFT))
			modifiers |= EVENTFLAG_IS_LEFT;
		if(GetKeyDown(KEY_RSHIFT))
			modifiers |= EVENTFLAG_IS_RIGHT;;

		if(mod == KMOD_NUM)
			modifiers |= EVENTFLAG_NUM_LOCK_ON;

		return modifiers;
	}

	int YumeInput::GetMouseModifiers()
	{
		int modifiers = 0;

		if(GetKeyDown(KEY_SHIFT))
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		if(GetKeyDown(KEY_CTRL))
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		if(GetKeyDown(KEY_ALT))
			modifiers |= EVENTFLAG_ALT_DOWN;
		if(GetMouseButtonDown(MOUSEB_LEFT))
			modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
		if(GetMouseButtonDown(MOUSEB_MIDDLE))
			modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
		if(GetMouseButtonDown(MOUSEB_RIGHT))
			modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

		int mod = SDL_GetModState();
		mod = mod & KMOD_NUM;

		// Low bit set from GetKeyState indicates "toggled".
		if(mod == KMOD_NUM)
			modifiers |= EVENTFLAG_NUM_LOCK_ON;

		mod = SDL_GetModState();
		mod = mod & KMOD_CAPS;

		if(mod == KMOD_CAPS)
			modifiers |= EVENTFLAG_CAPS_LOCK_ON;
		return modifiers;
	}

	void YumeInput::HandleSDLEvent(void* sdlEvent)
	{
		SDL_Event& evt = *static_cast<SDL_Event*>(sdlEvent);


		if(!inputFocus_ && evt.type >= SDL_KEYDOWN && evt.type <= SDL_MULTIGESTURE)
		{
			// Require the click to be at least 1 pixel inside the window to disregard clicks in the title bar
			if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.x > 0 && evt.button.y > 0 && evt.button.x < graphics_->GetWidth() - 1 &&
				evt.button.y < graphics_->GetHeight() - 1)
			{
				focusedThisFrame_ = true;
				// Do not cause the click to actually go throughfin
				return;
			}
			else if(evt.type == SDL_FINGERDOWN)
			{
				// When focusing by touch, call GainFocus() immediately as it resets the state; a touch has sustained state
				// which should be kept
				GainFocus();
			}
			else
				return;
		}



		switch(evt.type)
		{
		case SDL_SYSWMEVENT:
		{
			UINT message = evt.syswm.msg->msg.win.msg;
			break;
		}
		break;
		case SDL_KEYDOWN:
			SetKey(ConvertSDLKeyCode(evt.key.keysym.sym,evt.key.keysym.scancode),evt.key.keysym.scancode,0,true);
			break;

		case SDL_KEYUP:
			SetKey(ConvertSDLKeyCode(evt.key.keysym.sym,evt.key.keysym.scancode),evt.key.keysym.scancode,0,false);
			break;

		case SDL_MOUSEBUTTONDOWN:
			SetMouseButton(0,1 << (evt.button.button - 1),true);
			break;
		case SDL_MOUSEBUTTONUP:
		{
			SetMouseButton(0,1 << (evt.button.button - 1),false);
		}
		break;

		case SDL_MOUSEMOTION:
			if(mouseVisible_ || mouseMode_ == MM_FREE)
			{
				mouseMove_.x_ += evt.motion.xrel;
				mouseMove_.y_ += evt.motion.yrel;

				if(mouseVisible_)
				{
					FireMouseMove(0,evt.motion.x,evt.motion.y,mouseButtonDown_);
				}
				else
					FireMouseMove(0,evt.motion.xrel,evt.motion.yrel,mouseButtonDown_);
			}
			break;

		case SDL_MOUSEWHEEL:
			SetMouseWheel(evt.wheel.y);
		case SDL_WINDOWEVENT:
		{
			switch(evt.window.event)
			{
			case SDL_WINDOWEVENT_MINIMIZED:
				minimized_ = true;
				SendInputFocusEvent();
				break;

			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				minimized_ = false;
				SendInputFocusEvent();
				break;

			case SDL_WINDOWEVENT_RESIZED:
				inResize_ = true;
				//graphics_->WindowResized();
				inResize_ = false;
				break;
			case SDL_WINDOWEVENT_MOVED:
				//graphics_->WindowMoved();
				break;

			default: break;
			}
		}
		break;

		case SDL_QUIT:
		{
			gYume->pEngine->Exit();
		}
		break;

		default: break;
		}
	}

	void YumeInput::HandleScreenMode(YumeHash eventType,VariantMap& eventData)
	{
		if(!initialized_)
			Initialize();

		// Re-enable cursor clipping, and re-center the cursor (if needed) to the new screen size, so that there is no erroneous
		// mouse move event. Also get new window ID if it changed
		SDL_Window* window = graphics_->GetWindow();
		windowID_ = SDL_GetWindowID(window);

		// If screen mode happens due to mouse drag resize, do not recenter the mouse as that would lead to erratic window sizes
		if(!mouseVisible_ && mouseMode_ != MM_FREE && !inResize_)
		{
			IntVector2 center(graphics_->GetWidth() / 2,graphics_->GetHeight() / 2);
			SetMousePosition(center);
			lastMousePosition_ = center;
			focusedThisFrame_ = true;
		}
		else
			lastMousePosition_ = GetMousePosition();

		if(graphics_->GetFullscreen())
			focusedThisFrame_ = true;

		// After setting a new screen mode we should not be minimized
		minimized_ = (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0;

		// Remember that screen mode changed in case we lose focus (needed on Linux)
		if(!inResize_)
			screenModeChanged_ = true;
	}

	void YumeInput::HandleBeginFrame(int frameNumber)
	{
		// Update input right at the beginning of the frame
		Update();
	}
}
