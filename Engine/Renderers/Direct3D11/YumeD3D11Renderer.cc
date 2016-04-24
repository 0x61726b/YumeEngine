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
#include "Math/YumePlane.h"

#include "YumeD3D11Shader.h"
#include "YumeD3D11ShaderVariation.h"
#include "YumeD3D11VertexBuffer.h"
#include "YumeD3D11ConstantBuffer.h"
#include "YumeD3D11ShaderProgram.h"
#include "YumeD3D11Texture2D.h"
#include "YumeD3D11IndexBuffer.h"
#include "YumeD3D11InputLayout.h"

#include "YumeD3D11Texture2D.h"
#include "YumeD3D11Texture3D.h"
#include "YumeD3D11TextureCube.h"

#include "Renderer/YumeRenderable.h"
#include "Renderer/YumeGeometry.h"
#include "Renderer/YumeImage.h"

#include "Core/YumeBase.h"
#include "Core/YumeDefaults.h"
#include "Math/YumeMath.h"

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
		unsigned r = (unsigned)(YumeEngine::Clamp(color.r_ * 255.0f,0.0f,255.0f));
		unsigned g = (unsigned)(YumeEngine::Clamp(color.g_ * 255.0f,0.0f,255.0f));
		unsigned b = (unsigned)(YumeEngine::Clamp(color.b_ * 255.0f,0.0f,255.0f));
		unsigned a = (unsigned)(YumeEngine::Clamp(color.a_ * 255.0f,0.0f,255.0f));
		return (((a)& 0xff) << 24) | (((r)& 0xff) << 16) | (((g)& 0xff) << 8) | ((b)& 0xff);
	}

	extern "C" void YumeD3DExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeD3D11Renderer* renderer = (new YumeD3D11Renderer);
		engine->SetRenderer(renderer);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeD3DExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* renderer = gYume->pRHI;
		delete renderer;
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

	}

	bool YumeD3D11Renderer::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		if(fullscreen_ && (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED))
			return false;

		ResetRenderTargets();

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			SetTexture(i,0);

		numPrimitives_ = 0;
		numBatches_ = 0;

		return true;
	}

	void YumeD3D11Renderer::EndFrame()
	{
		if(!IsInitialized())
			return;

		impl_->swapChain_->Present(vsync_ ? 1 : 0,0);

		CleanupScratchBuffers();
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

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			textures_[i] = 0;
			impl_->shaderResourceViews_[i] = 0;
			impl_->samplers_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			renderTargets_[i] = 0;
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

		noDs_ = false;

		indexBuffer_ = 0;
		vertexDeclarationHash_ = 0;
		primitiveType_ = 0;
		vertexShader_ = 0;
		pixelShader_ = 0;
		geometryShader_ = 0;
		shaderProgram_ = 0;
		blendMode_ = BLEND_REPLACE;
		textureAnisotropy_ = 1;
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
		stencilZFailBack_ = OP_KEEP;
		stencilRef_ = 0;
		stencilCompareMask_ = M_MAX_UNSIGNED;
		stencilWriteMask_ = M_MAX_UNSIGNED;
		useClipPlane_ = false;
		renderTargetsDirty_ = true;
		texturesDirty_ = true;
		vertexDeclarationDirty_ = true;
		blendStateDirty_ = true;
		depthStateDirty_ = true;
		rasterizerStateDirty_ = true;
		scissorRectDirty_ = true;
		stencilRefDirty_ = true;
		blendStateHash_ = M_MAX_UNSIGNED;
		depthStateHash_ = M_MAX_UNSIGNED;
		rasterizerStateHash_ = M_MAX_UNSIGNED;
		firstDirtyTexture_ = lastDirtyTexture_ = M_MAX_UNSIGNED;
		firstDirtyVB_ = lastDirtyVB_ = M_MAX_UNSIGNED;
		dirtyConstantBuffers_.clear();
	}

	void YumeD3D11Renderer::ClearDepthStencil(unsigned flags,float depth,unsigned stencil)
	{
		unsigned depthClearFlags = 0;
		if(flags & CLEAR_DEPTH)
			depthClearFlags |= D3D11_CLEAR_DEPTH;
		if(flags & CLEAR_STENCIL)
			depthClearFlags |= D3D11_CLEAR_STENCIL;

		if(depthStencil_)
			impl_->deviceContext_->ClearDepthStencilView((ID3D11DepthStencilView*)depthStencil_->GetRenderTargetView(),depthClearFlags,depth,(UINT8)stencil);
		else
			impl_->deviceContext_->ClearDepthStencilView(impl_->depthStencilView_,depthClearFlags,depth,(UINT8)stencil);
	}

	void YumeD3D11Renderer::Clear(unsigned flags,const YumeColor& color,float depth,unsigned stencil)
	{
		IntVector2 rtSize = GetRenderTargetDimensions();

		

		SetDepthWrite(true);
		PreDraw();

		if((flags & CLEAR_COLOR) && impl_->renderTargetViews_[0])
		{
			for(int i=0; i < MAX_RENDERTARGETS;++i)
			{
				if(impl_->renderTargetViews_[i])
					impl_->deviceContext_->ClearRenderTargetView(impl_->renderTargetViews_[i],color.Data());
			}
		}

		if((flags & (CLEAR_DEPTH | CLEAR_STENCIL)) && impl_->depthStencilView_)
		{
			unsigned depthClearFlags = 0;
			if(flags & CLEAR_DEPTH)
				depthClearFlags |= D3D11_CLEAR_DEPTH;
			if(flags & CLEAR_STENCIL)
				depthClearFlags |= D3D11_CLEAR_STENCIL;
			impl_->deviceContext_->ClearDepthStencilView(impl_->depthStencilView_,depthClearFlags,depth,(UINT8)stencil);
		}
	}

	void YumeD3D11Renderer::ClearRenderTarget(unsigned index,unsigned flags,const YumeColor& color,float depth,unsigned stencil)
	{
		impl_->deviceContext_->ClearRenderTargetView((ID3D11RenderTargetView*)renderTargets_[index]->GetRenderTargetView(),color.Data());
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
		IntVector2 size = GetRenderTargetDimensions();

		IntRect rectCopy = rect;

		if(rectCopy.right_ <= rectCopy.left_)
			rectCopy.right_ = rectCopy.left_ + 1;
		if(rectCopy.bottom_ <= rectCopy.top_)
			rectCopy.bottom_ = rectCopy.top_ + 1;
		rectCopy.left_ = Clamp(rectCopy.left_,0,size.x_);
		rectCopy.top_ = Clamp(rectCopy.top_,0,size.y_);
		rectCopy.right_ = Clamp(rectCopy.right_,0,size.x_);
		rectCopy.bottom_ = Clamp(rectCopy.bottom_,0,size.y_);

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
		SetScissorTest(false);
	}

	IntVector2 YumeD3D11Renderer::GetRenderTargetDimensions() const
	{
		int width,height;

		if(renderTargets_[0])
		{
			width = renderTargets_[0]->GetWidth();
			height = renderTargets_[0]->GetHeight();
		}
		else if(depthStencil_) // Depth-only rendering
		{
			width = depthStencil_->GetWidth();
			height = depthStencil_->GetHeight();
		}
		else
		{
			width = windowWidth_;
			height = windowHeight_;
		}

		return IntVector2(width,height);
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
			SDL_SetWindowPosition(window_,pos.x_,pos.y_);
		else
			windowPos_ = IntVector2(pos.x_,pos.y_); // Sets as initial position for OpenWindow()
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

		window_ = SDL_CreateWindow(windowTitle_.c_str(),windowPos_.x_,windowPos_.y_,width,height,flags);

		if(!window_)
		{
			YUMELOG_ERROR("Rendering window couldn't be created! Error:" << std::endl << SDL_GetError());
			return false;
		}
		SDL_GetWindowPosition(window_,&windowPos_.x_,&windowPos_.y_);


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
		{
			MutexLock lock(gpuResourceMutex_);

			for(int i=0; i < gpuResources_.size(); ++i)
			{
				if(gpuResources_[i])
					gpuResources_[i]->Release();
			}

			gpuResources_.clear();
		}


		// ToDo(arkenthera) not sure about this..
		int size = constantBuffers_.size();
		for(int i=0; i < size; ++i)
			constantBuffers_[i].Reset();
		constantBuffers_.clear();
		vertexDeclarations_.clear();

		BlendStatesMap::iterator blendIt;

		for(blendIt = impl_->blendStates_.begin();blendIt != impl_->blendStates_.end(); ++blendIt)
			D3D_SAFE_RELEASE(blendIt->second);

		impl_->blendStates_.clear();



		DepthStatesMap::iterator depthIt;

		for(depthIt = impl_->depthStates_.begin();depthIt != impl_->depthStates_.end(); ++depthIt)
			D3D_SAFE_RELEASE(depthIt->second);

		impl_->depthStates_.clear();



		RasterizerStatesMap::iterator rasterizerIt;

		for(rasterizerIt = impl_->rasterizerStates_.begin();rasterizerIt != impl_->rasterizerStates_.end(); ++rasterizerIt)
			D3D_SAFE_RELEASE(rasterizerIt->second);
		impl_->rasterizerStates_.clear();

		D3D_SAFE_RELEASE(standardFilter_);
		D3D_SAFE_RELEASE(lpvFilter_);
		D3D_SAFE_RELEASE(vplFilter_);
		D3D_SAFE_RELEASE(shadowFilter_);
		D3D_SAFE_RELEASE(bsInject_);
		D3D_SAFE_RELEASE(bsPropogate_);
		D3D_SAFE_RELEASE(dss_disableDepthTest_);
		D3D_SAFE_RELEASE(dss_enableDepthTest_);

		D3D_SAFE_RELEASE(impl_->defaultRenderTargetView_);
		D3D_SAFE_RELEASE(impl_->defaultDepthStencilView_);
		D3D_SAFE_RELEASE(impl_->defaultDepthTexture_);
		D3D_SAFE_RELEASE(impl_->resolveTexture_);
		D3D_SAFE_RELEASE(impl_->swapChain_);
		D3D_SAFE_RELEASE(impl_->deviceContext_);
		D3D_SAFE_RELEASE(impl_->device_);

		//lastShader_.reset();

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



		UnregisterFactories();

		// Shut down SDL now. Graphics should be the last SDL-using subsystem to be destroyed
		SDL_Quit();
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
					unsigned error = (unsigned)(Abs(resolutions[i].x_ - width) + Abs(resolutions[i].y_ - height));
					if(error < bestError)
					{
						best = i;
						bestError = error;
					}
				}

				width = resolutions[best].x_;
				height = resolutions[best].y_;
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

		CreateStandardSampler();
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

		if(!multisamples.Contains(multisample))
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



