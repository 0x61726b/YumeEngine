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

#include "YumeGLRenderer.h"
#include "YumeGLRendererImpl.h"
#include "YumeGLGpuResource.h"
#include "YumeGLShader.h"
#include "YumeGLShaderProgram.h"
#include "YumeGLConstantBuffer.h"
#include "YumeGLVertexBuffer.h"
#include "YumeGLIndexBuffer.h"
#include "YumeGLTexture2D.h"
#include "Renderer/YumeRenderable.h"
#include "YumeGLRenderable.h"

#include "Core/YumeDefaults.h"

#include "Math/YumeMath.h"

#include "Logging/logging.h"

#include "Engine/YumeEngine.h"

#include "Math/YumePlane.h"

#include "Renderer/YumeResourceManager.h"

#include <boost/algorithm/string.hpp>

#include <SDL_syswm.h>

namespace YumeEngine
{
	extern "C" void YumeGLExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = new YumeGLRenderer;
		engine->SetRenderer(graphics_);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeGLExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = engine->GetRenderer();
		delete graphics_;
	}
	//---------------------------------------------------------------------


	static const unsigned glCmpFunc[] =
	{
		GL_ALWAYS,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_LESS,
		GL_LEQUAL,
		GL_GREATER,
		GL_GEQUAL
	};

	static const unsigned glSrcBlend[] =
	{
		GL_ONE,
		GL_ONE,
		GL_DST_COLOR,
		GL_SRC_ALPHA,
		GL_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_DST_ALPHA,
		GL_ONE,
		GL_SRC_ALPHA
	};

	static const unsigned glDestBlend[] =
	{
		GL_ZERO,
		GL_ONE,
		GL_ZERO,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE,
		GL_ONE
	};

	static const unsigned glBlendOp[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT
	};

#ifndef GL_ES_VERSION_2_0
	static const unsigned glFillMode[] =
	{
		GL_FILL,
		GL_LINE,
		GL_POINT
	};

	static const unsigned glStencilOps[] =
	{
		GL_KEEP,
		GL_ZERO,
		GL_REPLACE,
		GL_INCR_WRAP,
		GL_DECR_WRAP
	};
#endif

	// Remap vertex attributes on OpenGL so that all usually needed attributes including skinning fit to the first 8.
	// This avoids a skinning bug on GLES2 devices which only support 8.
	static const unsigned glVertexAttrIndex[] =
	{
		0,1,2,3,4,8,9,5,6,7,10,11,12,13
	};

#ifdef GL_ES_VERSION_2_0
	static unsigned glesDepthStencilFormat = GL_DEPTH_COMPONENT16;
	static unsigned glesReadableDepthFormat = GL_DEPTH_COMPONENT;
#endif

	static YumeString extensions;

	bool CheckExtension(const YumeString& name)
	{
		if(extensions.empty())
			extensions = (const char*)glGetString(GL_EXTENSIONS);
		return extensions.find(name) != std::string::npos;
	}

	static void GetGLPrimitiveType(unsigned elementCount,PrimitiveType type,unsigned& primitiveCount,GLenum& glPrimitiveType)
	{
		switch(type)
		{
		case TRIANGLE_LIST:
			primitiveCount = elementCount / 3;
			glPrimitiveType = GL_TRIANGLES;
			break;

		case LINE_LIST:
			primitiveCount = elementCount / 2;
			glPrimitiveType = GL_LINES;
			break;

		case POINT_LIST:
			primitiveCount = elementCount;
			glPrimitiveType = GL_POINTS;
			break;

		case TRIANGLE_STRIP:
			primitiveCount = elementCount - 2;
			glPrimitiveType = GL_TRIANGLE_STRIP;
			break;

		case LINE_STRIP:
			primitiveCount = elementCount - 1;
			glPrimitiveType = GL_LINE_STRIP;
			break;

		case TRIANGLE_FAN:
			primitiveCount = elementCount - 2;
			glPrimitiveType = GL_TRIANGLE_FAN;
			break;
		}
	}

	bool YumeGLRenderer::gl3Support = false;
	bool YumeGLRenderer::GetGL3Support()
	{
		return gl3Support;
	}


	unsigned YumeGLRenderer::GetMaxBones()
	{
		return gl3Support ? 128 : 64;
	}


	YumeGLRenderer::YumeGLRenderer():
		impl_(new YumeGLRendererImpl())
	{
		forceGL2_ = (false);
		instancingSupport_ = (false);
		lightPrepassSupport_ = (false);
		deferredSupport_ = (false);
		anisotropySupport_ = (false);
		dxtTextureSupport_ = (false);
		etcTextureSupport_ = (false);
		pvrtcTextureSupport_ = (false);
		sRGBSupport_ = (false);
		sRGBWriteSupport_ = (false);
		numPrimitives_ = (0);
		numBatches_ = (0);
		dummyColorFormat_ = (0);
		shadowMapFormat_ = (GL_DEPTH_COMPONENT16);
		hiresShadowMapFormat_ = (GL_DEPTH_COMPONENT24);

#ifndef GL_ES_VERSION_2_0
		apiName_ = ("GL2");
#else
		apiName_ = ("GLES2");
#endif
		shaderPath_ = "Shaders/GLSL/";
		shaderExtension_ = ".glsl";

		RegisterFactories();

		SetTextureUnitMappings();
		ResetCache();

		YUMELOG_INFO("Initializing OpenGL graphics....");

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	}

	YumeGLRenderer::~YumeGLRenderer()
	{
		Close();

		delete impl_;
		impl_ = 0;

		UnregisterFactories();
		// Shut down SDL now. Graphics should be the last SDL-using subsystem to be destroyed
		SDL_Quit();

		for(ShaderProgramMap::iterator i=shaderPrograms_.begin(); i != shaderPrograms_.end(); ++i)
		{
			i->second.reset();
		}
		shaderPrograms_.clear();

		lastShader_.reset();
	}

	bool YumeGLRenderer::IsInitialized()
	{
		return window_ != 0;
	}

	bool YumeGLRenderer::IsDeviceLost() const
	{
		return impl_->context_ == 0;
	}

	bool YumeGLRenderer::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		if(fullscreen_ && (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED))
			return false;

		// Re-enable depth test and depth func in case a third party program has modified it
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(glCmpFunc[depthTestMode_]);

		// Set default rendertarget and depth buffer
		ResetRenderTargets();

		// Cleanup textures from previous frame
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			SetTexture(i,0);

		// Enable color and depth write
		SetColorWrite(true);
		SetDepthWrite(true);

		numPrimitives_ = 0;
		numBatches_ = 0;

		return true;
	}

	void YumeGLRenderer::EndFrame()
	{
		if(!IsInitialized())
			return;

		SDL_GL_SwapWindow(window_);

		CleanupScratchBuffers();
	}

	void YumeGLRenderer::Clear(unsigned flags,const YumeColor& color,float depth,unsigned stencil)
	{
		PreDraw();

#ifdef GL_ES_VERSION_2_0
		flags &= ~CLEAR_STENCIL;
#endif

		bool oldColorWrite = colorWrite_;
		bool oldDepthWrite = depthWrite_;

		if(flags & CLEAR_COLOR && !oldColorWrite)
			SetColorWrite(true);
		if(flags & CLEAR_DEPTH && !oldDepthWrite)
			SetDepthWrite(true);
		if(flags & CLEAR_STENCIL && stencilWriteMask_ != M_MAX_UNSIGNED)
			glStencilMask(M_MAX_UNSIGNED);

		unsigned glFlags = 0;
		if(flags & CLEAR_COLOR)
		{
			glFlags |= GL_COLOR_BUFFER_BIT;
			glClearColor(color.r_,color.g_,color.b_,color.a_);
		}
		if(flags & CLEAR_DEPTH)
		{
			glFlags |= GL_DEPTH_BUFFER_BIT;
			glClearDepth(depth);
		}
		if(flags & CLEAR_STENCIL)
		{
			glFlags |= GL_STENCIL_BUFFER_BIT;
			glClearStencil(stencil);
		}

		// If viewport is less than full screen, set a scissor to limit the clear
		/// \todo Any user-set scissor test will be lost
		IntVector2 viewSize = GetRenderTargetDimensions();
		if(viewport_.left_ != 0 || viewport_.top_ != 0 || viewport_.right_ != viewSize.x_ || viewport_.bottom_ != viewSize.y_)
			SetScissorTest(true,IntRect(0,0,viewport_.Width(),viewport_.Height()));
		else
			SetScissorTest(false);

		glClear(glFlags);

		SetScissorTest(false);
		SetColorWrite(oldColorWrite);
		SetDepthWrite(oldDepthWrite);
		if(flags & CLEAR_STENCIL && stencilWriteMask_ != M_MAX_UNSIGNED)
			glStencilMask(stencilWriteMask_);
	}

	void YumeGLRenderer::CreateRendererCapabilities()
	{
		// Check supported features: light pre-pass, deferred rendering and hardware depth texture
		lightPrepassSupport_ = false;
		deferredSupport_ = false;

#ifndef GL_ES_VERSION_2_0
		int numSupportedRTs = 1;
		if(gl3Support)
		{
			// Work around GLEW failure to check extensions properly from a GL3 context
			instancingSupport_ = true;
			dxtTextureSupport_ = true;
			anisotropySupport_ = true;
			sRGBSupport_ = true;
			sRGBWriteSupport_ = true;

			glVertexAttribDivisor(ELEMENT_INSTANCEMATRIX1,1);
			glVertexAttribDivisor(ELEMENT_INSTANCEMATRIX2,1);
			glVertexAttribDivisor(ELEMENT_INSTANCEMATRIX3,1);

			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,&numSupportedRTs);
		}
		else
		{
			instancingSupport_ = GLEW_ARB_instanced_arrays != 0;
			dxtTextureSupport_ = GLEW_EXT_texture_compression_s3tc != 0;
			anisotropySupport_ = GLEW_EXT_texture_filter_anisotropic != 0;
			sRGBSupport_ = GLEW_EXT_texture_sRGB != 0;
			sRGBWriteSupport_ = GLEW_EXT_framebuffer_sRGB != 0;

			// Set up instancing divisors if supported
			if(instancingSupport_)
			{
				glVertexAttribDivisorARB(ELEMENT_INSTANCEMATRIX1,1);
				glVertexAttribDivisorARB(ELEMENT_INSTANCEMATRIX2,1);
				glVertexAttribDivisorARB(ELEMENT_INSTANCEMATRIX3,1);
			}

			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT,&numSupportedRTs);
		}

		// Must support 2 rendertargets for light pre-pass, and 4 for deferred
		if(numSupportedRTs >= 2)
			lightPrepassSupport_ = true;
		if(numSupportedRTs >= 4)
			deferredSupport_ = true;

