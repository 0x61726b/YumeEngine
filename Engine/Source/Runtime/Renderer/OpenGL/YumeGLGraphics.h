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
#ifndef __YumeGLGraphics_h__
#define __YumeGLGraphics_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeVector2.h"
#include "Math/YumeVector4.h"

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeRendererImpl;

	typedef std::vector<YumeGpuResource*> GpuResourceVector;
	class YumeAPIExport YumeGraphics
	{
	public:
		YumeGraphics();
		virtual ~YumeGraphics();

		static YumeGraphics* Get();

		bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample);

		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags, const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

		

		void ResetRenderTargets();
		void SetViewport(const Vector4&);

		Vector2 GetRenderTargetDimensions() const;

		void SetFlushGPU(bool flushGpu);
		void CreateRendererCapabilities();
		YumeVector<int>::type GetMultiSampleLevels() const;

		//Window ops
		void Close();
		void AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		void Maximize();
		void SetWindowPos(const Vector2& pos);
		void SetWindowTitle(const YumeString& string);

		void AddGpuResource(YumeGpuResource* object);
		void RemoveGpuResource(YumeGpuResource* object);
		void Restore();


		bool IsInitialized() const { return initialized_; }

		YumeRendererImpl* GetImpl() const { return impl_; }
	private:
		YumeVector<Vector2>::type				GetScreenResolutions();
	private:
		bool									initialized_;

		YumeString								windowTitle_;
		int										windowWidth_;
		int										windowHeight_;
		Vector2									windowPos_;
		bool									fullscreen_;
		bool									borderless_;
		bool									resizeable_;
		bool									tripleBuffer_;
		int										multiSample_;
		bool									vsync_;
		bool									flushGpu_;
		bool									forceGL2_;
		bool									gl3Support;
		YumeString								apiName_;
		bool									anisotropySupport_;
		bool									dxtTextureSupport_;
		bool									etcTextureSupport_;
		bool									pvrtcTextureSupport_;

		int										numPrimitives_;
		int										numBatches_;

		Vector4									viewport_;

		//Renderer features
		bool sRGB_;
		bool lightPrepassSupport_;
		bool deferredSupport_;
		bool hardwareShadowSupport_;
		bool instancingSupport_;
		bool sRGBSupport_;
		bool sRGBWriteSupport_;
		unsigned dummyColorFormat_;
		unsigned shadowMapFormat_;
		unsigned hiresShadowMapFormat_;

		bool renderTargetsDirty_;
		bool texturesDirty_;
		bool vertexDeclarationDirty_;
		bool blendStateDirty_;
		bool depthStateDirty_;
		bool rasterizerStateDirty_;
		bool scissorRectDirty_;
		bool stencilRefDirty_;


		boost::mutex							gpuResourceMutex_;
		GpuResourceVector						gpuResources_;

		YumeRendererImpl*						impl_;
	};
}


//----------------------------------------------------------------------------
#endif
