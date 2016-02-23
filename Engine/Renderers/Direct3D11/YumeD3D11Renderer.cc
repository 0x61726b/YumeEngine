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
#include "YumeRequired.h"
#include "YumeD3D11Renderer.h"

#include "YumeD3D11GpuResource.h"
#include "YumeD3D11RendererImpl.h"

#include "Math/YumeMath.h"

#include "Logging/logging.h"

#include "Engine/YumeEngine.h"

#include <SDL_syswm.h>

namespace YumeEngine
{
	extern "C" void YumeD3DExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeRenderer* graphics_ = new YumeD3D11Renderer;
		engine->SetRenderer(graphics_);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeD3DExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRenderer* graphics_ = engine->GetRenderer();
		delete graphics_;
	}
	//---------------------------------------------------------------------
	YumeD3D11Renderer::YumeD3D11Renderer()
		: initialized_(false),
		numPrimitives_(0),
		numBatches_(0),
		windowWidth_(0),
		windowHeight_(0),
		windowTitle_("Yume Engine"),
		windowPos_(SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED),
		impl_(YumeAPINew YumeD3D11RendererImpl),
		flushGpu_(false),
		multiSample_(1),
		fullscreen_(false),
		borderless_(false),
		resizeable_(false),
		vsync_(false),
		tripleBuffer_(false)
	{


		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	}

	YumeD3D11Renderer::~YumeD3D11Renderer()
	{
		{
			boost::mutex::scoped_lock lock(gpuResourceMutex_);

			for(int i=0; i < gpuResources_.size(); ++i)
				gpuResources_[i]->Release();

			gpuResources_.clear();
		}
		impl_->blendStates_.clear();

		BlendStatesMap::iterator blendIt;

		for(blendIt = impl_->blendStates_.begin();blendIt != impl_->blendStates_.end(); ++blendIt)
			D3D_SAFE_RELEASE(blendIt->second);

		impl_->depthStates_.clear();

		DepthStatesMap::iterator depthIt;

		for(depthIt = impl_->depthStates_.begin();depthIt != impl_->depthStates_.end(); ++depthIt)
			D3D_SAFE_RELEASE(depthIt->second);


		impl_->rasterizerStates_.clear();
		RasterizerStatesMap::iterator rasterizerIt;

		for(rasterizerIt = impl_->rasterizerStates_.begin();rasterizerIt != impl_->rasterizerStates_.end(); ++rasterizerIt)
			D3D_SAFE_RELEASE(rasterizerIt->second);

		D3D_SAFE_RELEASE(impl_->defaultRenderTargetView_);
		D3D_SAFE_RELEASE(impl_->defaultDepthStencilView_);
		D3D_SAFE_RELEASE(impl_->defaultDepthTexture_);
		D3D_SAFE_RELEASE(impl_->resolveTexture_);
		D3D_SAFE_RELEASE(impl_->swapChain_);
		D3D_SAFE_RELEASE(impl_->deviceContext_);
		D3D_SAFE_RELEASE(impl_->device_);

		if(impl_->debug_)
		{
			impl_->debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			D3D_SAFE_RELEASE(impl_->debug_);
		}

		if(impl_->window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(impl_->window_);
			impl_->window_ = 0;
		}

		delete impl_;
		impl_ = 0;

		// Shut down SDL now. Graphics should be the last SDL-using subsystem to be destroyed
		SDL_Quit();

	}

	bool YumeD3D11Renderer::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		if(fullscreen_ && (SDL_GetWindowFlags(impl_->window_) & SDL_WINDOW_MINIMIZED))
			return false;

		numPrimitives_	= 0;
		numBatches_		= 0;

		return true;
	}

	void YumeD3D11Renderer::EndFrame()
	{
		if(!IsInitialized())
			return;

		impl_->swapChain_->Present(vsync_ ? 1 : 0,0);
	}

	void YumeD3D11Renderer::Clear(unsigned flags,const Vector4& color,float depth,unsigned stencil)
	{
		//Dummy clear code
		/*if((flags & CLEAR_COLOR) && impl_->renderTargetViews_[0])
			impl_->deviceContext_->ClearRenderTargetView(impl_->renderTargetViews_[0],color.ptr());

			if((flags & (CLEAR_DEPTH | CLEAR_STENCIL)) && impl_->depthStencilView_)
			{
			unsigned depthClearFlags = 0;
			if(flags & CLEAR_DEPTH)
			depthClearFlags |= D3D11_CLEAR_DEPTH;
			if(flags & CLEAR_STENCIL)
			depthClearFlags |= D3D11_CLEAR_STENCIL;
			impl_->deviceContext_->ClearDepthStencilView(impl_->depthStencilView_,depthClearFlags,depth,(UINT8)stencil);
			}*/

		impl_->deviceContext_->ClearRenderTargetView(impl_->defaultRenderTargetView_,color.ptr());

		unsigned depthClearFlags = 0;
		if(flags & CLEAR_DEPTH)
			depthClearFlags |= D3D11_CLEAR_DEPTH;
		if(flags & CLEAR_STENCIL)
			depthClearFlags |= D3D11_CLEAR_STENCIL;
		impl_->deviceContext_->ClearDepthStencilView(impl_->defaultDepthStencilView_,depthClearFlags,depth,(UINT8)stencil);
	}

	void YumeD3D11Renderer::CreateRendererCapabilities()
	{
		lightPrepassSupport_ = true;
		deferredSupport_ = true;
		hardwareShadowSupport_ = true;
		instancingSupport_ = true;
		shadowMapFormat_ = DXGI_FORMAT_R16_TYPELESS;
		hiresShadowMapFormat_ = DXGI_FORMAT_R32_TYPELESS;
		dummyColorFormat_ = DXGI_FORMAT_UNKNOWN;
		sRGBSupport_ = true;
		sRGBWriteSupport_ = true;
	}

	static HWND GetWindowHandle(SDL_Window* window)
	{
		SDL_SysWMinfo sysInfo;

		SDL_VERSION(&sysInfo.version);
		SDL_GetWindowWMInfo(window,&sysInfo);
		return sysInfo.info.win.window;
	}

	void YumeD3D11Renderer::ResetRenderTargets()
	{
		SetViewport(Vector4(0,0,windowWidth_,windowHeight_));
	}

	void YumeD3D11Renderer::SetViewport(const Vector4& rect)
	{
		Vector2 size = GetRenderTargetDimensions();



		static D3D11_VIEWPORT d3dViewport;
		d3dViewport.TopLeftX = rect.x;
		d3dViewport.TopLeftY = rect.y;
		d3dViewport.Width = size.x;
		d3dViewport.Height = size.y;
		d3dViewport.MinDepth = 0.0f;
		d3dViewport.MaxDepth = 1.0f;

		impl_->deviceContext_->RSSetViewports(1,&d3dViewport);


		viewport_.x = rect.x;
		viewport_.y = rect.y;
		viewport_.z = size.x;
		viewport_.w = size.y;



		// Disable scissor test, needs to be re-enabled by the user
		/*SetScissorTest(false);*/
	}

	Vector2 YumeD3D11Renderer::GetRenderTargetDimensions() const
	{
		int width,height;

		//if(renderTargets_[0])
		//{
		//	width = renderTargets_[0]->GetWidth();
		//	height = renderTargets_[0]->GetHeight();
		//}
		//else if(depthStencil_) // Depth-only rendering
		//{
		//	width = depthStencil_->GetWidth();
		//	height = depthStencil_->GetHeight();
		//}
		//else
		//{
		//	width = windowWidth_;
		//	height = windowHeight_;
		//}

		width = windowWidth_;
		height = windowHeight_;
		return Vector2(width,height);
	}


	YumeVector<int>::type YumeD3D11Renderer::GetMultiSampleLevels() const
	{
		YumeVector<int>::type levelsVector;
		levelsVector.push_back(1);

		if(impl_->device_)
		{
			for(unsigned i = 2; i <= 16; ++i)
			{
				unsigned levels = 0;
				impl_->device_->CheckMultisampleQualityLevels(sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM,
					i,&levels);
				levelsVector.push_back(i);
			}
		}

		return levelsVector;
	}


	void YumeD3D11Renderer::SetFlushGPU(bool flushGpu)
	{
		flushGpu_ = flushGpu;

		if(impl_->device_)
		{
			IDXGIDevice1* dxgiDevice;
			impl_->device_->QueryInterface(IID_IDXGIDevice1,(void**)&dxgiDevice);
			if(dxgiDevice)
			{
				dxgiDevice->SetMaximumFrameLatency(flushGpu_ ? 1 : 3);
				dxgiDevice->Release();
			}
		}
	}

	void YumeD3D11Renderer::SetWindowPos(const Vector2& pos)
	{
		if(impl_->window_)
			SDL_SetWindowPosition(impl_->window_,pos.x,pos.y);
		else
			windowPos_ = pos; // Sets as initial position for OpenWindow()
	}
	void YumeD3D11Renderer::SetWindowTitle(const YumeString& title)
	{
		windowTitle_ = title;
		if(impl_->window_)
			SDL_SetWindowTitle(impl_->window_,title.c_str());
	}
	bool YumeD3D11Renderer::OpenWindow(int width,int height,bool resizable,bool borderless)
	{
		unsigned flags = 0;
		if(resizable)
			flags |= SDL_WINDOW_RESIZABLE;
		if(borderless)
			flags |= SDL_WINDOW_BORDERLESS;

		impl_->window_ = SDL_CreateWindow(windowTitle_.c_str(),windowPos_.x,windowPos_.y,width,height,flags);

		if(!impl_->window_)
		{
			YUMELOG_ERROR("Rendering window couldn't be created! Error:" << std::endl << SDL_GetError());
			return false;
		}
		int *x = new int;
		int *y = new int;
		SDL_GetWindowPosition(impl_->window_,x,y);

		windowPos_.x = (float)*x;
		windowPos_.y = (float)*y;

		return true;
	}


	void YumeD3D11Renderer::AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless)
	{
		if(!newWidth || !newHeight)
		{
			SDL_MaximizeWindow(impl_->window_);
			SDL_GetWindowSize(impl_->window_,&newWidth,&newHeight);
		}
		else
			SDL_SetWindowSize(impl_->window_,newWidth,newHeight);

		SDL_SetWindowFullscreen(impl_->window_,newFullscreen ? SDL_TRUE : SDL_FALSE);
		SDL_SetWindowBordered(impl_->window_,newBorderless ? SDL_FALSE : SDL_TRUE);
	}

	void YumeD3D11Renderer::Close()
	{
		if(impl_->window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(impl_->window_);
			impl_->window_ = 0;
		}
	}

	bool YumeD3D11Renderer::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
		int multiSample)
	{
		bool maximize = false;

		// Find out the full screen mode display format (match desktop color depth)
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(0,&mode);
		DXGI_FORMAT fullscreenFormat = SDL_BITSPERPIXEL(mode.format) == 16 ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;

		// If zero dimensions in windowed mode, set windowed mode to maximize and set a predefined default restored window size. If zero in fullscreen, use desktop mode
		if(!width || !height)
		{
			if(fullscreen || borderless)
			{
				width = mode.w;
				height = mode.h;
			}
			else
			{
				maximize = resizable;
				width = 1024;
				height = 768;
			}
		}

		// Fullscreen or Borderless can not be resizable
		if(fullscreen || borderless)
			resizable = false;

		// Borderless cannot be fullscreen, they are mutually exclusive
		if(borderless)
			fullscreen = false;

		// If nothing changes, do not reset the device
		if(width == windowWidth_ && height == windowHeight_ && fullscreen == fullscreen_ && borderless == borderless_ && resizable == resizeable_ &&
			vsync == vsync_ && tripleBuffer == tripleBuffer_ && multiSample == multiSample_)
			return true;

		SDL_SetHint(SDL_HINT_ORIENTATIONS,"LandscapeLeft");

		if(!impl_->window_)
		{
			if(!OpenWindow(width,height,resizable,borderless))
				return false;
		}

		if(fullscreen)
		{
			YumeVector<Vector2>::type resolutions = GetScreenResolutions();
			if(resolutions.size() > 0)
			{
				unsigned best = 0;
				unsigned bestError = 0xffffffff;

				for(unsigned i = 0; i < resolutions.size(); ++i)
				{
					unsigned error = (unsigned)(Math::Abs(resolutions[i].x - width) + Math::Abs(resolutions[i].y - height));
					if(error < bestError)
					{
						best = i;
						bestError = error;
					}
				}

				width = resolutions[best].x;
				height = resolutions[best].y;
			}
		}

		AdjustWindow(width,height,fullscreen,borderless);

		if(maximize)
		{
			Maximize();
			SDL_GetWindowSize(impl_->window_,&width,&height);
		}

		if(!impl_->device_ || multiSample_ != multiSample)
			CreateD3D11Device(width,height,multiSample);
		UpdateSwapchain(width,height);

		fullscreen_ = fullscreen;
		borderless_ = borderless;
		resizeable_ = resizable;
		vsync_ = vsync;
		tripleBuffer_ = tripleBuffer;

		Clear(CLEAR_COLOR);
		impl_->swapChain_->Present(0,0);

		YUMELOG_INFO("Graphics Mode: ");
		YUMELOG_INFO("Width: " << width);
		YUMELOG_INFO("Height: " << height);
		YUMELOG_INFO("Fullscreen: " << fullscreen);
		YUMELOG_INFO("Borderless: " << borderless);
		YUMELOG_INFO("Vsync: " << vsync);
		YUMELOG_INFO("Multisample: " << multiSample);
		YUMELOG_INFO("Triple Buffer: " << tripleBuffer);


		initialized_ = true;
		return true;
	}

	bool YumeD3D11Renderer::CreateD3D11Device(int width,int height,int multisample)
	{
		// Device needs only to be created once
		if(!impl_->device_)
		{
			HRESULT hr = D3D11CreateDevice(
				0,
				D3D_DRIVER_TYPE_HARDWARE,
				0,
				D3D11_CREATE_DEVICE_DEBUG,
				0,
				0,
				D3D11_SDK_VERSION,
				&impl_->device_,
				0,
				&impl_->deviceContext_
				);

			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(impl_->device_);
				D3D_SAFE_RELEASE(impl_->deviceContext_);
				YUMELOG_ERROR("Failed to create D3D11 device" << hr);
				return false;
			}



			CreateRendererCapabilities();

			SetFlushGPU(flushGpu_);
		}

		YumeVector<int>::type multisamples = GetMultiSampleLevels();

		if(std::find(multisamples.begin(),multisamples.end(),multisample) != multisamples.end())
		{
			multisample = 1;
		}

		if(impl_->swapChain_)
		{
			impl_->swapChain_->Release();
			impl_->swapChain_ = 0;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		memset(&swapChainDesc,0,sizeof swapChainDesc);
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = (UINT)width;
		swapChainDesc.BufferDesc.Height = (UINT)height;
		swapChainDesc.BufferDesc.Format = sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = GetWindowHandle(impl_->window_);
		swapChainDesc.SampleDesc.Count = (UINT)multisample;
		swapChainDesc.SampleDesc.Quality = multisample > 1 ? 0xffffffff : 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGIDevice* dxgiDevice = 0;
		impl_->device_->QueryInterface(IID_IDXGIDevice,(void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter = 0;
		dxgiDevice->GetParent(IID_IDXGIAdapter,(void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory = 0;
		dxgiAdapter->GetParent(IID_IDXGIFactory,(void**)&dxgiFactory);
		HRESULT hr = dxgiFactory->CreateSwapChain(impl_->device_,&swapChainDesc,&impl_->swapChain_);
		// After creating the swap chain, disable automatic Alt-Enter fullscreen/windowed switching
		// (the application will switch manually if it wants to)
		dxgiFactory->MakeWindowAssociation(GetWindowHandle(impl_->window_),DXGI_MWA_NO_ALT_ENTER);

		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();

		impl_->GetDevice()->QueryInterface(__uuidof(ID3D11Debug),reinterpret_cast<void**>(&impl_->debug_));

		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(impl_->swapChain_);
			YUMELOG_ERROR("Failed to create D3D11 swap chain" << hr);
			return false;
		}
		else if(impl_->swapChain_)
		{
			multiSample_ = multisample;
			return true;
		}

		return false;
	}

	bool YumeD3D11Renderer::UpdateSwapchain(int width,int height)
	{
		bool success = true;

		ID3D11RenderTargetView* nullView = 0;
		impl_->deviceContext_->OMSetRenderTargets(1,&nullView,0);
		if(impl_->defaultRenderTargetView_)
		{
			impl_->defaultRenderTargetView_->Release();
			impl_->defaultRenderTargetView_ = 0;
		}
		if(impl_->defaultDepthStencilView_)
		{
			impl_->defaultDepthStencilView_->Release();
			impl_->defaultDepthStencilView_ = 0;
		}
		if(impl_->defaultDepthTexture_)
		{
			impl_->defaultDepthTexture_->Release();
			impl_->defaultDepthTexture_ = 0;
		}
		if(impl_->resolveTexture_)
		{
			impl_->resolveTexture_->Release();
			impl_->resolveTexture_ = 0;
		}

		impl_->depthStencilView_ = 0;
		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			impl_->renderTargetViews_[i] = 0;
		renderTargetsDirty_ = true;

		impl_->swapChain_->ResizeBuffers(1,(UINT)width,(UINT)height,DXGI_FORMAT_UNKNOWN,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// Create default rendertarget view representing the backbuffer
		ID3D11Texture2D* backbufferTexture;
		HRESULT hr = impl_->swapChain_->GetBuffer(0,IID_ID3D11Texture2D,(void**)&backbufferTexture);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(backbufferTexture);
			YUMELOG_ERROR("Failed to get backbuffer texture.Error: " << hr);
			success = false;
		}
		else
		{
			hr = impl_->device_->CreateRenderTargetView(backbufferTexture,0,&impl_->defaultRenderTargetView_);
			backbufferTexture->Release();
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(impl_->defaultRenderTargetView_);
				YUMELOG_ERROR("Failed to create backbuffer rendertarget view.Error: " << hr);
				success = false;
			}
		}

		// Create default depth-stencil texture and view
		D3D11_TEXTURE2D_DESC depthDesc;
		memset(&depthDesc,0,sizeof depthDesc);
		depthDesc.Width = (UINT)width;
		depthDesc.Height = (UINT)height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = (UINT)multiSample_;
		depthDesc.SampleDesc.Quality = multiSample_ > 1 ? 0xffffffff : 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hr = impl_->device_->CreateTexture2D(&depthDesc,0,&impl_->defaultDepthTexture_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(impl_->defaultDepthTexture_);
			YUMELOG_ERROR("Failed to create backbuffer depth-stencil texture.Error: " << hr);
			success = false;
		}
		else
		{
			hr = impl_->device_->CreateDepthStencilView(impl_->defaultDepthTexture_,0,&impl_->defaultDepthStencilView_);
			if(FAILED(hr))
			{
				D3D_SAFE_RELEASE(impl_->defaultDepthStencilView_);
				YUMELOG_ERROR("Failed to create backbuffer depth-stencil view.Error: " << hr);
				success = false;
			}
		}

		// Update internally held backbuffer size
		windowWidth_ = width;
		windowHeight_ = height;

		ResetRenderTargets();
		return success;
	}
	void YumeD3D11Renderer::Maximize()
	{
		if(!impl_->window_)
			return;

		SDL_MaximizeWindow(impl_->window_);
	}

	YumeVector<Vector2>::type YumeD3D11Renderer::GetScreenResolutions()
	{
		YumeVector<Vector2>::type resolutions;


		unsigned numModes = (unsigned)SDL_GetNumDisplayModes(0);

		for(unsigned i = 0; i < numModes; ++i)
		{
			SDL_DisplayMode mode;
			SDL_GetDisplayMode(0,i,&mode);
			int width = mode.w;
			int height = mode.h;

			// Store mode if unique
			bool unique = true;
			for(unsigned j = 0; j < resolutions.size(); ++j)
			{
				if(resolutions[j].x == width && resolutions[j].y == height)
				{
					unique = false;
					break;
				}
			}

			if(unique)
				resolutions.push_back(Vector2(width,height));
		}
		return resolutions;
	}

	void YumeD3D11Renderer::AddGpuResource(YumeGpuResource* gpuRes)
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		gpuResources_.push_back(gpuRes);
	}

	void YumeD3D11Renderer::RemoveGpuResource(YumeGpuResource* gpuRes)
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		//Check if valid
		GpuResourceVector::iterator It = std::find(gpuResources_.begin(),gpuResources_.end(),gpuRes);

		if(It != gpuResources_.end())
			gpuResources_.erase(It);
	}
}