#else
		dxtTextureSupport_ = CheckExtension("EXT_texture_compression_dxt1");
		etcTextureSupport_ = CheckExtension("OES_compressed_ETC1_RGB8_texture");
		pvrtcTextureSupport_ = CheckExtension("IMG_texture_compression_pvrtc");

		// Check for best supported depth renderbuffer format for GLES2
		if(CheckExtension("GL_OES_depth24"))
			glesDepthStencilFormat = GL_DEPTH_COMPONENT24_OES;
		if(CheckExtension("GL_OES_packed_depth_stencil"))
			glesDepthStencilFormat = GL_DEPTH24_STENCIL8_OES;
		if(!CheckExtension("GL_OES_depth_texture"))
		{
			shadowMapFormat_ = 0;
			hiresShadowMapFormat_ = 0;
			glesReadableDepthFormat = 0;
		}
		else
		{
			shadowMapFormat_ = GL_DEPTH_COMPONENT;
			hiresShadowMapFormat_ = 0;
		}
#endif
	}


	YumeVertexBuffer* YumeGLRenderer::CreateVertexBuffer()
	{
		return YumeAPINew YumeGLVertexBuffer(this);
	}

	YumeIndexBuffer* YumeGLRenderer::CreateIndexBuffer()
	{
		return YumeAPINew YumeGLIndexBuffer(this);
	}

	void YumeGLRenderer::ResetCache()
	{
		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			vertexBuffers_[i] = 0;
			elementMasks_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			textures_[i] = 0;
			textureTypes_[i] = 0;
		}

		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			renderTargets_[i] = 0;

		depthStencil_ = 0;
		viewport_ = IntRect(0,0,0,0);
		indexBuffer_ = 0;
		vertexShader_ = 0;
		pixelShader_ = 0;
		shaderProgram_ = 0;
		blendMode_ = BLEND_REPLACE;
		textureAnisotropy_ = 1;
		colorWrite_ = true;
		cullMode_ = CULL_NONE;
		constantDepthBias_ = 0.0f;
		slopeScaledDepthBias_ = 0.0f;
		depthTestMode_ = CMP_ALWAYS;
		depthWrite_ = false;
		fillMode_ = FILL_SOLID;
		scissorTest_ = false;
		scissorRect_ = IntRect::ZERO;
		stencilTest_ = false;
		stencilTestMode_ = CMP_ALWAYS;
		stencilPass_ = OP_KEEP;
		stencilFail_ = OP_KEEP;
		stencilZFail_ = OP_KEEP;
		stencilRef_ = 0;
		stencilCompareMask_ = M_MAX_UNSIGNED;
		stencilWriteMask_ = M_MAX_UNSIGNED;
		useClipPlane_ = false;
		lastInstanceOffset_ = 0;
		impl_->activeTexture_ = 0;
		impl_->enabledAttributes_ = 0;
		impl_->boundFBO_ = impl_->systemFBO_;
		impl_->boundVBO_ = 0;
		impl_->boundUBO_ = 0;
		impl_->sRGBWrite_ = false;

		// Set initial state to match Direct3D
		if(impl_->context_)
		{
			glEnable(GL_DEPTH_TEST);
			SetCullMode(CULL_CCW);
			SetDepthTest(CMP_LESSEQUAL);
			SetDepthWrite(true);
		}

		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS * 2; ++i)
			currentConstantBuffers_[i] = 0;
		dirtyConstantBuffers_.clear();
	}

	void YumeGLRenderer::ResetRenderTargets()
	{
		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			SetRenderTarget(i,(YumeRenderable*)0);
		SetDepthStencil((YumeRenderable*)0);
		SetViewport(IntRect(0,0,windowWidth_,windowHeight_));
	}

	void YumeGLRenderer::SetViewport(const IntRect& rect)
	{
		PreDraw();

		IntVector2 rtSize = GetRenderTargetDimensions();

		IntRect rectCopy = rect;

		if(rectCopy.right_ <= rectCopy.left_)
			rectCopy.right_ = rectCopy.left_ + 1;
		if(rectCopy.bottom_ <= rectCopy.top_)
			rectCopy.bottom_ = rectCopy.top_ + 1;
		rectCopy.left_ = Clamp(rectCopy.left_,0,rtSize.x_);
		rectCopy.top_ = Clamp(rectCopy.top_,0,rtSize.y_);
		rectCopy.right_ = Clamp(rectCopy.right_,0,rtSize.x_);
		rectCopy.bottom_ = Clamp(rectCopy.bottom_,0,rtSize.y_);

		// Use Direct3D convention with the vertical coordinates ie. 0 is top
		glViewport(rectCopy.left_,rtSize.y_ - rectCopy.bottom_,rectCopy.Width(),rectCopy.Height());
		viewport_ = rectCopy;

		// Disable scissor test, needs to be re-enabled by the user
		SetScissorTest(false);
	}

	IntVector2 YumeGLRenderer::GetRenderTargetDimensions() const
	{
		int width,height;

		if(renderTargets_[0])
		{
			width = renderTargets_[0]->GetWidth();
			height = renderTargets_[0]->GetHeight();
		}
		else if(depthStencil_)
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


	YumeVector<int>::type YumeGLRenderer::GetMultiSampleLevels() const
	{
		YumeVector<int>::type levelsVector;
		levelsVector.push_back(1);



		return levelsVector;
	}


	void YumeGLRenderer::SetFlushGPU(bool flushGpu_)
	{
		flushGpu_ = flushGpu_;

	}

	void YumeGLRenderer::SetWindowPos(const Vector2& pos)
	{
		if(window_)
			SDL_SetWindowPosition(window_,pos.x_,pos.y_);
		else
			windowPos_ = IntVector2(pos.x_,pos.y_); // Sets as initial position for OpenWindow()
	}
	void YumeGLRenderer::SetWindowTitle(const YumeString& title)
	{
		windowTitle_ = title;
		if(window_)
			SDL_SetWindowTitle(window_,title.c_str());
	}

	void YumeGLRenderer::AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless)
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

	void YumeGLRenderer::Close()
	{
		if(!IsInitialized())
			return;

		// Actually close the window
		Release(true,true);
	}

	void YumeGLRenderer::Release(bool clearGPUObjects,bool closeWindow)
	{
		if(!window_)
			return;

		{
			boost::mutex::scoped_lock lock(gpuResourceMutex_);

			if(clearGPUObjects)
			{

				for(YumeVector<YumeGpuResource*>::iterator i = gpuResources_.begin(); i != gpuResources_.end(); ++i)
					(*i)->Release();
				gpuResources_.clear();
			}
			else
			{
				// We are not shutting down, but recreating the context: mark GPU objects lost
				for(YumeVector<YumeGpuResource*>::iterator i = gpuResources_.begin(); i != gpuResources_.end(); ++i)
					static_cast<YumeGLResource*>(*i)->OnDeviceLost();

				// In this case clear shader programs last so that they do not attempt to delete their OpenGL program
				// from a context that may no longer exist
				shaderPrograms_.clear();

				//SendEvent(E_DEVICELOST);
			}
		}

		CleanupFramebuffers();
		depthTextures_.clear();

		if(impl_->context_)
		{
			// Do not log this message if we are exiting
			if(!clearGPUObjects)
				YUMELOG_INFO("OpenGL context lost");

			SDL_GL_DeleteContext(impl_->context_);
			impl_->context_ = 0;
		}

		if(closeWindow)
		{
			SDL_ShowCursor(SDL_TRUE);

			// Do not destroy external window except when shutting down
			if(clearGPUObjects)
			{
				SDL_DestroyWindow(window_);
				window_ = 0;
			}
		}
	}
	bool YumeGLRenderer::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
		int multiSample)
	{
		bool maximize = false;

		// Fullscreen or Borderless can not be resizable
		if(fullscreen || borderless)
			resizable = false;

		// Borderless cannot be fullscreen, they are mutually exclusive
		if(borderless)
			fullscreen = false;


		multiSample = Clamp(multiSample,1,16);

		if(IsInitialized() && width == windowWidth_ && height == windowHeight_ && fullscreen == fullscreen_ && borderless == borderless_ &&
			resizable == resizeable_ && vsync == vsync_ && tripleBuffer == tripleBuffer_ && multiSample == multiSample_)
			return true;

		// If only vsync changes, do not destroy/recreate the context
		if(IsInitialized() && width == windowWidth_ && height == windowHeight_ && fullscreen == fullscreen_ && borderless == borderless_ &&
			resizable == resizeable_ && tripleBuffer == tripleBuffer_ && multiSample == multiSample_ && vsync != vsync_)
		{
			SDL_GL_SetSwapInterval(vsync ? 1 : 0);
			vsync_ = vsync;
			return true;
		}

		// If zero dimensions in windowed mode, set windowed mode to maximize and set a predefined default restored window size.
		// If zero in fullscreen, use desktop mode
		if(!width || !height)
		{
			if(fullscreen || borderless)
			{
				SDL_DisplayMode mode;
				SDL_GetDesktopDisplayMode(0,&mode);
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

		if(fullscreen)
		{
			YumeVector<IntVector2>::type resolutions = GetScreenResolutions();
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

		if(!impl_->context_)
		{
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);


#ifndef GL_ES_VERSION_2_0
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);



			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);

			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);

			if(!forceGL2_)
			{
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
			}
			else
			{
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,0);
			}
