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

#include "Renderer/YumeResourceManager.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeMatrix3.h"
#include "Math/YumeMatrix4.h"
#include "Math/YumeMath.h"
#include "Math/YumeRect.h"

#include "YumeD3D11Shader.h"
#include "YumeD3D11ShaderVariation.h"
#include "YumeD3D11VertexBuffer.h"
#include "YumeD3D11ConstantBuffer.h"
#include "YumeD3D11ShaderProgram.h"
#include "YumeD3D11Texture2D.h"
#include "YumeD3D11IndexBuffer.h"
#include "YumeD3D11InputLayout.h"

#include "Renderer/YumeTexture2D.h"

#include "Renderer/YumeRenderable.h"

#include "Renderer/YumeImage.h"

#include "Core/YumeBase.h"
#include "Core/YumeDefaults.h"

#include "Logging/logging.h"

#include "Engine/YumeEngine.h"

#include <SDL_syswm.h>

#include <boost/algorithm/string.hpp>

namespace YumeEngine
{

	static const D3D11_COMPARISON_FUNC d3dCmpFunc[] =
	{
		D3D11_COMPARISON_ALWAYS,
		D3D11_COMPARISON_EQUAL,
		D3D11_COMPARISON_NOT_EQUAL,
		D3D11_COMPARISON_LESS,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_GREATER,
		D3D11_COMPARISON_GREATER_EQUAL
	};

	static const DWORD d3dBlendEnable[] =
	{
		FALSE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE
	};

	static const D3D11_BLEND d3dSrcBlend[] =
	{
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_DEST_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_ALPHA,
	};

	static const D3D11_BLEND d3dDestBlend[] =
	{
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE
	};

	static const D3D11_BLEND_OP d3dBlendOp[] =
	{
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT
	};