#ifdef _DEBUG
		hr = impl_->device_->QueryInterface(IID_ID3D11Debug,(void**)(&impl_->debug_));
		//ID3D11InfoQueue *d3dInfoQueue = nullptr;
		//impl_->device_->QueryInterface(__uuidof(ID3D11InfoQueue),(void**)&d3dInfoQueue);



		///*d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION,true);
		//d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR,true);*/

		//D3D11_MESSAGE_ID hide[] =
		//{
		//	D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
		//	(D3D11_MESSAGE_ID)3146081
		//};

		//D3D11_INFO_QUEUE_FILTER filter;
		//memset(&filter,0,sizeof(filter));
		//filter.DenyList.NumIDs = _countof(hide);
		//filter.DenyList.pIDList = hide;
		//d3dInfoQueue->AddStorageFilterEntries(&filter);
		//d3dInfoQueue->Release();
#endif

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

			impl_->defaultRenderTargetView_->SetPrivateData(WKPDID_D3DDebugObjectName,13,"BackBufferRTV");
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
			impl_->defaultDepthStencilView_->SetPrivateData(WKPDID_D3DDebugObjectName,13,"DefaultDSV");
		}

		// Update internally held backbuffer size
		windowWidth_ = width;
		windowHeight_ = height;

		ResetRenderTargets();
		return success;
	}

	YumeShaderVariation* YumeD3D11Renderer::GetShader(ShaderType type,const YumeString& name,const YumeString& defines,const YumeString& entryPoint) const
	{
		return GetShader(type,name.c_str(),defines.c_str(),entryPoint);
	}

	YumeShaderVariation* YumeD3D11Renderer::GetShader(ShaderType type,const char* name,const char* defines,const YumeString& entryPoint) const
	{
		if(lastEntryPoint != entryPoint || lastShaderName_ != name || !lastShader_)
		{
			YumeResourceManager* resource_ = gYume->pResourceManager;
			YumeString fullShaderName = shaderPath_ + name + shaderExtension_;
			// Try to reduce repeated error log prints because of missing shaders
			if(lastShaderName_ == name && lastEntryPoint == entryPoint && !resource_->Exists(fullShaderName))
				return 0;

			lastShader_ = resource_->PrepareResource<YumeD3D11Shader>(fullShaderName);
			lastShaderName_ = name;
			lastEntryPoint = entryPoint;
		}

		return lastShader_ ? lastShader_->GetVariation(type,defines,entryPoint) : (YumeD3D11ShaderVariation*)0;
	}

	bool YumeD3D11Renderer::HasShaderParameter(YumeHash param)
	{
		return shaderProgram_ && shaderProgram_->parameters_.find(param) != shaderProgram_->parameters_.end();
	}


	void YumeD3D11Renderer::SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps,YumeShaderVariation* gs)
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
						YUMELOG_ERROR("Failed to compile vertex shader " << vs->GetFullName().c_str() << ":\n" << vs->GetCompilerOutput().c_str());
						vs = 0;
					}
				}
				else
					vs = 0;
			}
			//debugging purposes
			/*ID3D11VertexShader* vvv = (ID3D11VertexShader*)vsVar_->GetGPUObject();*/
			impl_->deviceContext_->VSSetShader((ID3D11VertexShader*)(vs ? vsVar_->GetGPUObject() : 0),0,0);
			vertexShader_ = vs;
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
						YUMELOG_ERROR("Failed to compile pixel shader " << ps->GetFullName().c_str() << ":\n" << ps->GetCompilerOutput().c_str());
						ps = 0;
					}
				}
				else
					ps = 0;
			}
			/*ID3D11PixelShader* kappa = (ID3D11PixelShader*)psVar_->GetGPUObject();*/
			impl_->deviceContext_->PSSetShader((ID3D11PixelShader*)(psVar_ ? psVar_->GetGPUObject() : 0),0,0);
			pixelShader_ = ps;
		}


		if(gs && gs != geometryShader_)
		{
			YumeD3D11ShaderVariation* gsVar_ = static_cast<YumeD3D11ShaderVariation*>(gs);
			if(gsVar_ && !gsVar_->GetGPUObject())
			{
				if(gs->GetCompilerOutput().empty())
				{
					bool success = gs->Create();
					if(!success)
					{
						YUMELOG_ERROR("Failed to compile geometry shader " << gs->GetFullName().c_str() << ":\n" << gs->GetCompilerOutput().c_str());
						gs = 0;
					}
				}
				else
					gs = 0;
			}

			impl_->deviceContext_->GSSetShader((ID3D11GeometryShader*)(gsVar_ ? gsVar_->GetGPUObject() : 0),0,0);
			geometryShader_ = gs;
		}
		else if(!gs)
		{
			if(geometryShader_)
				impl_->deviceContext_->GSSetShader(NULL,0,0);
			geometryShader_ = 0;
		}

		if(vertexShader_ && pixelShader_)
		{
			Pair<YumeShaderVariation*,YumeShaderVariation*> p = MakePair(vertexShader_,pixelShader_);
			ShaderProgramMap::iterator i = shaderPrograms_.find(p);
			if(i != shaderPrograms_.end())
				shaderProgram_ = i->second;
			else
			{
				SharedPtr<YumeD3D11ShaderProgram> newProgram = shaderPrograms_[p] = (new YumeD3D11ShaderProgram(this,vertexShader_,pixelShader_,geometryShader_));
				shaderProgram_ = newProgram;
			}

			bool vsBuffersChanged = false;
			bool psBuffersChanged = false;
			bool gsBuffersChanged = false;

			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				ID3D11Buffer* vsBuffer = shaderProgram_->vsConstantBuffers_[i] ? (ID3D11Buffer*)static_cast<YumeD3D11ConstantBuffer*>(shaderProgram_->vsConstantBuffers_[i])->
					GetGPUObject() : 0;
				if(vsBuffer != impl_->constantBuffers_[VS][i])
				{
					impl_->constantBuffers_[VS][i] = vsBuffer;
					shaderParameterSources_[i] = (const void*)0xffffffff;
					vsBuffersChanged = true;
				}

				ID3D11Buffer* psBuffer = shaderProgram_->psConstantBuffers_[i] ? (ID3D11Buffer*)static_cast<YumeD3D11ConstantBuffer*>(shaderProgram_->psConstantBuffers_[i])->
					GetGPUObject() : 0;
				if(psBuffer != impl_->constantBuffers_[PS][i])
				{
					impl_->constantBuffers_[PS][i] = psBuffer;
					shaderParameterSources_[i] = (const void*)0xffffffff;
					psBuffersChanged = true;
				}

				if(geometryShader_)
				{
					ID3D11Buffer* gsBuffer = shaderProgram_->gsConstantBuffers_[i] ? (ID3D11Buffer*)static_cast<YumeD3D11ConstantBuffer*>(shaderProgram_->gsConstantBuffers_[i])->
						GetGPUObject() : 0;
					if(gsBuffer != impl_->constantBuffers_[GS][i])
					{
						impl_->constantBuffers_[GS][i] = gsBuffer;
						shaderParameterSources_[i] = (const void*)0xffffffff;
						gsBuffersChanged = true;
					}
				}
			}

			if(vsBuffersChanged)
				impl_->deviceContext_->VSSetConstantBuffers(0,MAX_SHADER_PARAMETER_GROUPS,&impl_->constantBuffers_[VS][0]);
			if(psBuffersChanged)
				impl_->deviceContext_->PSSetConstantBuffers(0,MAX_SHADER_PARAMETER_GROUPS,&impl_->constantBuffers_[PS][0]);
			if(gsBuffersChanged)
				impl_->deviceContext_->GSSetConstantBuffers(0,MAX_SHADER_PARAMETER_GROUPS,&impl_->constantBuffers_[GS][0]);
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

	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const DirectX::XMMATRIX& matrix)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		ShaderParameter ss = i->second;
		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(DirectX::XMMATRIX),&matrix);
	}
	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& vector)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(DirectX::XMFLOAT3),&vector);
	}

	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& vector)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(DirectX::XMFLOAT4),&vector);
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
		buffer->SetVector3ArrayParameter(i->second.offset_,3,&matrix);
	}

	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const Matrix3x4& matrix)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);
		buffer->SetParameter(i->second.offset_,sizeof(Matrix3x4),&matrix);
	}
	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const Vector4Vector::type& vectorArray)
	{
		YumeMap<YumeHash,ShaderParameter>::iterator i;
		if(!shaderProgram_ || (i = shaderProgram_->parameters_.find(param)) == shaderProgram_->parameters_.end())
			return;

		YumeConstantBuffer* buffer = i->second.bufferPtr_;
		if(!buffer->IsDirty())
			dirtyConstantBuffers_.push_back(buffer);

		SetShaderParameter(param,vectorArray[0].Data(),4);
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

	void YumeD3D11Renderer::SetShaderParameter(YumeHash param,const Variant& value)
	{
		switch(value.GetType())
		{
		case VAR_BOOL:
			SetShaderParameter(param,value.GetBool());
			break;

		case VAR_FLOAT:
			SetShaderParameter(param,value.GetFloat());
			break;

		case VAR_VECTOR2:
			SetShaderParameter(param,value.GetVector2());
			break;

		case VAR_VECTOR3:
			SetShaderParameter(param,value.GetVector3());
			break;

		case VAR_VECTOR4:
			SetShaderParameter(param,value.GetVector4());
			break;

		case VAR_COLOR:
			SetShaderParameter(param,value.GetColor());
			break;

		case VAR_MATRIX3:
			SetShaderParameter(param,value.GetMatrix3());
			break;

		case VAR_MATRIX3X4:
			SetShaderParameter(param,value.GetMatrix3x4());
			break;

		case VAR_MATRIX4:
			SetShaderParameter(param,value.GetMatrix4());
			break;
		case VAR_VARIANTVECTOR:
		{
			const VariantVector::type vector = value.GetVariantVector();
			int size = vector.size();
			SetShaderParameter(param,vector);
		}
		break;
		case VAR_VECTOR4VECTOR:
		{
			const Vector4Vector::type& vector = value.GetVector4Array();
			SetShaderParameter(param,vector);
		}
		break;
		case VAR_BUFFER:
		{
			const YumeVector<unsigned char>::type& buffer = value.GetBuffer();
			if(buffer.size() >= sizeof(float))
				SetShaderParameter(param,reinterpret_cast<const float*>(&buffer[0]),buffer.size() / sizeof(float));
		}
		break;
		default:
			// Unsupported parameter type, do nothing
			break;
		}
	}

	void YumeD3D11Renderer::CreateStandardSampler()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd,sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = impl_->device_->CreateSamplerState(&sd,&shadowFilter_);

		shadowFilter_->SetPrivateData(WKPDID_D3DDebugObjectName,12,"ShadowFilter");

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.BorderColor[0] = sd.BorderColor[1] = sd.BorderColor[2] = sd.BorderColor[3] = 0.f;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.MaxAnisotropy = 1;

		hr = impl_->device_->CreateSamplerState(&sd,&lpvFilter_);
		lpvFilter_->SetPrivateData(WKPDID_D3DDebugObjectName,9,"LPVFilter");

		ZeroMemory(&sd,sizeof(sd));
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.MaxAnisotropy = 1;
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MaxLOD = D3D11_FLOAT32_MAX;

		hr = impl_->device_->CreateSamplerState(&sd,&vplFilter_);
		vplFilter_->SetPrivateData(WKPDID_D3DDebugObjectName,9,"VPLFilter");


		ZeroMemory(&sd,sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MaxLOD = D3D11_FLOAT32_MAX;

		hr = impl_->device_->CreateSamplerState(&sd,&standardFilter_);
		standardFilter_->SetPrivateData(WKPDID_D3DDebugObjectName,14,"StandardFilter");

		if(FAILED(hr))
		{
			YUMELOG_ERROR("Could not create standard filter");
		}



		if(FAILED(hr))
		{
			YUMELOG_ERROR("Could not create LPV filter");
		}

		D3D11_BLEND_DESC bld;
		ZeroMemory(&bld,sizeof(D3D11_BLEND_DESC));

		bld.IndependentBlendEnable = TRUE;

		for(size_t i = 0; i < 6; ++i)
		{
			bld.RenderTarget[i].BlendEnable =    TRUE;
			bld.RenderTarget[i].SrcBlend =       D3D11_BLEND_ONE;
			bld.RenderTarget[i].DestBlend =      D3D11_BLEND_ONE;
			bld.RenderTarget[i].SrcBlendAlpha =  D3D11_BLEND_ONE;
			bld.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			bld.RenderTarget[i].BlendOp =        D3D11_BLEND_OP_ADD;
			bld.RenderTarget[i].BlendOpAlpha =   D3D11_BLEND_OP_ADD;
			bld.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		hr = impl_->device_->CreateBlendState(&bld,&bsInject_);

		if(FAILED(hr))
		{
			YUMELOG_ERROR("Could not create blend state of injection");
		}

		for(size_t i = 0; i < 3; ++i)
			bld.RenderTarget[i].BlendEnable = FALSE;

		hr = impl_->device_->CreateBlendState(&bld,&bsPropogate_);

		impl_->samplers_[0] = standardFilter_;
		impl_->samplers_[1] = vplFilter_;
		impl_->samplers_[2] = shadowFilter_;
		impl_->samplers_[3] = lpvFilter_;


		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsd.DepthFunc = D3D11_COMPARISON_LESS;
		dsd.StencilEnable = true;
		dsd.StencilReadMask = 0xFF;
		dsd.StencilWriteMask = 0xFF;
		dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsd.DepthEnable = FALSE;
		impl_->device_->CreateDepthStencilState(&dsd,&dss_disableDepthTest_);

		dsd.DepthEnable = TRUE;
		impl_->device_->CreateDepthStencilState(&dsd,&dss_enableDepthTest_);
	}

	void YumeD3D11Renderer::GenerateMips(YumeTexture2D* texture)
	{
		impl_->deviceContext_->GenerateMips((ID3D11ShaderResourceView*)texture->GetShaderResourceView());
	}

	void YumeD3D11Renderer::BindDefaultDepthStencil()
	{
		depthStencil_ = 0;
		impl_->depthStencilView_ = impl_->defaultDepthStencilView_;
		rasterizerStateDirty_ = true;
	}

	void YumeD3D11Renderer::BindDepthStateEnable()
	{
		impl_->deviceContext_->OMSetDepthStencilState(dss_enableDepthTest_,0);
	}

	void YumeD3D11Renderer::BindDepthStateDisable()
	{
		impl_->deviceContext_->OMSetDepthStencilState(dss_disableDepthTest_,0);
	}

	void YumeD3D11Renderer::BindPsuedoBuffer()
	{
		unsigned int stride = 0;
		unsigned int offset = 0;
		ID3D11Buffer* buffer[] ={nullptr};

		impl_->deviceContext_->IASetVertexBuffers(0,1,buffer,&stride,&offset);
		impl_->deviceContext_->IASetInputLayout(nullptr);

		vertexDeclarationDirty_ = false;
		vertexDeclarationHash_ = M_MAX_UNSIGNED;

		for(int i=0; i < 4; ++i)
			vertexBuffers_[i] = 0;
	}
	void YumeD3D11Renderer::BindNullIndexBuffer()
	{
		impl_->deviceContext_->IASetIndexBuffer(nullptr,DXGI_FORMAT_R32_UINT,0);
		indexBuffer_ = 0;
	}

	void YumeD3D11Renderer::BindLPVSampler()
	{
		impl_->samplers_[1] = lpvFilter_;
		texturesDirty_ = true;
		firstDirtyTexture_ = -1;
		lastDirtyTexture_ = -1;
	}

	void YumeD3D11Renderer::BindShadowsSampler()
	{
		impl_->samplers_[2] = shadowFilter_;
		texturesDirty_ = true;
		firstDirtyTexture_ = -1;
		lastDirtyTexture_ = -1;
	}

	void YumeD3D11Renderer::BindStandardSampler()
	{
		impl_->samplers_[0] = standardFilter_;
		impl_->deviceContext_->VSSetSamplers(0,1,
			&impl_->samplers_[0]);
	}

	void YumeD3D11Renderer::BindSampler(ShaderType type,unsigned samplerStartSlot,unsigned samplerCount,unsigned internalIndex)
	{
		if(type == VS)
			impl_->deviceContext_->VSSetSamplers(samplerStartSlot,samplerCount,
			&impl_->samplers_[internalIndex]);
		else if(type == PS)
			impl_->deviceContext_->PSSetSamplers(samplerStartSlot,samplerCount,
			&impl_->samplers_[internalIndex]);
	}

	void YumeD3D11Renderer::PSBindSRV(unsigned start,unsigned count,YumeTexture2D** textures)
	{
		std::vector<ID3D11ShaderResourceView*> srvs;

		for(int i=0; i < count; ++i)
		{
			if(*textures)
				srvs.push_back((ID3D11ShaderResourceView*)(*textures)->GetShaderResourceView());
			else
				srvs.push_back(nullptr);
			textures++;
		}
		impl_->deviceContext_->PSSetShaderResources(start,count,&srvs[0]);
	}

	void YumeD3D11Renderer::PSBindSRV(unsigned start,unsigned count,YumeTexture3D** textures)
	{
		std::vector<ID3D11ShaderResourceView*> srvs;

		for(int i=0; i < count; ++i)
		{
			if(*textures)
				srvs.push_back((ID3D11ShaderResourceView*)(*textures)->GetShaderResourceView());
			else
				srvs.push_back(nullptr);
			textures++;
		}
		impl_->deviceContext_->PSSetShaderResources(start,count,&srvs[0]);
	}

	void YumeD3D11Renderer::VSBindSRV(unsigned start,unsigned count,YumeTexture2D** textures)
	{
		std::vector<ID3D11ShaderResourceView*> srvs;

		for(int i=0; i < count; ++i)
		{
			srvs.push_back((ID3D11ShaderResourceView*)(*textures)->GetShaderResourceView());
			textures++;
		}
		impl_->deviceContext_->VSSetShaderResources(start,count,&srvs[0]);
	}


	void YumeD3D11Renderer::BindResetTextures(int start,int count,bool ps)
	{
		std::vector<ID3D11ShaderResourceView*> null_srv;

		for(int i=0; i < count; ++i)
			null_srv.push_back(nullptr);

		if(ps)
		{
			impl_->deviceContext_->PSSetShaderResources(start,count,&null_srv[0]);
			return;
		}
		else
			impl_->deviceContext_->PSSetShaderResources(start,count,&null_srv[0]);

		impl_->deviceContext_->VSSetShaderResources(start,count,&null_srv[0]);

		for(int i=start; i < start + count; ++i)
		{
			textures_[i] = 0;
			impl_->shaderResourceViews_[i] = 0;
		}
	}

	void YumeD3D11Renderer::BindBackbuffer()
	{
		
		impl_->renderTargetViews_[0] = impl_->defaultRenderTargetView_;
		impl_->deviceContext_->OMSetRenderTargets(1,&impl_->defaultRenderTargetView_,nullptr);
	}

	void YumeD3D11Renderer::BindResetRenderTargets(int count)
	{

		if(count == 0)
		{
			for(int i=0; i < MAX_RENDERTARGETS ; ++i)
			{
				renderTargets_[i] = 0;
				impl_->renderTargetViews_[i] = 0;
			}
			impl_->deviceContext_->OMSetRenderTargets(0,nullptr,nullptr);
			return;
		}

		std::vector<ID3D11RenderTargetView*> null_views;

		for(int i=0; i < count; ++i)
			null_views.push_back(nullptr);

		impl_->deviceContext_->OMSetRenderTargets(count,&null_views[0],nullptr);

		for(int i=0; i < count ; ++i)
		{
			renderTargets_[i] = 0;
			impl_->renderTargetViews_[i] = 0;
		}
	}

	void YumeD3D11Renderer::BindInjectBlendState()
	{
		FLOAT factors[4] ={1.0f,1.0f,1.0f,1.0f};
		impl_->deviceContext_->OMSetBlendState(bsInject_,factors,0xffffffff);

	}

	void YumeD3D11Renderer::BindPropogateBlendState()
	{
		FLOAT factors[4] ={1.0f,1.0f,1.0f,1.0f};
		impl_->deviceContext_->OMSetBlendState(bsPropogate_,factors,0xffffffff);
	}

	void YumeD3D11Renderer::BindNullBlendState()
	{
		impl_->deviceContext_->OMSetBlendState(nullptr,nullptr,0xffffffff);
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
			if(firstDirtyTexture_ == M_MAX_UNSIGNED)
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
			/*impl_->samplers_[index] = texture ? (ID3D11SamplerState*)texture->GetSampler() : 0;*/
			texturesDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetRenderTarget(unsigned index,YumeRenderable* renderTarget)
	{
		if(index >= MAX_RENDERTARGETS)
			return;

		if(renderTarget != renderTargets_[index])
		{
			renderTargets_[index] = renderTarget;
			renderTargetsDirty_ = true;
		}
	}
	void YumeD3D11Renderer::SetDepthStencil(YumeRenderable* depthStencil)
	{
		depthStencil_ = depthStencil;
		renderTargetsDirty_ = true;

	/*	if(depthStencil_)
			impl_->depthStencilView_ = (ID3D11DepthStencilView*)depthStencil_->GetRenderTargetView();
		else
			impl_->depthStencilView_ = 0;*/
	}
	void YumeD3D11Renderer::SetDepthStencil(YumeTexture2D* texture)
	{
		YumeRenderable* depthStencil = 0;
		if(texture)
			depthStencil = texture->GetRenderSurface();

		SetDepthStencil(depthStencil);
		// Constant depth bias depends on the bitdepth
		rasterizerStateDirty_ = true;
	}

	void YumeD3D11Renderer::SetBlendMode(BlendMode mode)
	{
		if(mode != blendMode_)
		{
			blendMode_ = mode;
			blendStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetColorWrite(bool enable)
	{
		if(enable != colorWrite_)
		{
			colorWrite_ = enable;
			blendStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetCullMode(CullMode mode)
	{
		if(mode != cullMode_)
		{
			cullMode_ = mode;
			rasterizerStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetDepthBias(float constantBias,float slopeScaledBias)
	{
		if(constantBias != constantDepthBias_ || slopeScaledBias != slopeScaledDepthBias_)
		{
			constantDepthBias_ = constantBias;
			slopeScaledDepthBias_ = slopeScaledBias;
			rasterizerStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetDepthTest(CompareMode mode)
	{
		if(mode != depthTestMode_)
		{
			depthTestMode_ = mode;
			depthStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetDepthWrite(bool enable)
	{
		if(enable != depthWrite_)
		{
			depthWrite_ = enable;
			depthStateDirty_ = true;
			// Also affects whether a read-only version of depth-stencil should be bound, to allow sampling
			renderTargetsDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetFillMode(FillMode mode)
	{
		if(mode != fillMode_)
		{
			fillMode_ = mode;
			rasterizerStateDirty_ = true;
		}
	}


	void YumeD3D11Renderer::SetScissorTest(bool enable,const Rect& rect,bool borderInclusive)
	{
		// During some light rendering loops, a full rect is toggled on/off repeatedly.
		// Disable scissor in that case to reduce state changes
		if(rect.min_.x_ <= 0.0f && rect.min_.y_ <= 0.0f && rect.max_.x_ >= 1.0f && rect.max_.y_ >= 1.0f)
			enable = false;

		if(enable)
		{
			IntVector2 rtSize(GetRenderTargetDimensions());
			IntVector2 viewSize(viewport_.Size());
			IntVector2 viewPos(viewport_.left_,viewport_.top_);
			IntRect intRect;
			int expand = borderInclusive ? 1 : 0;

			intRect.left_ = Clamp((int)((rect.min_.x_ + 1.0f) * 0.5f * viewSize.x_) + viewPos.x_,0,rtSize.x_ - 1);
			intRect.top_ = Clamp((int)((-rect.max_.y_ + 1.0f) * 0.5f * viewSize.y_) + viewPos.y_,0,rtSize.y_ - 1);
			intRect.right_ = Clamp((int)((rect.max_.x_ + 1.0f) * 0.5f * viewSize.x_) + viewPos.x_ + expand,0,rtSize.x_);
			intRect.bottom_ = Clamp((int)((-rect.min_.y_ + 1.0f) * 0.5f * viewSize.y_) + viewPos.y_ + expand,0,rtSize.y_);

			if(intRect.right_ == intRect.left_)
				intRect.right_++;
			if(intRect.bottom_ == intRect.top_)
				intRect.bottom_++;

			if(intRect.right_ < intRect.left_ || intRect.bottom_ < intRect.top_)
				enable = false;

			if(enable && intRect != scissorRect_)
			{
				scissorRect_ = intRect;
				scissorRectDirty_ = true;
			}
		}

		if(enable != scissorTest_)
		{
			scissorTest_ = enable;
			rasterizerStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetScissorTest(bool enable,const IntRect& rect)
	{
		IntVector2 rtSize(GetRenderTargetDimensions());
		IntVector2 viewPos(viewport_.left_,viewport_.top_);

		if(enable)
		{
			IntRect intRect;
			intRect.left_ = Clamp(rect.left_ + viewPos.x_,0,rtSize.x_ - 1);
			intRect.top_ = Clamp(rect.top_ + viewPos.y_,0,rtSize.y_ - 1);
			intRect.right_ = Clamp(rect.right_ + viewPos.x_,0,rtSize.x_);
			intRect.bottom_ = Clamp(rect.bottom_ + viewPos.y_,0,rtSize.y_);

			if(intRect.right_ == intRect.left_)
				intRect.right_++;
			if(intRect.bottom_ == intRect.top_)
				intRect.bottom_++;

			if(intRect.right_ < intRect.left_ || intRect.bottom_ < intRect.top_)
				enable = false;

			if(enable && intRect != scissorRect_)
			{
				scissorRect_ = intRect;
				scissorRectDirty_ = true;
			}
		}

		if(enable != scissorTest_)
		{
			scissorTest_ = enable;
			rasterizerStateDirty_ = true;
		}
	}

	void YumeD3D11Renderer::SetStencilTest(bool enable,CompareMode mode,StencilOp pass,StencilOp fail,StencilOp zFail,StencilOp zFailBack,unsigned stencilRef,
		unsigned compareMask,unsigned writeMask)
	{
		if(enable != stencilTest_)
		{
			stencilTest_ = enable;
			depthStateDirty_ = true;
		}

		if(enable)
		{
			if(mode != stencilTestMode_)
			{
				stencilTestMode_ = mode;
				depthStateDirty_ = true;
			}
			if(pass != stencilPass_)
			{
				stencilPass_ = pass;
				depthStateDirty_ = true;
			}
			if(fail != stencilFail_)
			{
				stencilFail_ = fail;
				depthStateDirty_ = true;
			}
			if(zFail != stencilZFail_)
			{
				stencilZFail_ = zFail;
				depthStateDirty_ = true;
			}
			if(zFailBack != stencilZFailBack_)
			{
				stencilZFailBack_= zFailBack;
				depthStateDirty_ = true;
			}
			if(compareMask != stencilCompareMask_)
			{
				stencilCompareMask_ = compareMask;
				depthStateDirty_ = true;
			}
			if(writeMask != stencilWriteMask_)
			{
				stencilWriteMask_ = writeMask;
				depthStateDirty_ = true;
			}
			if(stencilRef != stencilRef_)
			{
				stencilRef_ = stencilRef;
				stencilRefDirty_ = true;
				depthStateDirty_ = true;
			}
		}
	}

	void YumeD3D11Renderer::SetClipPlane(bool enable,const Plane& clipPlane,const Matrix3x4& view,const Matrix4& projection)
	{
		useClipPlane_ = enable;

		if(enable)
		{
			Matrix4 viewProj = projection * view;
			clipPlane_ = clipPlane.Transformed(viewProj).ToVector4();
			SetShaderParameter(VSP_CLIPPLANE,clipPlane_);
		}
	}

	void YumeD3D11Renderer::ClearParameterSource(ShaderParameterGroup group)
	{
		shaderParameterSources_[group] = (const void*)M_MAX_UNSIGNED;
	}

	void YumeD3D11Renderer::ClearParameterSources()
	{
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			shaderParameterSources_[i] = (const void*)M_MAX_UNSIGNED;
	}

	void YumeD3D11Renderer::ClearTransformSources()
	{
		shaderParameterSources_[SP_CAMERA] = (const void*)M_MAX_UNSIGNED;
		shaderParameterSources_[SP_OBJECT] = (const void*)M_MAX_UNSIGNED;
	}

	YumeVertexBuffer* YumeD3D11Renderer::CreateVertexBuffer()
	{
		return YumeAPINew YumeD3D11VertexBuffer();
	}

	YumeIndexBuffer* YumeD3D11Renderer::CreateIndexBuffer()
	{
		return YumeAPINew YumeD3D11IndexBuffer();
	}

	YumeTexture2D* YumeD3D11Renderer::CreateTexture2D()
	{
		return YumeAPINew YumeD3D11Texture2D();
	}
	YumeTexture3D* YumeD3D11Renderer::CreateTexture3D()
	{
		return YumeAPINew YumeD3D11Texture3D();
	}
	YumeTextureCube* YumeD3D11Renderer::CreateTextureCube()
	{
		return YumeAPINew YumeD3D11TextureCube();
	}

	YumeInputLayout* YumeD3D11Renderer::CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks)
	{
		return YumeAPINew YumeD3D11InputLayout(vertexShader,buffers,elementMasks);
	}

	void YumeD3D11Renderer::CleanupShaderPrograms(YumeShaderVariation* variation)
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
			return i->second;
		else
		{
			SharedPtr<YumeConstantBuffer> newConstantBuffer(new YumeD3D11ConstantBuffer());
			newConstantBuffer->SetSize(size);
			constantBuffers_[key] = newConstantBuffer;
			return newConstantBuffer;
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

				if(firstDirtyVB_ == M_MAX_UNSIGNED)
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

			buffer = i < buffers.size() ? static_cast<YumeD3D11VertexBuffer*>(buffers[i].Get()) : 0;
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

				if(firstDirtyVB_ == M_MAX_UNSIGNED)
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

	bool YumeD3D11Renderer::NeedParameterUpdate(ShaderParameterGroup group,const void* source)
	{
		if((unsigned)(size_t)shaderParameterSources_[group] == M_MAX_UNSIGNED || shaderParameterSources_[group] != source)
		{
			shaderParameterSources_[group] = source;
			return true;
		}
		else
			return false;
	}

	void YumeD3D11Renderer::PreDraw()
	{
		if(renderTargetsDirty_)
		{
			if(!noDs_)
				impl_->depthStencilView_ =
					depthStencil_ ? (ID3D11DepthStencilView*)depthStencil_->GetRenderTargetView() : impl_->defaultDepthStencilView_;
			else
				impl_->depthStencilView_ = 0;

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

		if(vertexDeclarationDirty_ && vertexShader_ && vertexShader_->GetByteCode().size())
		{
			if(firstDirtyVB_ < M_MAX_UNSIGNED)
			{
				impl_->deviceContext_->IASetVertexBuffers(firstDirtyVB_,lastDirtyVB_ - firstDirtyVB_ + 1,
					&impl_->vertexBuffers_[firstDirtyVB_],&impl_->vertexSizes_[firstDirtyVB_],&impl_->vertexOffsets_[firstDirtyVB_]);

				firstDirtyVB_ = lastDirtyVB_ = M_MAX_UNSIGNED;
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
					if(i == vertexDeclarations_.end())
					{
						SharedPtr<YumeInputLayout> newVertexDeclaration(new YumeD3D11InputLayout(vertexShader_,vertexBuffers_,
							elementMasks_));
						i= vertexDeclarations_.insert(MakePair(newVertexDeclarationHash,newVertexDeclaration));
					}

					impl_->deviceContext_->IASetInputLayout((ID3D11InputLayout*)i->second->GetInputLayout());
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
						YUMELOG_ERROR("Failed to create blend state " << hr);
					}


					i = impl_->blendStates_.insert(MakePair(newBlendStateHash,newBlendState));
				}
				FLOAT factors[4] ={1.0f,1.0f,1.0f,1.0f};
				impl_->deviceContext_->OMSetBlendState(i->second,factors,M_MAX_UNSIGNED);
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
					stateDesc.BackFace.StencilDepthFailOp = d3dStencilOp[stencilZFailBack_];
					stateDesc.BackFace.StencilPassOp = d3dStencilOp[stencilPass_];
					stateDesc.BackFace.StencilFunc = d3dCmpFunc[stencilTestMode_];

					ID3D11DepthStencilState* newDepthState = 0;
					HRESULT hr = impl_->device_->CreateDepthStencilState(&stateDesc,&newDepthState);
					if(FAILED(hr))
					{
						D3D_SAFE_RELEASE(newDepthState);
						YUMELOG_ERROR("Failed to create depth state",hr);
					}

					i = impl_->depthStates_.insert(MakePair(newDepthStateHash,newDepthState));
				}
				impl_->deviceContext_->OMSetDepthStencilState(i->second,stencilRef_);
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
				if(i == impl_->rasterizerStates_.end())
				{
					D3D11_RASTERIZER_DESC stateDesc;
					memset(&stateDesc,0,sizeof stateDesc);
					stateDesc.FillMode = d3dFillMode[fillMode_];
					stateDesc.CullMode = d3dCullMode[cullMode_];
					stateDesc.FrontCounterClockwise = FALSE;
					stateDesc.DepthBias = scaledDepthBias;
					stateDesc.DepthBiasClamp = 0;
					stateDesc.SlopeScaledDepthBias = 0;
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

					i = impl_->rasterizerStates_.insert(MakePair(newRasterizerStateHash,newRasterizerState));
				}

				impl_->deviceContext_->RSSetState(i->second);
				rasterizerStateHash_ = newRasterizerStateHash;
			}

			rasterizerStateDirty_ = false;
		}

		if(scissorRectDirty_)
		{
			/*D3D11_RECT d3dRect;
			d3dRect.left = scissorRect_.left_;
			d3dRect.top = scissorRect_.top_;
			d3dRect.right = scissorRect_.right_;
			d3dRect.bottom = scissorRect_.bottom_;
			impl_->deviceContext_->RSSetScissorRects(1,&d3dRect);*/
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

	void YumeD3D11Renderer::BeginEvent(const YumeString& str)
	{
		WString event(str);
		D3DPERF_BeginEvent(0xFFFFFFFF,event.c_str());
	}
	void YumeD3D11Renderer::EndEvent()
	{
		D3DPERF_EndEvent();
	}

	bool YumeD3D11Renderer::ResolveToTexture(YumeTexture2D* destination,const IntRect& viewport)
	{
		if(!destination || !destination->GetRenderSurface())
			return false;

		IntRect vpCopy = viewport;
		if(vpCopy.right_ <= vpCopy.left_)
			vpCopy.right_ = vpCopy.left_ + 1;
		if(vpCopy.bottom_ <= vpCopy.top_)
			vpCopy.bottom_ = vpCopy.top_ + 1;

		D3D11_BOX srcBox;
		srcBox.left = Clamp(vpCopy.left_,0,windowWidth_);
		srcBox.top = Clamp(vpCopy.top_,0,windowHeight_);
		srcBox.right = Clamp(vpCopy.right_,0,windowWidth_);
		srcBox.bottom = Clamp(vpCopy.bottom_,0,windowHeight_);
		srcBox.front = 0;
		srcBox.back = 1;

		ID3D11Resource* source = 0;
		bool resolve = multiSample_ > 1;
		impl_->defaultRenderTargetView_->GetResource(&source);

		if(!resolve)
		{
			if(!srcBox.left && !srcBox.top && srcBox.right == windowWidth_ && srcBox.bottom == windowHeight_)
				impl_->deviceContext_->CopyResource((ID3D11Resource*)static_cast<YumeD3D11Texture2D*>(destination)->GetGPUObject(),source);
			else
				impl_->deviceContext_->CopySubresourceRegion((ID3D11Resource*)static_cast<YumeD3D11Texture2D*>(destination)->GetGPUObject(),0,0,0,0,source,0,&srcBox);
		}
		else
		{
			if(!srcBox.left && !srcBox.top && srcBox.right == windowWidth_ && srcBox.bottom == windowHeight_)
			{
				impl_->deviceContext_->ResolveSubresource((ID3D11Resource*)static_cast<YumeD3D11Texture2D*>(destination)->GetGPUObject(),0,source,0,(DXGI_FORMAT)
					destination->GetFormat());
			}
			else
			{
				CreateResolveTexture();

				if(impl_->resolveTexture_)
				{
					impl_->deviceContext_->ResolveSubresource(impl_->resolveTexture_,0,source,0,DXGI_FORMAT_R8G8B8A8_UNORM);
					impl_->deviceContext_->CopySubresourceRegion((ID3D11Resource*)static_cast<YumeD3D11Texture2D*>(destination)->GetGPUObject(),0,0,0,0,impl_->resolveTexture_,0,&srcBox);
				}
			}
		}

		source->Release();

		return true;
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
				if(resolutions[j].x_ == width && resolutions[j].y_ == height)
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
		MutexLock lock(gpuResourceMutex_);

		gpuResources_.push_back(gpuRes);
	}

	void YumeD3D11Renderer::RemoveGpuResource(YumeGpuResource* gpuRes)
	{
		MutexLock lock(gpuResourceMutex_);

		//Check if valid
		GpuResourceVector::iterator It = gpuResources_.find(gpuRes);

		if(It != gpuResources_.end())
			gpuResources_.erase(It);
	}

	void YumeD3D11Renderer::RegisterFactories()
	{
		gYume->pObjFactory->RegisterFactoryFunction(("D3D11Shader"),[](void) -> YumeBase * { return new YumeD3D11Shader();});
		gYume->pObjFactory->RegisterFactoryFunction(("Texture2D"),[this](void) -> YumeBase * { return new YumeD3D11Texture2D();});
		gYume->pObjFactory->RegisterFactoryFunction(("Texture3D"),[this](void) -> YumeBase * { return new YumeD3D11Texture3D();});
		gYume->pObjFactory->RegisterFactoryFunction(("TextureCube"),[this](void) -> YumeBase * { return new YumeD3D11TextureCube();});
		gYume->pObjFactory->RegisterFactoryFunction(("IndexBuffer"),[this](void) -> YumeBase * { return new YumeD3D11IndexBuffer();});
		gYume->pObjFactory->RegisterFactoryFunction(("VertexBuffer"),[this](void) -> YumeBase * { return new YumeD3D11VertexBuffer();});
	}
	void YumeD3D11Renderer::UnregisterFactories()
	{
		gYume->pObjFactory->UnRegisterFactoryFunction(("D3D11Shader"));
		gYume->pObjFactory->UnRegisterFactoryFunction(("Texture2D"));
		gYume->pObjFactory->UnRegisterFactoryFunction(("Texture3D"));
		gYume->pObjFactory->UnRegisterFactoryFunction(("TextureCube"));
		gYume->pObjFactory->UnRegisterFactoryFunction(("IndexBuffer"));
		gYume->pObjFactory->UnRegisterFactoryFunction(("VertexBuffer"));
	}

	void YumeD3D11Renderer::CreateResolveTexture()
	{
		if(impl_->resolveTexture_)
			return;

		D3D11_TEXTURE2D_DESC textureDesc;
		memset(&textureDesc,0,sizeof textureDesc);
		textureDesc.Width = (UINT)windowWidth_;
		textureDesc.Height = (UINT)windowHeight_;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.CPUAccessFlags = 0;

		HRESULT hr = impl_->device_->CreateTexture2D(&textureDesc,0,&impl_->resolveTexture_);
		if(FAILED(hr))
		{
			D3D_SAFE_RELEASE(impl_->resolveTexture_);
			YUMELOG_ERROR("Could not create resolve texture",hr);
		}
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


	unsigned YumeD3D11Renderer::GetAlphaFormatNs()
	{
		return DXGI_FORMAT_A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetLuminanceFormatNs()
	{
		// Note: not same sampling behavior as on D3D9; need to sample the R channel only
		return DXGI_FORMAT_R8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetLuminanceAlphaFormatNs()
	{
		// Note: not same sampling behavior as on D3D9; need to sample the RG channels
		return DXGI_FORMAT_R8G8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBFormatNs()
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBAFormatNs()
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetBGRAFormatNs()
	{
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBA16FormatNs()
	{
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGBAFloat16FormatNs()
	{
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRGBAFloat32FormatNs()
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRG16FormatNs()
	{
		return DXGI_FORMAT_R16G16_UNORM;
	}

	unsigned YumeD3D11Renderer::GetRGFloat16FormatNs()
	{
		return DXGI_FORMAT_R16G16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetRGFloat32FormatNs()
	{
		return DXGI_FORMAT_R32G32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetFloat16FormatNs()
	{
		return DXGI_FORMAT_R16_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetFloat32FormatNs()
	{
		return DXGI_FORMAT_R32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetLinearDepthFormatNs()
	{
		return DXGI_FORMAT_R32_FLOAT;
	}

	unsigned YumeD3D11Renderer::GetDepthStencilFormatNs()
	{
		return DXGI_FORMAT_R24G8_TYPELESS;
	}

	unsigned YumeD3D11Renderer::GetReadableDepthFormatNs()
	{
		return DXGI_FORMAT_R24G8_TYPELESS;
	}

	unsigned YumeD3D11Renderer::GetFormat(const YumeString& formatName)
	{

		YumeString nameLower = formatName.ToLower().Trimmed();

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