#else
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#endif

			if(multiSample > 1)
			{
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,multiSample);
			}
			else
			{
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,0);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,0);
			}

			int x = fullscreen ? 0 : windowPos_.x_;
			int y = fullscreen ? 0 : windowPos_.y_;

			unsigned flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
			if(fullscreen)
				flags |= SDL_WINDOW_FULLSCREEN;
			if(resizable)
				flags |= SDL_WINDOW_RESIZABLE;
			if(borderless)
				flags |= SDL_WINDOW_BORDERLESS;

			SDL_SetHint(SDL_HINT_ORIENTATIONS,"LandscapeLeft");

			for(;;)
			{
				window_ = SDL_CreateWindow(windowTitle_.c_str(),x,y,width,height,flags);

				if(window_)
					break;
				else
				{
					if(multiSample > 1)
					{
						// If failed with multisampling, retry first without
						multiSample = 1;
						SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,0);
						SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,0);
					}
					else
					{
						YUMELOG_ERROR("Could not create window, root cause: " << SDL_GetError());
						return false;
					}
				}
			}

			if(maximize)
			{
				Maximize();
				SDL_GetWindowSize(window_,&width,&height);
			}

			Restore();

			if(!impl_->context_)
				return false;
		}

		YumeRHI::SetGraphicsMode(width,height,fullscreen,borderless,resizable,vsync,tripleBuffer,multiSample);

		SDL_GL_SetSwapInterval(vsync ? 1 : 0);

		fullscreen_ = fullscreen;
		resizeable_ = resizable;
		borderless_ = borderless;
		vsync_ = vsync;
		tripleBuffer_ = tripleBuffer;
		multiSample_ = multiSample;

		int *x = new int;
		int *y = new int;

		if(!fullscreen)
			SDL_GetWindowPosition(window_,x,y);

		SDL_GetWindowSize(window_,&windowWidth_,&windowHeight_);
		if(!fullscreen)
			SDL_GetWindowPosition(window_,&windowPos_.x_,&windowPos_.y_);

		// Reset rendertargets and viewport for the new screen mode
		ResetRenderTargets();

		// Clear the initial window contents to black
		Clear(CLEAR_COLOR);
		SDL_GL_SwapWindow(window_);

		YUMELOG_INFO("Graphics Mode: " << std::endl <<
			"Width: " << width << std::endl <<
			"Height: " << height << std::endl <<
			"Fullscreen: " << fullscreen << std::endl <<
			"Borderless: " << borderless << std::endl <<
			"Vsync: " << vsync << std::endl <<
			"Multisample: " << multiSample <<
			"Triple Buffer: " << tripleBuffer
			);

		CreateRendererCapabilities();

		initialized_ = true;
		return true;
		}

	void YumeGLRenderer::Restore()
	{
		if(!window_)
			return;

		// Ensure first that the context exists
		if(!impl_->context_)
		{
			impl_->context_ = SDL_GL_CreateContext(window_);

#ifndef GL_ES_VERSION_2_0
			// If we're trying to use OpenGL 3, but context creation fails, retry with 2
			if(!forceGL2_ && !impl_->context_)
			{
				forceGL2_ = true;
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,0);
				impl_->context_ = SDL_GL_CreateContext(window_);
			}
#endif

			if(!impl_->context_)
			{
				YUMELOG_ERROR("Could not create OpenGL context, root cause " << SDL_GetError());
				return;
			}

			//// Clear cached extensions string from the previous context
			//extensions.Clear();

			// Initialize OpenGL extensions library (desktop only)
#ifndef GL_ES_VERSION_2_0
			GLenum err = glewInit();
			if(GLEW_OK != err)
			{
				YUMELOG_ERROR("Could not initialize OpenGL extensions, root cause:" << glewGetErrorString(err));
				return;
			}

			if(!forceGL2_ && GLEW_VERSION_3_2)
			{
				gl3Support = true;
				apiName_ = "GL3";

				// Create and bind a vertex array object that will stay in use throughout
				unsigned vertexArrayObject;
				glGenVertexArrays(1,&vertexArrayObject);
				glBindVertexArray(vertexArrayObject);
			}
			else if(GLEW_VERSION_2_0)
			{
				if(!GLEW_EXT_framebuffer_object || !GLEW_EXT_packed_depth_stencil)
				{
					YUMELOG_ERROR("EXT_framebuffer_object and EXT_packed_depth_stencil OpenGL extensions are required");
					return;
				}

				gl3Support = false;
				apiName_ = "GL2";
			}
			else
			{
				YUMELOG_ERROR("OpenGL 2.0 is required");
				return;
			}
#endif

			// Set up texture data read/write alignment. It is important that this is done before uploading any texture data
			glPixelStorei(GL_PACK_ALIGNMENT,1);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			ResetCache();
		}
	}

	void YumeGLRenderer::Maximize()
	{
		if(!window_)
			return;

		SDL_MaximizeWindow(window_);
	}

	YumeVector<IntVector2>::type YumeGLRenderer::GetScreenResolutions()
	{
		YumeVector<IntVector2>::type ret;
		// Emscripten is not able to return a valid list
#ifndef __EMSCRIPTEN__
		unsigned numModes = (unsigned)SDL_GetNumDisplayModes(0);

		for(unsigned i = 0; i < numModes; ++i)
		{
			SDL_DisplayMode mode;
			SDL_GetDisplayMode(0,i,&mode);
			int width = mode.w;
			int height = mode.h;

			// Store mode if unique
			bool unique = true;
			for(unsigned j = 0; j < ret.size(); ++j)
			{
				if(ret[j].x_ == width && ret[j].y_ == height)
				{
					unique = false;
					break;
				}
			}

			if(unique)
				ret.push_back(IntVector2(width,height));
		}
#endif

		return ret;
	}

	void YumeGLRenderer::SetRenderTarget(unsigned index,YumeRenderable* renderTarget)
	{
		if(index >= MAX_RENDERTARGETS)
			return;

		if(renderTarget != renderTargets_[index])
		{
			renderTargets_[index] = renderTarget;

			// If the rendertarget is also bound as a texture, replace with backup texture or null
			if(renderTarget)
			{
				YumeTexture* parentTexture = renderTarget->GetParentTexture();

				for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
				{
					if(textures_[i] == parentTexture)
						SetTexture(i,textures_[i]->GetBackupTexture());
				}
			}

			impl_->fboDirty_ = true;
		}
	}

	void YumeGLRenderer::SetDepthStencil(YumeRenderable* depthStencil)
	{
		// If we are using a rendertarget texture, it is required in OpenGL to also have an own depth-stencil
		// Create a new depth-stencil texture as necessary to be able to provide similar behaviour as Direct3D9
		if(renderTargets_[0] && !depthStencil)
		{
			int width = renderTargets_[0]->GetWidth();
			int height = renderTargets_[0]->GetHeight();

			// Direct3D9 default depth-stencil can not be used when rendertarget is larger than the window.
			// Check size similarly
			if(width <= windowWidth_ && height <= windowHeight_)
			{
				int searchKey = (width << 16) | height;
				YumeMap<int,SharedPtr<YumeTexture2D> >::iterator i = depthTextures_.find(searchKey);
				if(i != depthTextures_.end())
					depthStencil = i->second->GetRenderSurface();
				else
				{
					SharedPtr<YumeTexture2D> newDepthTexture = SharedPtr<YumeGLTexture2D>(new YumeGLTexture2D(this));
					newDepthTexture->SetSize(width,height,GetDepthStencilFormat(),TEXTURE_DEPTHSTENCIL);
					depthTextures_[searchKey] = newDepthTexture;
					depthStencil = newDepthTexture->GetRenderSurface();
				}
			}
		}

		if(depthStencil != depthStencil_)
		{
			depthStencil_ = depthStencil;
			impl_->fboDirty_ = true;
		}
	}

	void YumeGLRenderer::SetDepthStencil(YumeTexture2D* texture)
	{
		YumeRenderable* depthStencil = 0;
		if(texture)
			depthStencil = texture->GetRenderSurface();

		SetDepthStencil(depthStencil);
	}

	void YumeGLRenderer::SetBlendMode(BlendMode mode)
	{
		if(mode != blendMode_)
		{
			if(mode == BLEND_REPLACE)
				glDisable(GL_BLEND);
			else
			{
				glEnable(GL_BLEND);
				glBlendFunc(glSrcBlend[mode],glDestBlend[mode]);
				glBlendEquation(glBlendOp[mode]);
			}

			blendMode_ = mode;
		}
	}

	void YumeGLRenderer::SetColorWrite(bool enable)
	{
		if(enable != colorWrite_)
		{
			if(enable)
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			else
				glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

			colorWrite_ = enable;
		}
	}

	void YumeGLRenderer::SetCullMode(CullMode mode)
	{
		if(mode != cullMode_)
		{
			if(mode == CULL_NONE)
				glDisable(GL_CULL_FACE);
			else
			{
				// Use Direct3D convention, ie. clockwise vertices define a front face
				glEnable(GL_CULL_FACE);
				glCullFace(mode == CULL_CCW ? GL_FRONT : GL_BACK);
			}

			cullMode_ = mode;
		}
	}

	void YumeGLRenderer::SetDepthBias(float constantBias,float slopeScaledBias)
	{
		if(constantBias != constantDepthBias_ || slopeScaledBias != slopeScaledDepthBias_)
		{
#ifndef GL_ES_VERSION_2_0
			if(slopeScaledBias != 0.0f)
			{
				// OpenGL constant bias is unreliable and dependant on depth buffer bitdepth, apply in the projection matrix instead
				float adjustedSlopeScaledBias = slopeScaledBias + 1.0f;
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(adjustedSlopeScaledBias,0.0f);
			}
			else
				glDisable(GL_POLYGON_OFFSET_FILL);
#endif

			constantDepthBias_ = constantBias;
			slopeScaledDepthBias_ = slopeScaledBias;
			// Force update of the projection matrix shader parameter
			ClearParameterSource(SP_CAMERA);
		}
	}

	void YumeGLRenderer::SetDepthTest(CompareMode mode)
	{
		if(mode != depthTestMode_)
		{
			glDepthFunc(glCmpFunc[mode]);
			depthTestMode_ = mode;
		}
	}

	void YumeGLRenderer::SetDepthWrite(bool enable)
	{
		if(enable != depthWrite_)
		{
			glDepthMask(enable ? GL_TRUE : GL_FALSE);
			depthWrite_ = enable;
		}
	}

	void YumeGLRenderer::SetFillMode(FillMode mode)
	{
#ifndef GL_ES_VERSION_2_0
		if(mode != fillMode_)
		{
			glPolygonMode(GL_FRONT_AND_BACK,glFillMode[mode]);
			fillMode_ = mode;
		}
#endif
	}


	void YumeGLRenderer::SetScissorTest(bool enable,const Rect& rect,bool borderInclusive)
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

			if(enable && scissorRect_ != intRect)
			{
				// Use Direct3D convention with the vertical coordinates ie. 0 is top
				glScissor(intRect.left_,rtSize.y_ - intRect.bottom_,intRect.Width(),intRect.Height());
				scissorRect_ = intRect;
			}
		}
		else
			scissorRect_ = IntRect::ZERO;

		if(enable != scissorTest_)
		{
			if(enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
			scissorTest_ = enable;
		}
	}

	void YumeGLRenderer::SetScissorTest(bool enable,const IntRect& rect)
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

			if(enable && scissorRect_ != intRect)
			{
				// Use Direct3D convention with the vertical coordinates ie. 0 is top
				glScissor(intRect.left_,rtSize.y_ - intRect.bottom_,intRect.Width(),intRect.Height());
				scissorRect_ = intRect;
			}
		}
		else
			scissorRect_ = IntRect::ZERO;

		if(enable != scissorTest_)
		{
			if(enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
			scissorTest_ = enable;
		}
	}


	void YumeGLRenderer::SetClipPlane(bool enable,const Plane& clipPlane,const Matrix3x4& view,const Matrix4& projection)
	{
#ifndef GL_ES_VERSION_2_0
		if(enable != useClipPlane_)
		{
			if(enable)
				glEnable(GL_CLIP_PLANE0);
			else
				glDisable(GL_CLIP_PLANE0);

			useClipPlane_ = enable;
		}

		if(enable)
		{
			Matrix4 viewProj = projection * view;
			clipPlane_ = clipPlane.Transformed(viewProj).ToVector4();

			if(!gl3Support)
			{
				GLdouble planeData[4];
				planeData[0] = clipPlane_.x_;
				planeData[1] = clipPlane_.y_;
				planeData[2] = clipPlane_.z_;
				planeData[3] = clipPlane_.w_;

				glClipPlane(GL_CLIP_PLANE0,&planeData[0]);
			}
		}
#endif
	}

	void YumeGLRenderer::SetStencilTest(bool enable,CompareMode mode,StencilOp pass,StencilOp fail,StencilOp zFail,unsigned stencilRef,
		unsigned compareMask,unsigned writeMask)
	{

#ifndef GL_ES_VERSION_2_0
		if(enable != stencilTest_)
		{
			if(enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
			stencilTest_ = enable;
		}

		if(enable)
		{
			if(mode != stencilTestMode_ || stencilRef != stencilRef_ || compareMask != stencilCompareMask_)
			{
				glStencilFunc(glCmpFunc[mode],stencilRef,compareMask);
				stencilTestMode_ = mode;
				stencilRef_ = stencilRef;
				stencilCompareMask_ = compareMask;
			}
			if(writeMask != stencilWriteMask_)
			{
				glStencilMask(writeMask);
				stencilWriteMask_ = writeMask;
			}
			if(pass != stencilPass_ || fail != stencilFail_ || zFail != stencilZFail_)
			{
				glStencilOp(glStencilOps[fail],glStencilOps[zFail],glStencilOps[pass]);
				stencilPass_ = pass;
				stencilFail_ = fail;
				stencilZFail_ = zFail;
			}
		}
#endif
	}

	void YumeGLRenderer::ClearParameterSource(ShaderParameterGroup group)
	{
		if(shaderProgram_)
			shaderProgram_->ClearParameterSource(group);
	}

	void YumeGLRenderer::ClearParameterSources()
	{
		YumeGLShaderProgram::ClearParameterSources();
	}

	void YumeGLRenderer::ClearTransformSources()
	{
		if(shaderProgram_)
		{
			shaderProgram_->ClearParameterSource(SP_CAMERA);
			shaderProgram_->ClearParameterSource(SP_OBJECT);
		}
	}


	YumeShaderVariation* YumeGLRenderer::GetShader(ShaderType type,const YumeString& name,const YumeString& defines) const
	{
		return GetShader(type,name.c_str(),defines.c_str());
	}

	YumeShaderVariation* YumeGLRenderer::GetShader(ShaderType type,const char* name,const char* defines) const
	{
		if(lastShaderName_ != name || !lastShader_)
		{
			YumeResourceManager* resource_ = YumeEngine3D::Get()->GetResourceManager();
			YumeString fullShaderName = shaderPath_ + name + shaderExtension_;
			// Try to reduce repeated error log prints because of missing shaders
			if(lastShaderName_ == name && !resource_->Exists(fullShaderName))
				return 0;

			lastShader_ = resource_->PrepareResource<YumeGLShader>(fullShaderName);
			lastShaderName_ = name;
		}

		return lastShader_ ? lastShader_->GetVariation(type,defines) : (YumeGLShaderVariation*)0;
	}

	void YumeGLRenderer::CleanupShaderPrograms(YumeShaderVariation* variation)
	{
		for(ShaderProgramMap::iterator i = shaderPrograms_.begin(); i != shaderPrograms_.end();)
		{
			if(i->second->GetVertexShader() == variation || i->second->GetPixelShader() == variation)
			{
				i->second.reset();
				i = shaderPrograms_.erase(i);
			}
			else
				++i;
		}

		if(vertexShader_ == variation || pixelShader_ == variation)
			shaderProgram_= 0;
	}

	YumeConstantBuffer* YumeGLRenderer::GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size)
	{
		unsigned key = (bindingIndex << 16) | size;
		YumeMap<unsigned,SharedPtr<YumeConstantBuffer> >::iterator i = constantBuffers_.find(key);
		std::pair<YumeMap<unsigned,SharedPtr<YumeConstantBuffer> >::iterator,bool> ret;
		if(i == constantBuffers_.end())
		{
			ret = constantBuffers_.insert(std::make_pair(key,SharedPtr<YumeConstantBuffer>(new YumeGLConstantBuffer(this))));
			ret.first->second->SetSize(size);
		}
		return ret.first->second.get();
	}


	void YumeGLRenderer::SetVertexBuffer(YumeVertexBuffer* buffer)
	{
		// Note: this is not multi-instance safe
		static YumeVector<YumeVertexBuffer*>::type vertexBuffers(1);
		static YumeVector<unsigned>::type elementMasks(1);
		vertexBuffers[0] = buffer;
		elementMasks[0] = MASK_DEFAULT;
		SetVertexBuffers(vertexBuffers,elementMasks);
	}

	bool YumeGLRenderer::SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,
		unsigned instanceOffset)
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

		bool changed = false;
		unsigned newAttributes = 0;

		for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			YumeGLVertexBuffer* buffer = 0;
			unsigned elementMask = 0;

			if(i < buffers.size() && buffers[i])
			{
				buffer = static_cast<YumeGLVertexBuffer*>(buffers[i]);
				if(elementMasks[i] == MASK_DEFAULT)
					elementMask = buffer->GetElementMask();
				else
					elementMask = buffer->GetElementMask() & elementMasks[i];
			}

			// If buffer and element mask have stayed the same, skip to the next buffer
			if(buffer == vertexBuffers_[i] && elementMask == elementMasks_[i] && instanceOffset == lastInstanceOffset_ && !changed)
			{
				newAttributes |= elementMask;
				continue;
			}

			vertexBuffers_[i] = buffer;
			elementMasks_[i] = elementMask;
			changed = true;

			// Beware buffers with missing OpenGL objects, as binding a zero buffer object means accessing CPU memory for vertex data,
			// in which case the pointer will be invalid and cause a crash
			if(!buffer || !buffer->GetGPUObject())
				continue;

			SetVBO(buffer->GetGPUObject());
			unsigned vertexSize = buffer->GetVertexSize();

			for(unsigned j = 0; j < MAX_VERTEX_ELEMENTS; ++j)
			{
				unsigned attrIndex = glVertexAttrIndex[j];
				unsigned elementBit = (unsigned)(1 << j);

				if(elementMask & elementBit)
				{
					newAttributes |= elementBit;

					// Enable attribute if not enabled yet
					if((impl_->enabledAttributes_ & elementBit) == 0)
					{
						glEnableVertexAttribArray(attrIndex);
						impl_->enabledAttributes_ |= elementBit;
					}

					// Set the attribute pointer. Add instance offset for the instance matrix pointers
					unsigned offset = (j >= ELEMENT_INSTANCEMATRIX1 && j < ELEMENT_OBJECTINDEX) ? instanceOffset * vertexSize : 0;
					glVertexAttribPointer(attrIndex,YumeGLVertexBuffer::elementComponents[j],YumeGLVertexBuffer::elementType[j],
						(GLboolean)YumeGLVertexBuffer::elementNormalize[j],vertexSize,
						reinterpret_cast<const GLvoid*>(buffer->GetElementOffset((VertexElement)j) + offset));
				}
			}
		}

		if(!changed)
			return true;

		lastInstanceOffset_ = instanceOffset;

		// Now check which vertex attributes should be disabled
		unsigned disableAttributes = impl_->enabledAttributes_ & (~newAttributes);
		unsigned disableIndex = 0;

		while(disableAttributes)
		{
			if(disableAttributes & 1)
			{
				glDisableVertexAttribArray(glVertexAttrIndex[disableIndex]);
				impl_->enabledAttributes_ &= ~(1 << disableIndex);
			}
			disableAttributes >>= 1;
			++disableIndex;
		}

		return true;
	}

	bool YumeGLRenderer::SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,
		unsigned instanceOffset)
	{
		return SetVertexBuffers(reinterpret_cast<const YumeVector<YumeVertexBuffer*>::type&>(buffers),elementMasks,instanceOffset);
	}

	void YumeGLRenderer::SetIndexBuffer(YumeIndexBuffer* buffer)
	{
		if(indexBuffer_ == buffer)
			return;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffer ? static_cast<YumeGLIndexBuffer*>(buffer)->GetGPUObject() : 0);
		indexBuffer_ = buffer;
	}

	bool YumeGLRenderer::NeedParameterUpdate(ShaderParameterGroup group,const void* source)
	{
		return shaderProgram_ ? shaderProgram_->NeedParameterUpdate(group,source) : false;
	}

	void YumeGLRenderer::SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps)
	{
		if(vs == vertexShader_ && ps == pixelShader_)
			return;

		// Compile the shaders now if not yet compiled. If already attempted, do not retry
		if(vs && !static_cast<YumeGLShaderVariation*>(vs)->GetGPUObject())
		{
			if(vs->GetCompilerOutput().empty())
			{

				bool success = vs->Create();
				if(success)
					YUMELOG_DEBUG("Compiled vertex shader " + vs->GetFullName());
				else
				{
					YUMELOG_ERROR("Failed to compile vertex shader " + vs->GetFullName() + ":\n" + vs->GetCompilerOutput());
					vs = 0;
				}
			}
			else
				vs = 0;
		}

		if(ps && !static_cast<YumeGLShaderVariation*>(ps)->GetGPUObject())
		{
			if(ps->GetCompilerOutput().empty())
			{
				bool success = ps->Create();
				if(success)
					YUMELOG_DEBUG("Compiled pixel shader " + ps->GetFullName());
				else
				{
					YUMELOG_ERROR("Failed to compile pixel shader " + ps->GetFullName() + ":\n" + ps->GetCompilerOutput());
					ps = 0;
				}
			}
			else
				ps = 0;
		}

		if(!vs || !ps)
		{
			glUseProgram(0);
			vertexShader_ = 0;
			pixelShader_ = 0;
			shaderProgram_ = 0;
		}
		else
		{
			vertexShader_ = vs;
			pixelShader_ = ps;

			std::pair<YumeShaderVariation*,YumeShaderVariation*> combination(vs,ps);
			ShaderProgramMap::iterator i = shaderPrograms_.find(combination);

			if(i != shaderPrograms_.end())
			{
				SharedPtr<YumeGLShaderProgram> glShader = boost::static_pointer_cast<YumeGLShaderProgram>(i->second);
				// Use the existing linked program
				if(glShader->GetGPUObject())
				{
					glUseProgram(glShader->GetGPUObject());
					shaderProgram_ = glShader.get();
				}
				else
				{
					glUseProgram(0);
					shaderProgram_ = 0;
				}
			}
			else
			{
				SharedPtr<YumeGLShaderProgram> newProgram(new YumeGLShaderProgram(this,vs,ps));
				if(newProgram->Link())
				{
					YUMELOG_DEBUG("Linked vertex shader " + vs->GetFullName() + " and pixel shader " + ps->GetFullName());
					// Note: Link() calls glUseProgram() to set the texture sampler uniforms,
					// so it is not necessary to call it again
					shaderProgram_ = newProgram.get();
				}
				else
				{
					YUMELOG_ERROR("Failed to link vertex shader " + vs->GetFullName() + " and pixel shader " + ps->GetFullName() + ":\n" +
						newProgram->GetLinkerOutput());
					glUseProgram(0);
					shaderProgram_ = 0;
				}

				shaderPrograms_[combination] = newProgram;
			}
		}

		// Update the clip plane uniform on GL3, and set constant buffers
#ifndef GL_ES_VERSION_2_0
		if(gl3Support && shaderProgram_)
		{
			const SharedPtr<YumeConstantBuffer>* constantBuffers = shaderProgram_->GetConstantBuffers();
			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS * 2; ++i)
			{
				YumeConstantBuffer* buffer = constantBuffers[i].get();
				if(buffer != currentConstantBuffers_[i])
				{
					unsigned object = buffer ? static_cast<YumeGLConstantBuffer*>(buffer)->GetGPUObject() : 0;
					glBindBufferBase(GL_UNIFORM_BUFFER,i,object);
					// Calling glBindBufferBase also affects the generic buffer binding point
					impl_->boundUBO_ = object;
					currentConstantBuffers_[i] = buffer;
					YumeGLShaderProgram::ClearGlobalParameterSource((ShaderParameterGroup)(i % MAX_SHADER_PARAMETER_GROUPS));
				}
			}

			SetShaderParameter(VSP_CLIPPLANE,useClipPlane_ ? clipPlane_ : Vector4(0.0f,0.0f,0.0f,1.0f));
		}