	static const D3D11_STENCIL_OP d3dStencilOp[] =
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_ZERO,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_INCR,
		D3D11_STENCIL_OP_DECR
	};

	static const D3D11_CULL_MODE d3dCullMode[] =
	{
		D3D11_CULL_NONE,
		D3D11_CULL_BACK,
		D3D11_CULL_FRONT
	};

	static const D3D11_FILL_MODE d3dFillMode[] =
	{
		D3D11_FILL_SOLID,
		D3D11_FILL_WIREFRAME,
		D3D11_FILL_WIREFRAME // Point fill mode not supported
	};

	static unsigned GetD3DColor(const YumeColor& color)
	{
		unsigned r = (unsigned)(Math::Clamp(color.r_ * 255.0f,0.0f,255.0f));
		unsigned g = (unsigned)(Math::Clamp(color.g_ * 255.0f,0.0f,255.0f));
		unsigned b = (unsigned)(Math::Clamp(color.b_ * 255.0f,0.0f,255.0f));
		unsigned a = (unsigned)(Math::Clamp(color.a_ * 255.0f,0.0f,255.0f));
		return (((a)& 0xff) << 24) | (((r)& 0xff) << 16) | (((g)& 0xff) << 8) | ((b)& 0xff);
	}

	extern "C" void YumeD3DExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = new YumeD3D11Renderer;
		engine->SetRenderer(graphics_);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeD3DExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = engine->GetRenderer();
		delete graphics_;
	}
	//---------------------------------------------------------------------
	YumeD3D11Renderer::YumeD3D11Renderer()
		: initialized_(false),
		impl_(YumeAPINew YumeD3D11RendererImpl),
		shaderProgram_(0)
	{
		apiName_ = "D3D11";
		shaderPath_ = "Shaders/HLSL/";
		shaderExtension_ = ".hlsl";

		ResetCache();

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

		RegisterFactories();
	}

	YumeD3D11Renderer::~YumeD3D11Renderer()
	{

		{
			boost::mutex::scoped_lock lock(gpuResourceMutex_);

			for(int i=0; i < gpuResources_.size(); ++i)
			{
				if(gpuResources_[i])
					gpuResources_[i]->Release();
			}

			gpuResources_.clear();
		}
		impl_->blendStates_.clear();

		// ToDo(arkenthera) not sure about this..
		int size = constantBuffers_.size();
		for(int i=0; i < size; ++i)
			constantBuffers_[i].reset();
		constantBuffers_.clear();

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

		if(window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(window_);
			window_ = 0;
		}

		delete impl_;
		impl_ = 0;

		lastShader_.reset();

		UnregisterFactories();

		// Shut down SDL now. Graphics should be the last SDL-using subsystem to be destroyed
		SDL_Quit();

	}

	bool YumeD3D11Renderer::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		if(fullscreen_ && (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED))
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

	void YumeD3D11Renderer::ResetCache()
	{
		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			vertexBuffers_[i] = 0;
			elementMasks_[i] = 0;
			impl_->vertexBuffers_[i] = 0;
			impl_->vertexSizes_[i] = 0;
			impl_->vertexOffsets_[i] = 0;
		}

		//
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			//textures_[i] = 0;
			impl_->shaderResourceViews_[i] = 0;
			impl_->samplers_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			//renderTargets_[i] = 0;
			impl_->renderTargetViews_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
		{
			impl_->constantBuffers_[VS][i] = 0;
			impl_->constantBuffers_[PS][i] = 0;
		}

		depthStencil_ = 0;
		impl_->depthStencilView_ = 0;
		viewport_ = IntRect(0,0,windowWidth_,windowHeight_);
		textureAnisotropy_ = 1;

		indexBuffer_ = 0;
		vertexDeclarationHash_ = 0;
		primitiveType_ = 0;
		vertexShader_ = 0;
		pixelShader_ = 0;
		shaderProgram_ = 0;
		blendMode_ = BLEND_REPLACE;
		colorWrite_ = true;
		cullMode_ = CULL_CCW;
		constantDepthBias_ = 0.0f;
		slopeScaledDepthBias_ = 0.0f;
		depthTestMode_ = CMP_LESSEQUAL;
		depthWrite_ = true;
		fillMode_ = FILL_SOLID;
		scissorTest_ = false;
		scissorRect_ = IntRect::ZERO;
		stencilTest_ = false;
		stencilTestMode_ = CMP_ALWAYS;
		stencilPass_ = OP_KEEP;
		stencilFail_ = OP_KEEP;
		stencilZFail_ = OP_KEEP;
		stencilRef_ = 0;
		stencilCompareMask_ = Math::M_MAX_UNSIGNED;
		stencilWriteMask_ = Math::M_MAX_UNSIGNED;
		useClipPlane_ = false;
		renderTargetsDirty_ = true;
		texturesDirty_ = false;
		vertexDeclarationDirty_ = true;
		blendStateDirty_ = true;
		depthStateDirty_ = true;
		rasterizerStateDirty_ = true;
		scissorRectDirty_ = true;
		stencilRefDirty_ = true;
		blendStateHash_ = Math::M_MAX_UNSIGNED;
		depthStateHash_ = Math::M_MAX_UNSIGNED;
		rasterizerStateHash_ = Math::M_MAX_UNSIGNED;
		firstDirtyTexture_ = lastDirtyTexture_ = Math::M_MAX_UNSIGNED;
		firstDirtyVB_ = lastDirtyVB_ = Math::M_MAX_UNSIGNED;
		dirtyConstantBuffers_.clear();
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


	static void GetD3DPrimitiveType(unsigned elementCount,PrimitiveType type,unsigned& primitiveCount,
		D3D_PRIMITIVE_TOPOLOGY& d3dPrimitiveType)
	{
		switch(type)
		{
		case TRIANGLE_LIST:
			primitiveCount = elementCount / 3;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;

		case LINE_LIST:
			primitiveCount = elementCount / 2;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			break;

		case POINT_LIST:
			primitiveCount = elementCount;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;

		case TRIANGLE_STRIP:
			primitiveCount = elementCount - 2;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;

		case LINE_STRIP:
			primitiveCount = elementCount - 1;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;

		case TRIANGLE_FAN:
			// Triangle fan is not supported on D3D11
			primitiveCount = 0;
			d3dPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			break;
		}
	}

	static HWND GetWindowHandle(SDL_Window* window)
	{
		SDL_SysWMinfo sysInfo;

		SDL_VERSION(&sysInfo.version);
		SDL_GetWindowWMInfo(window,&sysInfo);
		return sysInfo.info.win.window;
	}

	void YumeD3D11Renderer::SetViewport(const IntRect& rect)
	{
		Vector2 size = GetRenderTargetDimensions();

		IntRect rectCopy = rect;

		if(rectCopy.right_ <= rectCopy.left_)
			rectCopy.right_ = rectCopy.left_ + 1;
		if(rectCopy.bottom_ <= rectCopy.top_)
			rectCopy.bottom_ = rectCopy.top_ + 1;
		rectCopy.left_ = Math::Clamp<int>(rectCopy.left_,0,size.x);
		rectCopy.top_ = Math::Clamp<int>(rectCopy.top_,0,size.y);
		rectCopy.right_ = Math::Clamp<int>(rectCopy.right_,0,size.x);
		rectCopy.bottom_ = Math::Clamp<int>(rectCopy.bottom_,0,size.y);

		static D3D11_VIEWPORT d3dViewport;
		d3dViewport.TopLeftX = (float)rectCopy.left_;
		d3dViewport.TopLeftY = (float)rectCopy.top_;
		d3dViewport.Width = (float)(rectCopy.right_ - rectCopy.left_);
		d3dViewport.Height = (float)(rectCopy.bottom_ - rectCopy.top_);
		d3dViewport.MinDepth = 0.0f;
		d3dViewport.MaxDepth = 1.0f;

		impl_->deviceContext_->RSSetViewports(1,&d3dViewport);

		viewport_ = rectCopy;

		// Disable scissor test, needs to be re-enabled by the user
		//SetScissorTest(false);
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
		if(window_)
			SDL_SetWindowPosition(window_,pos.x,pos.y);
		else
			windowPos_ = pos; // Sets as initial position for OpenWindow()
	}
	void YumeD3D11Renderer::SetWindowTitle(const YumeString& title)
	{
		windowTitle_ = title;
		if(window_)
			SDL_SetWindowTitle(window_,title.c_str());
	}
	bool YumeD3D11Renderer::OpenWindow(int width,int height,bool resizable,bool borderless)
	{
		unsigned flags = 0;
		if(resizable)
			flags |= SDL_WINDOW_RESIZABLE;
		if(borderless)
			flags |= SDL_WINDOW_BORDERLESS;

		window_ = SDL_CreateWindow(windowTitle_.c_str(),windowPos_.x,windowPos_.y,width,height,flags);

		if(!window_)
		{
			YUMELOG_ERROR("Rendering window couldn't be created! Error:" << std::endl << SDL_GetError());
			return false;
		}
		int *x = new int;
		int *y = new int;
		SDL_GetWindowPosition(window_,x,y);

		windowPos_.x = (float)*x;
		windowPos_.y = (float)*y;

		return true;
	}


	void YumeD3D11Renderer::AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless)
	{
		if(!newWidth || !newHeight)
		{
			SDL_MaximizeWindow(window_);
			SDL_GetWindowSize(window_,&newWidth,&newHeight);
		}
		else
			SDL_SetWindowSize(window_,newWidth,newHeight);

		SDL_SetWindowFullscreen(window_,newFullscreen ? SDL_TRUE : SDL_FALSE);
		SDL_SetWindowBordered(window_,newBorderless ? SDL_FALSE : SDL_TRUE);
	}

	void YumeD3D11Renderer::Close()
	{
		if(window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(window_);
			window_ = 0;
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

		if(!window_)
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

		YumeRHI::SetGraphicsMode(width,height,fullscreen,borderless,resizable,vsync,tripleBuffer,multiSample);

		AdjustWindow(width,height,fullscreen,borderless);

		if(maximize)
		{
			Maximize();
			SDL_GetWindowSize(window_,&width,&height);
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
		swapChainDesc.OutputWindow = GetWindowHandle(window_);
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
		dxgiFactory->MakeWindowAssociation(GetWindowHandle(window_),DXGI_MWA_NO_ALT_ENTER);

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

	YumeShaderVariation* YumeD3D11Renderer::GetShader(ShaderType type,const YumeString& name,const YumeString& defines) const
	{
		return GetShader(type,name.c_str(),defines.c_str());
	}

	YumeShaderVariation* YumeD3D11Renderer::GetShader(ShaderType type,const char* name,const char* defines) const
	{
		if(lastShaderName_ != name || !lastShader_)
		{
			YumeResourceManager* resource_ = YumeEngine3D::Get()->GetResourceManager();
			YumeString fullShaderName = shaderPath_ + name + shaderExtension_;
			// Try to reduce repeated error log prints because of missing shaders
			if(lastShaderName_ == name && !resource_->Exists(fullShaderName))
				return 0;

			lastShader_ = resource_->PrepareResource<YumeD3D11Shader>(fullShaderName);
			lastShaderName_ = name;
		}

		return lastShader_ ? lastShader_->GetVariation(type,defines) : (YumeD3D11ShaderVariation*)0;
	}

	void YumeD3D11Renderer::SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps)
	{
		// Switch to the clip plane variations if necessary
		/// \todo Causes overhead and string manipulation per drawcall
		if(useClipPlane_)
		{
			if(vs)
				vs = vs->GetOwner()->GetVariation(VS,vs->GetDefines() + " CLIPPLANE");
			if(ps)
				ps = ps->GetOwner()->GetVariation(PS,ps->GetDefines() + " CLIPPLANE");
		}

		if(vs == vertexShader_ && ps == pixelShader_)
			return;

		if(vs != vertexShader_)
		{
			YumeD3D11ShaderVariation* vsVar_ = static_cast<YumeD3D11ShaderVariation*>(vs);
			// Create the shader now if not yet created. If already attempted, do not retry
			if(vsVar_ && !vsVar_->GetGPUObject())
			{
				if(vsVar_->GetCompilerOutput().empty())
				{
					bool success = vs->Create();
					if(!success)
					{
						YUMELOG_ERROR("Failed to compile vertex shader " + vs->GetFullName() + ":\n" + vs->GetCompilerOutput());
						vs = 0;
					}
				}
				else
					vs = 0;
			}

			impl_->deviceContext_->VSSetShader((ID3D11VertexShader*)(vs ? vsVar_->GetGPUObject() : 0),0,0);
			vertexShader_ = vsVar_;
			vertexDeclarationDirty_ = true;
		}

		if(ps != pixelShader_)
		{
			YumeD3D11ShaderVariation* psVar_ = static_cast<YumeD3D11ShaderVariation*>(ps);
			if(psVar_ && !psVar_->GetGPUObject())
			{
				if(ps->GetCompilerOutput().empty())
				{
					bool success = ps->Create();
					if(!success)
					{
						YUMELOG_ERROR("Failed to compile pixel shader " + ps->GetFullName() + ":\n" + ps->GetCompilerOutput());
						ps = 0;
					}
				}
				else
					ps = 0;
			}

			impl_->deviceContext_->PSSetShader((ID3D11PixelShader*)(psVar_ ? psVar_->GetGPUObject() : 0),0,0);
			pixelShader_ = psVar_;
		}

		// Update current shader parameters & constant buffers
		if(vertexShader_ && pixelShader_)
		{
			std::pair<YumeShaderVariation*,YumeShaderVariation*> key = std::make_pair(vertexShader_,pixelShader_);
			ShaderProgramMap::iterator i = shaderPrograms_.find(key);
			if(i != shaderPrograms_.end())
				shaderProgram_ = i->second.get();
			else
			{
				SharedPtr<YumeD3D11ShaderProgram> newProgram = shaderPrograms_[key] = SharedPtr<YumeD3D11ShaderProgram>(new YumeD3D11ShaderProgram(this,vertexShader_,pixelShader_));
				shaderProgram_ = newProgram.get();
			}

			bool vsBuffersChanged = false;
			bool psBuffersChanged = false;

			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				if(shaderProgram_->vsConstantBuffers_[i])
				{
					ID3D11Buffer* vsBuffer = shaderProgram_->vsConstantBuffers_[i] ? (ID3D11Buffer*)static_cast<YumeD3D11ConstantBuffer*>(shaderProgram_->vsConstantBuffers_[i])->
						GetGPUObject() : 0;
					if(vsBuffer != impl_->constantBuffers_[VS][i])
					{
						impl_->constantBuffers_[VS][i] = vsBuffer;
						shaderParameterSources_[i] = (const void*)0xffffffff;
						vsBuffersChanged = true;
					}
				}

				if(shaderProgram_->psConstantBuffers_[i])
				{
					ID3D11Buffer* psBuffer = shaderProgram_->psConstantBuffers_[i] ? (ID3D11Buffer*)static_cast<YumeD3D11ConstantBuffer*>(shaderProgram_->psConstantBuffers_[i])->
						GetGPUObject() : 0;
					if(psBuffer != impl_->constantBuffers_[PS][i])
					{
						impl_->constantBuffers_[PS][i] = psBuffer;
						shaderParameterSources_[i] = (const void*)0xffffffff;
						psBuffersChanged = true;
					}
				}


			}

			if(vsBuffersChanged)
				impl_->deviceContext_->VSSetConstantBuffers(0,MAX_SHADER_PARAMETER_GROUPS,&impl_->constantBuffers_[VS][0]);
			if(psBuffersChanged)
				impl_->deviceContext_->PSSetConstantBuffers(0,MAX_SHADER_PARAMETER_GROUPS,&impl_->constantBuffers_[PS][0]);
		}
		else
			shaderProgram_ = 0;

		// Store shader combination if shader dumping in progress
		/*if(shaderPrecache_)
			shaderPrecache_->StoreShaders(vertexShader_,pixelShader_);*/

		// Update clip plane parameter if necessary
		if(useClipPlane_)
			SetShaderParameter(VSP_CLIPPLANE,clipPlane_);
	}

	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const float* data,unsigned count)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,(unsigned)(count * sizeof(float)),data);
	}
	/// Set shader float constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,float value)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(float),&value);
	}
	/// Set shader boolean constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,bool value)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(bool),&value);
	}
	/// Set shader color constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const YumeColor& color)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(YumeColor),&color);
	}
	/// Set shader 2D vector constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const Vector2& vector)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Vector2),&vector);
	}
	/// Set shader 3x3 matrix constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const Matrix3& matrix)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Matrix3),&matrix);
	}

	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const Matrix3x4& matrix)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Matrix3x4),&matrix);
	}

	/// Set shader 3D vector constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const Vector3& vector)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Vector3),&vector);
	}
	/// Set shader 4x4 matrix constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash  param,const Matrix4& matrix)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Matrix4),&matrix);
	}
	/// Set shader 4D vector constant.
	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const Vector4& vector)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Vector4),&vector);
	}

	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const YumeVariant& value)
	{
		switch(value.GetType())
		{
		case VAR_BOOL:
			SetShaderParameter(param,value.Get<bool>());
			break;

		case VAR_FLOAT:
			SetShaderParameter(param,value.Get<float>());
			break;
		default:
			break;
		}
	}

	void YumeD3D11Renderer::SetTexture(unsigned index,YumeTexture* texture)
	{
		if(index >= MAX_TEXTURE_UNITS)
			return;

		// Check if texture is currently bound as a rendertarget. In that case, use its backup texture, or blank if not defined
		if(texture)
		{
			if(renderTargets_[0] && renderTargets_[0]->GetParentTexture() == texture)
				texture = texture->GetBackupTexture();
		}

		if(texture && texture->GetParametersDirty())
		{
			texture->UpdateParameters();
			textures_[index] = 0; // Force reassign
		}

		if(texture != textures_[index])
		{
			if(firstDirtyTexture_ == Math::M_MAX_UNSIGNED)
				firstDirtyTexture_ = lastDirtyTexture_ = index;
			else
			{
				if(index < firstDirtyTexture_)
					firstDirtyTexture_ = index;
				if(index > lastDirtyTexture_)
					lastDirtyTexture_ = index;
			}

			textures_[index] = texture;
			impl_->shaderResourceViews_[index] = texture ? (ID3D11ShaderResourceView*)texture->GetShaderResourceView() : 0;
			impl_->samplers_[index] = texture ? (ID3D11SamplerState*)texture->GetSampler() : 0;
			texturesDirty_ = true;
		}
	}

	YumeVertexBuffer* YumeD3D11Renderer::CreateVertexBuffer()
	{
		return YumeAPINew YumeD3D11VertexBuffer(this);
	}

	YumeIndexBuffer* YumeD3D11Renderer::CreateIndexBuffer()
	{
		return YumeAPINew YumeD3D11IndexBuffer(this);
	}

	YumeInputLayout* YumeD3D11Renderer::CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks)
	{
		return YumeAPINew YumeD3D11InputLayout(this,vertexShader,buffers,elementMasks);
	}

	void YumeD3D11Renderer::CleanUpShaderPrograms(YumeShaderVariation* variation)
	{
		for(ShaderProgramMap::iterator i = shaderPrograms_.begin(); i != shaderPrograms_.end();)
		{
			if(i->first.first == variation || i->first.second == variation)
				i = shaderPrograms_.erase(i);
			else
				++i;
		}

		if(vertexShader_ == variation || pixelShader_ == variation)
			shaderProgram_ = 0;
	}



	YumeConstantBuffer* YumeD3D11Renderer::GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size)
	{
		// Ensure that different shader types and index slots get unique buffers, even if the size is same
		unsigned key = type | (index << 1) | (size << 4);
		YumeMap<unsigned,SharedPtr<YumeConstantBuffer> >::iterator i = constantBuffers_.find(key);
		if(i != constantBuffers_.end())
			return i->second.get();
		else
		{
			SharedPtr<YumeConstantBuffer> newConstantBuffer(new YumeD3D11ConstantBuffer(this));
			newConstantBuffer->SetSize(size);
			constantBuffers_[key] = newConstantBuffer;
			return newConstantBuffer.get();
		}
	}

	void YumeD3D11Renderer::SetVertexBuffer(YumeVertexBuffer* buffer)
	{
		static YumeVector<YumeVertexBuffer*>::type vertexBuffers(1);
		static YumeVector<unsigned>::type elementMasks(1);
		vertexBuffers[0] = buffer;
		elementMasks[0] = MASK_DEFAULT;
		SetVertexBuffers(vertexBuffers,elementMasks);
	}

	bool YumeD3D11Renderer::SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset)
	{
		if(buffers.size() > MAX_VERTEX_STREAMS)
		{
			YUMELOG_ERROR("Too many vertex buffers");
			return false;
		}
		if(buffers.size() != elementMasks.size())
		{
			YUMELOG_ERROR("Amount of element masks and vertex buffers does not match");
			return false;
		}

		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			YumeD3D11VertexBuffer* buffer = 0;
			bool changed = false;

			buffer = i < buffers.size() ? static_cast<YumeD3D11VertexBuffer*>(buffers[i]) : 0;
			if(buffer)
			{
				unsigned elementMask = buffer->GetElementMask() & elementMasks[i];
				unsigned offset = (elementMask & MASK_INSTANCEMATRIX1) ? instanceOffset * buffer->GetVertexSize() : 0;

				if(buffer != vertexBuffers_[i] || elementMask != elementMasks_[i] || offset != impl_->vertexOffsets_[i])
				{
					vertexBuffers_[i] = buffer;
					elementMasks_[i] = elementMask;
					impl_->vertexBuffers_[i] = (ID3D11Buffer*)buffer->GetGPUObject();
					impl_->vertexSizes_[i] = buffer->GetVertexSize();
					impl_->vertexOffsets_[i] = offset;
					changed = true;
				}
			}
			else if(vertexBuffers_[i])
			{
				vertexBuffers_[i] = 0;
				elementMasks_[i] = 0;
				impl_->vertexBuffers_[i] = 0;
				impl_->vertexSizes_[i] = 0;
				impl_->vertexOffsets_[i] = 0;
				changed = true;
			}

			if(changed)
			{
				vertexDeclarationDirty_ = true;

				if(firstDirtyVB_ == Math::M_MAX_UNSIGNED)
					firstDirtyVB_ = lastDirtyVB_ = i;
				else
				{
					if(i < firstDirtyVB_)
						firstDirtyVB_ = i;
					if(i > lastDirtyVB_)
						lastDirtyVB_ = i;
				}
			}
		}

		return true;
	}

	bool YumeD3D11Renderer::SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset)
	{
		return SetVertexBuffers(reinterpret_cast<const YumeVector<YumeVertexBuffer*>::type&>(buffers),elementMasks,instanceOffset);
	}

	void YumeD3D11Renderer::SetIndexBuffer(YumeIndexBuffer* buffer)
	{
		if(buffer != indexBuffer_)
		{
			if(buffer)
				impl_->deviceContext_->IASetIndexBuffer((ID3D11Buffer*)static_cast<YumeD3D11IndexBuffer*>(buffer)->GetGPUObject(),
				static_cast<YumeD3D11IndexBuffer*>(buffer)->GetIndexSize() == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,0);
			else
				impl_->deviceContext_->IASetIndexBuffer(0,DXGI_FORMAT_UNKNOWN,0);

			indexBuffer_ = buffer;
		}
	}

	void YumeD3D11Renderer::PreDraw()
	{
		if(renderTargetsDirty_)
		{
			impl_->depthStencilView_ =
				depthStencil_ ? (ID3D11DepthStencilView*)depthStencil_->GetRenderTargetView() : impl_->defaultDepthStencilView_;

			// If possible, bind a read-only depth stencil view to allow reading depth in shader
			if(!depthWrite_ && depthStencil_ && depthStencil_->GetReadOnlyView())
				impl_->depthStencilView_ = (ID3D11DepthStencilView*)depthStencil_->GetReadOnlyView();

			for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
				impl_->renderTargetViews_[i] =
				renderTargets_[i] ? (ID3D11RenderTargetView*)renderTargets_[i]->GetRenderTargetView() : 0;
			// If rendertarget 0 is null and not doing depth-only rendering, render to the backbuffer
			// Special case: if rendertarget 0 is null and depth stencil has same size as backbuffer, assume the intention is to do
			// backbuffer rendering with a custom depth stencil
			if(!renderTargets_[0] &&
				(!depthStencil_ || (depthStencil_ && depthStencil_->GetWidth() == windowWidth_ && depthStencil_->GetHeight() == windowHeight_)))
				impl_->renderTargetViews_[0] = impl_->defaultRenderTargetView_;

			impl_->deviceContext_->OMSetRenderTargets(MAX_RENDERTARGETS,&impl_->renderTargetViews_[0],impl_->depthStencilView_);
			renderTargetsDirty_ = false;
		}

		if(texturesDirty_ && firstDirtyTexture_ < Math::M_MAX_UNSIGNED)
		{
			// Set also VS textures to enable vertex texture fetch to work the same way as on OpenGL
			impl_->deviceContext_->VSSetShaderResources(firstDirtyTexture_,lastDirtyTexture_ - firstDirtyTexture_ + 1,
				&impl_->shaderResourceViews_[firstDirtyTexture_]);
			impl_->deviceContext_->VSSetSamplers(firstDirtyTexture_,lastDirtyTexture_ - firstDirtyTexture_ + 1,
				&impl_->samplers_[firstDirtyTexture_]);
			impl_->deviceContext_->PSSetShaderResources(firstDirtyTexture_,lastDirtyTexture_ - firstDirtyTexture_ + 1,
				&impl_->shaderResourceViews_[firstDirtyTexture_]);
			impl_->deviceContext_->PSSetSamplers(firstDirtyTexture_,lastDirtyTexture_ - firstDirtyTexture_ + 1,
				&impl_->samplers_[firstDirtyTexture_]);

			firstDirtyTexture_ = lastDirtyTexture_ = Math::M_MAX_UNSIGNED;
			texturesDirty_ = false;
		}

		if(vertexDeclarationDirty_ && vertexShader_ && vertexShader_->GetByteCode().size())
		{
			if(firstDirtyVB_ < Math::M_MAX_UNSIGNED)
			{
				impl_->deviceContext_->IASetVertexBuffers(firstDirtyVB_,lastDirtyVB_ - firstDirtyVB_ + 1,
					&impl_->vertexBuffers_[firstDirtyVB_],&impl_->vertexSizes_[firstDirtyVB_],&impl_->vertexOffsets_[firstDirtyVB_]);

				firstDirtyVB_ = lastDirtyVB_ = Math::M_MAX_UNSIGNED;
			}

			unsigned long long newVertexDeclarationHash = 0;
			for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
				newVertexDeclarationHash |= (unsigned long long)elementMasks_[i] << (i * 13);

			// Do not create input layout if no vertex buffers / elements
			if(newVertexDeclarationHash)
			{
				newVertexDeclarationHash |= (unsigned long long)vertexShader_->GetElementMask() << 51;
				if(newVertexDeclarationHash != vertexDeclarationHash_)
				{
					YumeMap<unsigned long long,SharedPtr<YumeInputLayout>>::iterator
						i = vertexDeclarations_.find(newVertexDeclarationHash);
					std::pair<YumeMap<unsigned long long,SharedPtr<YumeInputLayout>>::iterator,bool> ret;
					if(i == vertexDeclarations_.end())
					{
						SharedPtr<YumeInputLayout> newVertexDeclaration(new YumeD3D11InputLayout(this,vertexShader_,vertexBuffers_,
							elementMasks_));
						ret = vertexDeclarations_.insert(std::make_pair(newVertexDeclarationHash,newVertexDeclaration));

					}

					impl_->deviceContext_->IASetInputLayout((ID3D11InputLayout*)ret.first->second->GetInputLayout());
					vertexDeclarationHash_ = newVertexDeclarationHash;
				}
			}

			vertexDeclarationDirty_ = false;
		}

		if(blendStateDirty_)
		{
			unsigned newBlendStateHash = (unsigned)((colorWrite_ ? 1 : 0) | (blendMode_ << 1));
			if(newBlendStateHash != blendStateHash_)
			{
				YumeMap<unsigned,ID3D11BlendState*>::iterator i = impl_->blendStates_.find(newBlendStateHash);
				std::pair<YumeMap<unsigned,ID3D11BlendState*>::iterator,bool> ret;
				if(i == impl_->blendStates_.end())
				{
					D3D11_BLEND_DESC stateDesc;
					memset(&stateDesc,0,sizeof stateDesc);
					stateDesc.AlphaToCoverageEnable = false;
					stateDesc.IndependentBlendEnable = false;
					stateDesc.RenderTarget[0].BlendEnable = d3dBlendEnable[blendMode_];
					stateDesc.RenderTarget[0].SrcBlend = d3dSrcBlend[blendMode_];
					stateDesc.RenderTarget[0].DestBlend = d3dDestBlend[blendMode_];
					stateDesc.RenderTarget[0].BlendOp = d3dBlendOp[blendMode_];
					stateDesc.RenderTarget[0].SrcBlendAlpha = d3dSrcBlend[blendMode_];
					stateDesc.RenderTarget[0].DestBlendAlpha = d3dDestBlend[blendMode_];
					stateDesc.RenderTarget[0].BlendOpAlpha = d3dBlendOp[blendMode_];
					stateDesc.RenderTarget[0].RenderTargetWriteMask = colorWrite_ ? D3D11_COLOR_WRITE_ENABLE_ALL : 0x0;

					ID3D11BlendState* newBlendState = 0;
					HRESULT hr = impl_->device_->CreateBlendState(&stateDesc,&newBlendState);
					if(FAILED(hr))
					{
						D3D_SAFE_RELEASE(newBlendState);
						YUMELOG_ERROR("Failed to create blend state",hr);
					}


					ret = impl_->blendStates_.insert(std::make_pair(newBlendStateHash,newBlendState));
				}

				impl_->deviceContext_->OMSetBlendState(ret.first->second,0,Math::M_MAX_UNSIGNED);
				blendStateHash_ = newBlendStateHash;
			}

			blendStateDirty_ = false;
		}

		if(depthStateDirty_)
		{
			unsigned newDepthStateHash =
				(depthWrite_ ? 1 : 0) | (stencilTest_ ? 2 : 0) | (depthTestMode_ << 2) | ((stencilCompareMask_ & 0xff) << 5) |
				((stencilWriteMask_ & 0xff) << 13) | (stencilTestMode_ << 21) |
				((stencilFail_ + stencilZFail_ * 5 + stencilPass_ * 25) << 24);
			if(newDepthStateHash != depthStateHash_ || stencilRefDirty_)
			{
				YumeMap<unsigned,ID3D11DepthStencilState*>::iterator i = impl_->depthStates_.find(newDepthStateHash);
				std::pair<YumeMap<unsigned,ID3D11DepthStencilState*>::iterator,bool> ret;
				if(i == impl_->depthStates_.end())
				{
					D3D11_DEPTH_STENCIL_DESC stateDesc;
					memset(&stateDesc,0,sizeof stateDesc);
					stateDesc.DepthEnable = TRUE;
					stateDesc.DepthWriteMask = depthWrite_ ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
					stateDesc.DepthFunc = d3dCmpFunc[depthTestMode_];
					stateDesc.StencilEnable = stencilTest_ ? TRUE : FALSE;
					stateDesc.StencilReadMask = (unsigned char)stencilCompareMask_;
					stateDesc.StencilWriteMask = (unsigned char)stencilWriteMask_;
					stateDesc.FrontFace.StencilFailOp = d3dStencilOp[stencilFail_];
					stateDesc.FrontFace.StencilDepthFailOp = d3dStencilOp[stencilZFail_];
					stateDesc.FrontFace.StencilPassOp = d3dStencilOp[stencilPass_];
					stateDesc.FrontFace.StencilFunc = d3dCmpFunc[stencilTestMode_];
					stateDesc.BackFace.StencilFailOp = d3dStencilOp[stencilFail_];
					stateDesc.BackFace.StencilDepthFailOp = d3dStencilOp[stencilZFail_];
					stateDesc.BackFace.StencilPassOp = d3dStencilOp[stencilPass_];
					stateDesc.BackFace.StencilFunc = d3dCmpFunc[stencilTestMode_];

					ID3D11DepthStencilState* newDepthState = 0;
					HRESULT hr = impl_->device_->CreateDepthStencilState(&stateDesc,&newDepthState);
					if(FAILED(hr))
					{
						D3D_SAFE_RELEASE(newDepthState);
						YUMELOG_ERROR("Failed to create depth state",hr);
					}

					ret = impl_->depthStates_.insert(std::make_pair(newDepthStateHash,newDepthState));

				}

				impl_->deviceContext_->OMSetDepthStencilState(ret.first->second,stencilRef_);
				depthStateHash_ = newDepthStateHash;
			}

			depthStateDirty_ = false;
			stencilRefDirty_ = false;
		}

		if(rasterizerStateDirty_)
		{
			unsigned depthBits = 24;
			if(depthStencil_ && depthStencil_->GetParentTexture()->GetFormat() == DXGI_FORMAT_R16_TYPELESS)
				depthBits = 16;
			int scaledDepthBias = (int)(constantDepthBias_ * (1 << depthBits));

			unsigned newRasterizerStateHash =
				(scissorTest_ ? 1 : 0) | (fillMode_ << 1) | (cullMode_ << 3) | ((scaledDepthBias & 0x1fff) << 5) |
				((*((unsigned*)&slopeScaledDepthBias_) & 0x1fff) << 18);
			if(newRasterizerStateHash != rasterizerStateHash_)
			{
				YumeMap<unsigned,ID3D11RasterizerState*>::iterator i = impl_->rasterizerStates_.find(newRasterizerStateHash);
				std::pair<YumeMap<unsigned,ID3D11RasterizerState*>::iterator,bool> ret;
				if(i == impl_->rasterizerStates_.end())
				{
					D3D11_RASTERIZER_DESC stateDesc;
					memset(&stateDesc,0,sizeof stateDesc);
					stateDesc.FillMode = d3dFillMode[fillMode_];
					stateDesc.CullMode = d3dCullMode[cullMode_];
					stateDesc.FrontCounterClockwise = FALSE;
					stateDesc.DepthBias = scaledDepthBias;
					stateDesc.DepthBiasClamp = Math::POS_INFINITY;
					stateDesc.SlopeScaledDepthBias = slopeScaledDepthBias_;
					stateDesc.DepthClipEnable = TRUE;
					stateDesc.ScissorEnable = scissorTest_ ? TRUE : FALSE;
					stateDesc.MultisampleEnable = TRUE;
					stateDesc.AntialiasedLineEnable = FALSE;

					ID3D11RasterizerState* newRasterizerState = 0;
					HRESULT hr = impl_->device_->CreateRasterizerState(&stateDesc,&newRasterizerState);
					if(FAILED(hr))
					{
						D3D_SAFE_RELEASE(newRasterizerState);
						YUMELOG_ERROR("Failed to create rasterizer state" << hr);
					}

					ret = impl_->rasterizerStates_.insert(std::make_pair(newRasterizerStateHash,newRasterizerState));
				}

				impl_->deviceContext_->RSSetState(ret.first->second);
				rasterizerStateHash_ = newRasterizerStateHash;
			}

			rasterizerStateDirty_ = false;
		}

		if(scissorRectDirty_)
		{
			D3D11_RECT d3dRect;
			d3dRect.left = scissorRect_.left_;
			d3dRect.top = scissorRect_.top_;
			d3dRect.right = scissorRect_.right_;
			d3dRect.bottom = scissorRect_.bottom_;
			impl_->deviceContext_->RSSetScissorRects(1,&d3dRect);
			scissorRectDirty_ = false;
		}

		for(unsigned i = 0; i < dirtyConstantBuffers_.size(); ++i)
			dirtyConstantBuffers_[i]->Apply();
		dirtyConstantBuffers_.clear();
	}

	void YumeD3D11Renderer::Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount)
	{
		if(!vertexCount || !shaderProgram_)
			return;

		PreDraw();

		unsigned primitiveCount;
		D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType;

		if(fillMode_ == FILL_POINT)
			type = POINT_LIST;

		GetD3DPrimitiveType(vertexCount,type,primitiveCount,d3dPrimitiveType);
		if(d3dPrimitiveType != primitiveType_)
		{
			impl_->deviceContext_->IASetPrimitiveTopology(d3dPrimitiveType);
			primitiveType_ = d3dPrimitiveType;
		}
		impl_->deviceContext_->Draw(vertexCount,vertexStart);

		numPrimitives_ += primitiveCount;
		++numBatches_;
	}

	void YumeD3D11Renderer::Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount)
	{
		if(!vertexCount || !shaderProgram_)
			return;

		PreDraw();

		unsigned primitiveCount;
		D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType;

		if(fillMode_ == FILL_POINT)
			type = POINT_LIST;

		GetD3DPrimitiveType(indexCount,type,primitiveCount,d3dPrimitiveType);
		if(d3dPrimitiveType != primitiveType_)
		{
			impl_->deviceContext_->IASetPrimitiveTopology(d3dPrimitiveType);
			primitiveType_ = d3dPrimitiveType;
		}
		impl_->deviceContext_->DrawIndexed(indexCount,indexStart,0);

		numPrimitives_ += primitiveCount;
		++numBatches_;
	}

	void YumeD3D11Renderer::DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,
		unsigned instanceCount)
	{
		if(!indexCount || !instanceCount || !shaderProgram_)
			return;

		PreDraw();

		unsigned primitiveCount;
		D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType;

		if(fillMode_ == FILL_POINT)
			type = POINT_LIST;

		GetD3DPrimitiveType(indexCount,type,primitiveCount,d3dPrimitiveType);
		if(d3dPrimitiveType != primitiveType_)
		{
			impl_->deviceContext_->IASetPrimitiveTopology(d3dPrimitiveType);
			primitiveType_ = d3dPrimitiveType;
		}
		impl_->deviceContext_->DrawIndexedInstanced(indexCount,instanceCount,indexStart,0,0);

		numPrimitives_ += instanceCount * primitiveCount;
		++numBatches_;
	}


	void YumeD3D11Renderer::Maximize()
	{
		if(!window_)
			return;

		SDL_MaximizeWindow(window_);
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

	unsigned YumeD3D11Renderer::GetFormat(CompressedFormat format) const
	{
		switch(format)
		{
		case CF_RGBA:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case CF_DXT1:
			return DXGI_FORMAT_BC1_UNORM;

		case CF_DXT3:
			return DXGI_FORMAT_BC2_UNORM;

		case CF_DXT5:
			return DXGI_FORMAT_BC3_UNORM;

		default:
			return 0;
		}
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

	void YumeD3D11Renderer::RegisterFactories()
	{
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(GenerateHash("Shader"),[](void) -> YumeBase * { return new YumeD3D11Shader();});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(GenerateHash("Texture2D"),[this](void) -> YumeBase * { return new YumeD3D11Texture2D(this);});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(GenerateHash("IndexBuffer"),[this](void) -> YumeBase * { return new YumeD3D11IndexBuffer(this);});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(GenerateHash("VertexBuffer"),[this](void) -> YumeBase * { return new YumeD3D11VertexBuffer(this);});
	}
	void YumeD3D11Renderer::UnregisterFactories()
	{
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(GenerateHash("Shader"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(GenerateHash("Texture2D"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(GenerateHash("IndexBuffer"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(GenerateHash("VertexBuffer"));
	}


	unsigned YumeD3D11Renderer::GetAlphaFormat()
	{
		return DXGI_FORMAT_A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetLuminanceFormat()
	{
		// Note: not same sampling behavior as on D3D9; need to sample the R channel only
		return DXGI_FORMAT_R8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetLuminanceAlphaFormat()
	{
		// Note: not same sampling behavior as on D3D9; need to sample the RG channels
		return DXGI_FORMAT_R8G8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBFormat()
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBAFormat()
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBA16Format()
	{
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBAFloat16Format()
	{
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRGBAFloat32Format()
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRG16Format()
	{
		return DXGI_FORMAT_R16G16_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGFloat16Format()
	{
		return DXGI_FORMAT_R16G16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRGFloat32Format()
	{
		return DXGI_FORMAT_R32G32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetFloat16Format()
	{
		return DXGI_FORMAT_R16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetFloat32Format()
	{
		return DXGI_FORMAT_R32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetLinearDepthFormat()
	{
		return DXGI_FORMAT_R32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetDepthStencilFormat()
	{
		return DXGI_FORMAT_R24G8_TYPELESS;
	}

	unsigned YumeD3D11Renderer::GetReadableDepthFormat()
	{
		return DXGI_FORMAT_R24G8_TYPELESS;
	}

	unsigned YumeD3D11Renderer::GetFormat(const YumeString& formatName)
	{

		YumeString nameLower = formatName;
		boost::to_lower(nameLower);
		boost::trim(nameLower);

		if(nameLower == "a")
			return GetAlphaFormat();
		if(nameLower == "l")
			return GetLuminanceFormat();
		if(nameLower == "la")
			return GetLuminanceAlphaFormat();
		if(nameLower == "rgb")
			return GetRGBFormat();
		if(nameLower == "rgba")
			return GetRGBAFormat();
		if(nameLower == "rgba16")
			return GetRGBA16Format();
		if(nameLower == "rgba16f")
			return GetRGBAFloat16Format();
		if(nameLower == "rgba32f")
			return GetRGBAFloat32Format();
		if(nameLower == "rg16")
			return GetRG16Format();
		if(nameLower == "rg16f")
			return GetRGFloat16Format();
		if(nameLower == "rg32f")
			return GetRGFloat32Format();
		if(nameLower == "r16f")
			return GetFloat16Format();
		if(nameLower == "r32f" || nameLower == "float")
			return GetFloat32Format();
		if(nameLower == "lineardepth" || nameLower == "depth")
			return GetLinearDepthFormat();
		if(nameLower == "d24s8")
			return GetDepthStencilFormat();
		if(nameLower == "readabledepth" || nameLower == "hwdepth")
			return GetReadableDepthFormat();

		return GetRGBFormat();
	}
}
