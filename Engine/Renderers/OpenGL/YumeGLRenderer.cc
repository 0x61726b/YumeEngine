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

#include "Math/YumeMath.h"

#include "Logging/logging.h"

#include "Engine/YumeEngine.h"



#include <SDL_syswm.h>

namespace YumeEngine
{
	extern "C" void YumeGLExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeRenderer* graphics_ = new YumeGLRenderer;
		engine->SetRenderer(graphics_);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeGLExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRenderer* graphics_ = engine->GetRenderer();
		delete graphics_;
	}
	//---------------------------------------------------------------------
	YumeGLRenderer::YumeGLRenderer():
		impl_(new YumeGLRendererImpl()),
		windowWidth_(0),
		windowHeight_(0),
		windowPos_(SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED),
		multiSample_(1),
		fullscreen_(false),
		borderless_(false),
		resizeable_(false),
		vsync_(false),
		tripleBuffer_(false),
		sRGB_(false),
		forceGL2_(false),
		instancingSupport_(false),
		lightPrepassSupport_(false),
		deferredSupport_(false),
		anisotropySupport_(false),
		dxtTextureSupport_(false),
		etcTextureSupport_(false),
		pvrtcTextureSupport_(false),
		sRGBSupport_(false),
		sRGBWriteSupport_(false),
		numPrimitives_(0),
		numBatches_(0),
		dummyColorFormat_(0),
		shadowMapFormat_(GL_DEPTH_COMPONENT16),
		hiresShadowMapFormat_(GL_DEPTH_COMPONENT24),
#ifndef GL_ES_VERSION_2_0
		apiName_("GL2")
#else
		apiName_("GLES2")
#endif
	{

		YUMELOG_INFO("Initializing OpenGL graphics....");

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	}

	YumeGLRenderer::~YumeGLRenderer()
	{
		{
			boost::mutex::scoped_lock lock(gpuResourceMutex_);

			for(int i=0; i < gpuResources_.size(); ++i)
				gpuResources_[i]->Release();

			gpuResources_.clear();
		}

		if(window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(window_);
			window_ = 0;
		}

		delete impl_;
		impl_ = 0;

		// Shut down SDL now. Graphics should be the last SDL-using subsystem to be destroyed
		SDL_Quit();

	}

	bool YumeGLRenderer::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		if(fullscreen_ && (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED))
			return false;

		numPrimitives_	= 0;
		numBatches_		= 0;

		return true;
	}

	void YumeGLRenderer::EndFrame()
	{
		if(!IsInitialized())
			return;


	}

	void YumeGLRenderer::Clear(unsigned flags,const Vector4& color,float depth,unsigned stencil)
	{
		unsigned glFlags = 0;
		if(flags & CLEAR_COLOR)
		{
			glFlags |= GL_COLOR_BUFFER_BIT;
			glClearColor(color.x,color.y,color.z,color.w);
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

		glClear(glFlags);
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


#endif
	}

	void YumeGLRenderer::ResetRenderTargets()
	{
		SetViewport(Vector4(0,0,windowWidth_,windowHeight_));
	}

	void YumeGLRenderer::SetViewport(const Vector4& rect)
	{
		Vector2 size = GetRenderTargetDimensions();




		// Disable scissor test, needs to be re-enabled by the user
		/*SetScissorTest(false);*/
	}

	Vector2 YumeGLRenderer::GetRenderTargetDimensions() const
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
			SDL_SetWindowPosition(window_,pos.x,pos.y);
		else
			windowPos_ = pos; // Sets as initial position for OpenWindow()
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
		if(window_)
		{
			SDL_ShowCursor(SDL_TRUE);
			SDL_DestroyWindow(window_);
			window_ = 0;
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


		multiSample = Math::Clamp(multiSample,1,16);

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

			int x = fullscreen ? 0 : windowPos_.x;
			int y = fullscreen ? 0 : windowPos_.y;

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

		YumeRenderer::SetGraphicsMode(width,height,fullscreen,borderless,resizable,vsync,tripleBuffer,multiSample);

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
			//ResetCachedState();
		}
	}

	void YumeGLRenderer::Maximize()
	{
		if(!window_)
			return;

		SDL_MaximizeWindow(window_);
	}

	YumeVector<Vector2>::type YumeGLRenderer::GetScreenResolutions()
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

	void YumeGLRenderer::AddGpuResource(YumeGpuResource* gpuRes)
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		gpuResources_.push_back(gpuRes);
	}

	void YumeGLRenderer::RemoveGpuResource(YumeGpuResource* gpuRes)
	{
		boost::mutex::scoped_lock lock(gpuResourceMutex_);

		//Check if valid
		GpuResourceVector::iterator It = std::find(gpuResources_.begin(),gpuResources_.end(),gpuRes);

		if(It != gpuResources_.end())
			gpuResources_.erase(It);
	}
}