#endif

		// Store shader combination if shader dumping in progress
		//if(shaderPrecache_)
		//	shaderPrecache_->StoreShaders(vertexShader_,pixelShader_);
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const float* data,unsigned count)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,(unsigned)(count * sizeof(float)),data);
					return;
				}

				switch(info->type_)
				{
				case GL_FLOAT:
					glUniform1fv(info->location_,count,data);
					break;

				case GL_FLOAT_VEC2:
					glUniform2fv(info->location_,count / 2,data);
					break;

				case GL_FLOAT_VEC3:
					glUniform3fv(info->location_,count / 3,data);
					break;

				case GL_FLOAT_VEC4:
					glUniform4fv(info->location_,count / 4,data);
					break;

				case GL_FLOAT_MAT3:
					glUniformMatrix3fv(info->location_,count / 9,GL_FALSE,data);
					break;

				case GL_FLOAT_MAT4:
					glUniformMatrix4fv(info->location_,count / 16,GL_FALSE,data);
					break;

				default: break;
				}
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,float value)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(float),&value);
					return;
				}

				glUniform1fv(info->location_,1,&value);
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const YumeColor& color)
	{
		SetShaderParameter(param,color.Data(),4);
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Vector2& vector)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(Vector2),&vector);
					return;
				}

				// Check the uniform type to avoid mismatch
				switch(info->type_)
				{
				case GL_FLOAT:
					glUniform1fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC2:
					glUniform2fv(info->location_,1,vector.Data());
					break;

				default: break;
				}
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Matrix3& matrix)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetVector3ArrayParameter(info->location_,3,&matrix);
					return;
				}

				glUniformMatrix3fv(info->location_,1,GL_FALSE,matrix.Data());
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Vector3& vector)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(Vector3),&vector);
					return;
				}

				// Check the uniform type to avoid mismatch
				switch(info->type_)
				{
				case GL_FLOAT:
					glUniform1fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC2:
					glUniform2fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC3:
					glUniform3fv(info->location_,1,vector.Data());
					break;

				default: break;
				}
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Matrix4& matrix)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(Matrix4),&matrix);
					return;
				}

				glUniformMatrix4fv(info->location_,1,GL_FALSE,matrix.Data());
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Vector4& vector)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(Vector4),&vector);
					return;
				}

				// Check the uniform type to avoid mismatch
				switch(info->type_)
				{
				case GL_FLOAT:
					glUniform1fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC2:
					glUniform2fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC3:
					glUniform3fv(info->location_,1,vector.Data());
					break;

				case GL_FLOAT_VEC4:
					glUniform4fv(info->location_,1,vector.Data());
					break;

				default: break;
				}
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Matrix3x4& matrix)
	{
		if(shaderProgram_)
		{
			const ShaderParameter* info = shaderProgram_->GetParameter(param);
			if(info)
			{
				// Expand to a full Matrix4
				static Matrix4 fullMatrix;
				fullMatrix.m00_ = matrix.m00_;
				fullMatrix.m01_ = matrix.m01_;
				fullMatrix.m02_ = matrix.m02_;
				fullMatrix.m03_ = matrix.m03_;
				fullMatrix.m10_ = matrix.m10_;
				fullMatrix.m11_ = matrix.m11_;
				fullMatrix.m12_ = matrix.m12_;
				fullMatrix.m13_ = matrix.m13_;
				fullMatrix.m20_ = matrix.m20_;
				fullMatrix.m21_ = matrix.m21_;
				fullMatrix.m22_ = matrix.m22_;
				fullMatrix.m23_ = matrix.m23_;

				if(info->bufferPtr_)
				{
					YumeConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
						dirtyConstantBuffers_.push_back(buffer);
					buffer->SetParameter(info->location_,sizeof(Matrix4),&fullMatrix);
					return;
				}

				glUniformMatrix4fv(info->location_,1,GL_FALSE,fullMatrix.Data());
			}
		}
	}

	void YumeGLRenderer::SetShaderParameter(YumeHash param,const Variant& value)
	{

	}


	void YumeGLRenderer::Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount)
	{
		if(!vertexCount)
			return;

		PreDraw();

		unsigned primitiveCount;
		GLenum glPrimitiveType;

		GetGLPrimitiveType(vertexCount,type,primitiveCount,glPrimitiveType);
		glDrawArrays(glPrimitiveType,vertexStart,vertexCount);

		numPrimitives_ += primitiveCount;
		++numBatches_;
	}

	void YumeGLRenderer::Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount)
	{
		if(!indexCount || !indexBuffer_ || !static_cast<YumeGLIndexBuffer*>(indexBuffer_)->GetGPUObject())
			return;

		PreDraw();

		unsigned indexSize = indexBuffer_->GetIndexSize();
		unsigned primitiveCount;
		GLenum glPrimitiveType;

		GetGLPrimitiveType(indexCount,type,primitiveCount,glPrimitiveType);
		GLenum indexType = indexSize == sizeof(unsigned short) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		glDrawElements(glPrimitiveType,indexCount,indexType,reinterpret_cast<const GLvoid*>(indexStart * indexSize));

		numPrimitives_ += primitiveCount;
		++numBatches_;
	}

	void YumeGLRenderer::DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,
		unsigned instanceCount)
	{
#if !defined(GL_ES_VERSION_2_0) || defined(__EMSCRIPTEN__)
		if(!indexCount || !indexBuffer_ || !static_cast<YumeGLIndexBuffer*>(indexBuffer_)->GetGPUObject() || !instancingSupport_)
			return;

		PreDraw();

		unsigned indexSize = indexBuffer_->GetIndexSize();
		unsigned primitiveCount;
		GLenum glPrimitiveType;

		GetGLPrimitiveType(indexCount,type,primitiveCount,glPrimitiveType);
		GLenum indexType = indexSize == sizeof(unsigned short) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		if(gl3Support)
		{
			glDrawElementsInstanced(glPrimitiveType,indexCount,indexType,reinterpret_cast<const GLvoid*>(indexStart * indexSize),
				instanceCount);
		}
		else
		{
			glDrawElementsInstancedARB(glPrimitiveType,indexCount,indexType,reinterpret_cast<const GLvoid*>(indexStart * indexSize),
				instanceCount);
		}

		numPrimitives_ += instanceCount * primitiveCount;
		++numBatches_;
#endif
	}

	void YumeGLRenderer::SetTextureForUpdate(YumeTexture* texture)
	{
		if(impl_->activeTexture_ != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			impl_->activeTexture_ = 0;
		}

		unsigned glType = static_cast<YumeGLTexture2D*>(texture)->GetTarget();
		// Unbind old texture type if necessary
		if(textureTypes_[0] && textureTypes_[0] != glType)
			glBindTexture(textureTypes_[0],0);
		glBindTexture(glType,(GLuint)(static_cast<YumeGLTexture2D*>(texture)->GetGPUObject()));
		textureTypes_[0] = glType;
		textures_[0] = texture;
	}


	void YumeGLRenderer::PreDraw()
	{

#ifndef GL_ES_VERSION_2_0
		if(gl3Support)
		{
			for(YumeVector<YumeConstantBuffer*>::iterator i = dirtyConstantBuffers_.begin(); i != dirtyConstantBuffers_.end(); ++i)
				(*i)->Apply();
			dirtyConstantBuffers_.clear();
		}
#endif

		if(impl_->fboDirty_)
		{
			impl_->fboDirty_ = false;

			// First check if no framebuffer is needed. In that case simply return to backbuffer rendering
			bool noFbo = !depthStencil_;
			if(noFbo)
			{
				for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
				{
					if(renderTargets_[i])
					{
						noFbo = false;
						break;
					}
				}
			}

			if(noFbo)
			{
				if(impl_->boundFBO_ != impl_->systemFBO_)
				{
					BindFramebuffer(impl_->systemFBO_);
					impl_->boundFBO_ = impl_->systemFBO_;
				}

#ifndef GL_ES_VERSION_2_0
				// Disable/enable sRGB write
				if(sRGBWriteSupport_)
				{
					bool sRGBWrite = sRGB_;
					if(sRGBWrite != impl_->sRGBWrite_)
					{
						if(sRGBWrite)
							glEnable(GL_FRAMEBUFFER_SRGB_EXT);
						else
							glDisable(GL_FRAMEBUFFER_SRGB_EXT);
						impl_->sRGBWrite_ = sRGBWrite;
					}
				}
#endif

				return;
			}

			// Search for a new framebuffer based on format & size, or create new
			IntVector2 rtSize = YumeGLRenderer::GetRenderTargetDimensions();
			unsigned format = 0;
			if(renderTargets_[0])
				format = renderTargets_[0]->GetParentTexture()->GetFormat();
			else if(depthStencil_)
				format = depthStencil_->GetParentTexture()->GetFormat();

			unsigned long long fboKey = (rtSize.x_ << 16 | rtSize.y_) | (((unsigned long long)format) << 32);

			YumeMap<unsigned long long,FrameBufferObject>::iterator i = impl_->frameBuffers_.find(fboKey);
			std::pair<YumeMap<unsigned long long,FrameBufferObject>::iterator,bool> ret;
			if(i == impl_->frameBuffers_.end())
			{
				FrameBufferObject newFbo;
				newFbo.fbo_ = CreateFramebuffer();
				ret = impl_->frameBuffers_.insert(std::make_pair(fboKey,newFbo));

				if(ret.second)
				{
					i = ret.first;
				}
			}

			if(impl_->boundFBO_ != i->second.fbo_)
			{
				BindFramebuffer(i->second.fbo_);
				impl_->boundFBO_ = i->second.fbo_;
			}

#ifndef GL_ES_VERSION_2_0
			// Setup readbuffers & drawbuffers if needed
			if(i->second.readBuffers_ != GL_NONE)
			{
				glReadBuffer(GL_NONE);
				i->second.readBuffers_ = GL_NONE;
			}

			// Calculate the bit combination of non-zero color rendertargets to first check if the combination changed
			unsigned newDrawBuffers = 0;
			for(unsigned j = 0; j < MAX_RENDERTARGETS; ++j)
			{
				if(renderTargets_[j])
					newDrawBuffers |= 1 << j;
			}

			if(newDrawBuffers != i->second.drawBuffers_)
			{
				// Check for no color rendertargets (depth rendering only)
				if(!newDrawBuffers)
					glDrawBuffer(GL_NONE);
				else
				{
					int drawBufferIds[MAX_RENDERTARGETS];
					unsigned drawBufferCount = 0;

					for(unsigned j = 0; j < MAX_RENDERTARGETS; ++j)
					{
						if(renderTargets_[j])
						{
							if(!gl3Support)
								drawBufferIds[drawBufferCount++] = GL_COLOR_ATTACHMENT0_EXT + j;
							else
								drawBufferIds[drawBufferCount++] = GL_COLOR_ATTACHMENT0 + j;
						}
					}
					glDrawBuffers(drawBufferCount,(const GLenum*)drawBufferIds);
			}

				i->second.drawBuffers_ = newDrawBuffers;
		}
#endif

			for(unsigned j = 0; j < MAX_RENDERTARGETS; ++j)
			{
				if(renderTargets_[j])
				{
					YumeTexture* texture = renderTargets_[j]->GetParentTexture();

					// If texture's parameters are dirty, update before attaching
					if(texture->GetParametersDirty())
					{
						SetTextureForUpdate(texture);
						texture->UpdateParameters();
						SetTexture(0,0);
					}

					if(i->second.colorAttachments_[j] != renderTargets_[j])
					{
						BindColorAttachment(j,static_cast<YumeGLRenderable*>(renderTargets_[j])->GetTarget(),static_cast<YumeGLTexture2D*>(texture)->GetGPUObject());
						i->second.colorAttachments_[j] = renderTargets_[j];
					}
				}
				else
				{
					if(i->second.colorAttachments_[j])
					{
						BindColorAttachment(j,GL_TEXTURE_2D,0);
						i->second.colorAttachments_[j] = 0;
					}
				}
			}

			if(depthStencil_)
			{
				// Bind either a renderbuffer or a depth texture, depending on what is available
				YumeTexture* texture = depthStencil_->GetParentTexture();
#ifndef GL_ES_VERSION_2_0
				bool hasStencil = texture->GetFormat() == GL_DEPTH24_STENCIL8_EXT;
#else
				bool hasStencil = texture->GetFormat() == GL_DEPTH24_STENCIL8_OES;
#endif
				unsigned renderBufferID = static_cast<YumeGLRenderable*>(depthStencil_)->GetRenderBuffer();
				if(!renderBufferID)
				{
					// If texture's parameters are dirty, update before attaching
					if(texture->GetParametersDirty())
					{
						SetTextureForUpdate(texture);
						texture->UpdateParameters();
						SetTexture(0,0);
					}

					if(i->second.depthAttachment_ != depthStencil_)
					{
						BindDepthAttachment(static_cast<YumeGLTexture2D*>(texture)->GetGPUObject(),false);
						BindStencilAttachment(hasStencil ? static_cast<YumeGLTexture2D*>(texture)->GetGPUObject() : 0,false);
						i->second.depthAttachment_ = depthStencil_;
					}
				}
				else
				{
					if(i->second.depthAttachment_ != depthStencil_)
					{
						BindDepthAttachment(renderBufferID,true);
						BindStencilAttachment(hasStencil ? renderBufferID : 0,true);
						i->second.depthAttachment_ = depthStencil_;
					}
				}
			}
			else
			{
				if(i->second.depthAttachment_)
				{
					BindDepthAttachment(0,false);
					BindStencilAttachment(0,false);
					i->second.depthAttachment_ = 0;
				}
			}

#ifndef GL_ES_VERSION_2_0
			// Disable/enable sRGB write
			if(sRGBWriteSupport_)
			{
				bool sRGBWrite = renderTargets_[0] ? renderTargets_[0]->GetParentTexture()->GetSRGB() : sRGB_;
				if(sRGBWrite != impl_->sRGBWrite_)
				{
					if(sRGBWrite)
						glEnable(GL_FRAMEBUFFER_SRGB_EXT);
					else
						glDisable(GL_FRAMEBUFFER_SRGB_EXT);
					impl_->sRGBWrite_ = sRGBWrite;
				}
			}
#endif
	}
	}


	void YumeGLRenderer::CleanupRenderable(YumeRenderable* surface)
	{
		if(!surface)
			return;

		// Flush pending FBO changes first if any
		PreDraw();

		unsigned currentFBO = impl_->boundFBO_;

		// Go through all FBOs and clean up the surface from them
		for(YumeMap<unsigned long long,FrameBufferObject>::iterator i = impl_->frameBuffers_.begin();
			i != impl_->frameBuffers_.end(); ++i)
		{
			for(unsigned j = 0; j < MAX_RENDERTARGETS; ++j)
			{
				if(i->second.colorAttachments_[j] == surface)
				{
					if(currentFBO != i->second.fbo_)
					{
						BindFramebuffer(i->second.fbo_);
						currentFBO = i->second.fbo_;
					}
					BindColorAttachment(j,GL_TEXTURE_2D,0);
					i->second.colorAttachments_[j] = 0;
					// Mark drawbuffer bits to need recalculation
					i->second.drawBuffers_ = M_MAX_UNSIGNED;
				}
			}
			if(i->second.depthAttachment_ == surface)
			{
				if(currentFBO != i->second.fbo_)
				{
					BindFramebuffer(i->second.fbo_);
					currentFBO = i->second.fbo_;
				}
				BindDepthAttachment(0,false);
				BindStencilAttachment(0,false);
				i->second.depthAttachment_ = 0;
			}
		}

		// Restore previously bound FBO now if needed
		if(currentFBO != impl_->boundFBO_)
			BindFramebuffer(impl_->boundFBO_);
	}


	void YumeGLRenderer::CleanupFramebuffers()
	{
		if(!IsDeviceLost())
		{
			BindFramebuffer(impl_->systemFBO_);
			impl_->boundFBO_ = impl_->systemFBO_;
			impl_->fboDirty_ = true;

			for(YumeMap<unsigned long long,FrameBufferObject>::iterator i = impl_->frameBuffers_.begin();
				i != impl_->frameBuffers_.end(); ++i)
				DeleteFramebuffer(i->second.fbo_);
		}
		else
			impl_->boundFBO_ = 0;

		impl_->frameBuffers_.clear();
	}


	unsigned YumeGLRenderer::CreateFramebuffer()
	{
		unsigned newFbo = 0;
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glGenFramebuffersEXT(1,&newFbo);
		else
#endif
			glGenFramebuffers(1,&newFbo);
		return newFbo;
	}

	void YumeGLRenderer::DeleteFramebuffer(unsigned fbo)
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glDeleteFramebuffersEXT(1,&fbo);
		else
#endif
			glDeleteFramebuffers(1,&fbo);
	}

	void YumeGLRenderer::BindFramebuffer(unsigned fbo)
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo);
		else
#endif
			glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	}

	void YumeGLRenderer::BindColorAttachment(unsigned index,unsigned target,unsigned object)
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT + index,target,object,0);
		else
#endif
			glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + index,target,object,0);
	}

	void YumeGLRenderer::BindDepthAttachment(unsigned object,bool isRenderBuffer)
	{
		if(!object)
			isRenderBuffer = false;

#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
		{
			if(!isRenderBuffer)
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D,object,0);
			else
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,object);
		}
		else
#endif
		{
			if(!isRenderBuffer)
				glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,object,0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,object);
		}
	}

	void YumeGLRenderer::BindStencilAttachment(unsigned object,bool isRenderBuffer)
	{
		if(!object)
			isRenderBuffer = false;

#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
		{
			if(!isRenderBuffer)
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_TEXTURE_2D,object,0);
			else
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,object);
		}
		else
#endif
		{
			if(!isRenderBuffer)
				glFramebufferTexture2D(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_TEXTURE_2D,object,0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,object);
		}
	}

	bool YumeGLRenderer::CheckFramebuffer()
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			return glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT;
		else
#endif
			return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}



	void YumeGLRenderer::SetTextureUnitMappings()
	{
		textureUnits_["DiffMap"] = TU_DIFFUSE;
		textureUnits_["DiffCubeMap"] = TU_DIFFUSE;
		textureUnits_["AlbedoBuffer"] = TU_ALBEDOBUFFER;
		textureUnits_["NormalMap"] = TU_NORMAL;
		textureUnits_["NormalBuffer"] = TU_NORMALBUFFER;
		textureUnits_["SpecMap"] = TU_SPECULAR;
		textureUnits_["EmissiveMap"] = TU_EMISSIVE;
		textureUnits_["EnvMap"] = TU_ENVIRONMENT;
		textureUnits_["EnvCubeMap"] = TU_ENVIRONMENT;
		textureUnits_["LightRampMap"] = TU_LIGHTRAMP;
		textureUnits_["LightSpotMap"] = TU_LIGHTSHAPE;
		textureUnits_["LightCubeMap"] = TU_LIGHTSHAPE;
		textureUnits_["ShadowMap"] = TU_SHADOWMAP;
		textureUnits_["VolumeMap"] = TU_VOLUMEMAP;
		textureUnits_["FaceSelectCubeMap"] = TU_FACESELECT;
		textureUnits_["IndirectionCubeMap"] = TU_INDIRECTION;
		textureUnits_["DepthBuffer"] = TU_DEPTHBUFFER;
		textureUnits_["LightBuffer"] = TU_LIGHTBUFFER;
		textureUnits_["ZoneCubeMap"] = TU_ZONE;
		textureUnits_["ZoneVolumeMap"] = TU_ZONE;
	}


	void YumeGLRenderer::SetTexture(unsigned index,YumeTexture* texture)
	{
		if(index >= MAX_TEXTURE_UNITS)
			return;

		//// Check if texture is currently bound as a rendertarget. In that case, use its backup texture, or blank if not defined
		//if(texture)
		//{
		//	if(renderTargets_[0] && renderTargets_[0]->GetParentTexture() == texture)
		//		texture = texture->GetBackupTexture();
		//}

		//if(textures_[index] != texture)
		//{
		//	if(impl_->activeTexture_ != index)
		//	{
		//		glActiveTexture(GL_TEXTURE0 + index);
		//		impl_->activeTexture_ = index;
		//	}

		//	if(texture)
		//	{
		//		unsigned glType = texture->GetTarget();
		//		// Unbind old texture type if necessary
		//		if(textureTypes_[index] && textureTypes_[index] != glType)
		//			glBindTexture(textureTypes_[index],0);
		//		glBindTexture(glType,texture->GetGPUObject());
		//		textureTypes_[index] = glType;

		//		if(texture->GetParametersDirty())
		//			texture->UpdateParameters();
		//	}
		//	else if(textureTypes_[index])
		//	{
		//		glBindTexture(textureTypes_[index],0);
		//		textureTypes_[index] = 0;
		//	}

		//	textures_[index] = texture;
		//}
		//else
		//{
		//	if(texture && texture->GetParametersDirty())
		//	{
		//		if(impl_->activeTexture_ != index)
		//		{
		//			glActiveTexture(GL_TEXTURE0 + index);
		//			impl_->activeTexture_ = index;
		//		}

		//		glBindTexture(texture->GetTarget(),texture->GetGPUObject());
		//		texture->UpdateParameters();
		//	}
		//}
	}


	void YumeGLRenderer::MarkFBODirty()
	{
		impl_->fboDirty_ = true;
	}

	void YumeGLRenderer::SetVBO(unsigned object)
	{
		if(impl_->boundVBO_ != object)
		{
			if(object)
				glBindBuffer(GL_ARRAY_BUFFER,object);
			impl_->boundVBO_ = object;
		}
	}

	void YumeGLRenderer::SetUBO(unsigned object)
	{
#ifndef GL_ES_VERSION_2_0
		if(impl_->boundUBO_ != object)
		{
			if(object)
				glBindBuffer(GL_UNIFORM_BUFFER,object);
			impl_->boundUBO_ = object;
		}
#endif
		}

	unsigned YumeGLRenderer::GetAlphaFormat()
	{
#ifndef GL_ES_VERSION_2_0
		// Alpha format is deprecated on OpenGL 3+
		if(gl3Support)
			return GL_R8;
#endif
		return GL_ALPHA;
	}

	unsigned YumeGLRenderer::GetLuminanceFormat()
	{
#ifndef GL_ES_VERSION_2_0
		// Luminance format is deprecated on OpenGL 3+
		if(gl3Support)
			return GL_R8;
#endif
		return GL_LUMINANCE;
	}

	unsigned YumeGLRenderer::GetLuminanceAlphaFormat()
	{
#ifndef GL_ES_VERSION_2_0
		// Luminance alpha format is deprecated on OpenGL 3+
		if(gl3Support)
			return GL_RG8;
#endif
		return GL_LUMINANCE_ALPHA;
	}

	unsigned YumeGLRenderer::GetRGBFormat()
	{
		return GL_RGB;
	}

	unsigned YumeGLRenderer::GetRGBAFormat()
	{
		return GL_RGBA;
	}

	unsigned YumeGLRenderer::GetRGBA16Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RGBA16;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetRGBAFloat16Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RGBA16F_ARB;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetRGBAFloat32Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RGBA32F_ARB;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetRG16Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RG16;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetRGFloat16Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RG16F;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetRGFloat32Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_RG32F;
#else
		return GL_RGBA;
#endif
	}

	unsigned YumeGLRenderer::GetFloat16Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_R16F;
#else
		return GL_LUMINANCE;
#endif
	}

	unsigned YumeGLRenderer::GetFloat32Format()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_R32F;
#else
		return GL_LUMINANCE;
#endif
	}

	unsigned YumeGLRenderer::GetLinearDepthFormat()
	{
#ifndef GL_ES_VERSION_2_0
		// OpenGL 3 can use different color attachment formats
		if(gl3Support)
			return GL_R32F;
#endif
		// OpenGL 2 requires color attachments to have the same format, therefore encode deferred depth to RGBA manually
		// if not using a readable hardware depth texture
		return GL_RGBA;
}

	unsigned YumeGLRenderer::GetDepthStencilFormat()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_DEPTH24_STENCIL8_EXT;
#else
		return glesDepthStencilFormat;
#endif
	}

	unsigned YumeGLRenderer::GetReadableDepthFormat()
	{
#ifndef GL_ES_VERSION_2_0
		return GL_DEPTH_COMPONENT24;
#else
		return glesReadableDepthFormat;
#endif
	}


	unsigned YumeGLRenderer::GetFormat(CompressedFormat format) const
	{
		switch(format)
		{
		case CF_RGBA:
			return GL_RGBA;

		case CF_DXT1:
			return dxtTextureSupport_ ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : 0;

#if !defined(GL_ES_VERSION_2_0) || defined(__EMSCRIPTEN__)
		case CF_DXT3:
			return dxtTextureSupport_ ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : 0;

		case CF_DXT5:
			return dxtTextureSupport_ ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : 0;
#endif
#ifdef GL_ES_VERSION_2_0
		case CF_ETC1:
			return etcTextureSupport_ ? GL_ETC1_RGB8_OES : 0;

		case CF_PVRTC_RGB_2BPP:
			return pvrtcTextureSupport_ ? COMPRESSED_RGB_PVRTC_2BPPV1_IMG : 0;

		case CF_PVRTC_RGB_4BPP:
			return pvrtcTextureSupport_ ? COMPRESSED_RGB_PVRTC_4BPPV1_IMG : 0;

		case CF_PVRTC_RGBA_2BPP:
			return pvrtcTextureSupport_ ? COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : 0;

		case CF_PVRTC_RGBA_4BPP:
			return pvrtcTextureSupport_ ? COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : 0;
#endif

		default:
			return 0;
		}
	}


	unsigned YumeGLRenderer::GetFormat(const YumeString& formatName)
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

	void YumeGLRenderer::AddGpuResource(YumeGpuResource* gpuRes)
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		gpuResources_.push_back(gpuRes);
	}

	void YumeGLRenderer::RemoveGpuResource(YumeGpuResource* gpuRes)
	{
		//boost::mutex::scoped_lock lock(gpuResourceMutex_);

		//Check if valid
		GpuResourceVector::iterator It = std::find(gpuResources_.begin(),gpuResources_.end(),gpuRes);

		if(It != gpuResources_.end())
			gpuResources_.erase(It);
	}

	void YumeGLRenderer::RegisterFactories()
	{
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(("Shader"),[](void) -> YumeBase * { return new YumeGLShader();});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(("Texture2D"),[this](void) -> YumeBase * { return new YumeGLTexture2D(this);});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(("IndexBuffer"),[this](void) -> YumeBase * { return new YumeGLIndexBuffer(this);});
		YumeEngine3D::Get()->GetObjFactory()->RegisterFactoryFunction(("VertexBuffer"),[this](void) -> YumeBase * { return new YumeGLVertexBuffer(this);});
	}
	void YumeGLRenderer::UnregisterFactories()
	{
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(("Shader"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(("Texture2D"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(("IndexBuffer"));
		YumeEngine3D::Get()->GetObjFactory()->UnRegisterFactoryFunction(("VertexBuffer"));
	}
	}