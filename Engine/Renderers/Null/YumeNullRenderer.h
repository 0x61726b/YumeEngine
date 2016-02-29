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
#ifndef __YumeNullRenderer_h__
#define __YumeNullRenderer_h__
//----------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include "Renderer/YumeRHI.h"

#	if YUME_PLATFORM == YUME_PLATFORM_WIN32
#		if defined(BUILDING_YUME_NULL)
#			define YumeNullExport __declspec( dllexport )
#		else
#			define YumeNullExport  __declspec( dllimport )
#		endif
#	else
#	define YumeNullExport 	
#	endif

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeNullExport YumeNullRenderer : public YumeRHI
	{
	public:
		YumeNullRenderer();
		~YumeNullRenderer();


		bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample);

		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags,const Vector4& color = Vector4(0.0f,1.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0);

		bool CreateD3D11Device(int width,int height,int multisample);
		bool UpdateSwapchain(int width,int height);

		void ResetRenderTargets();
		void SetViewport(const IntRect&);

		Vector2 GetRenderTargetDimensions() const;

		void SetFlushGPU(bool flushGpu);
		void CreateRendererCapabilities();
		YumeVector<int>::type GetMultiSampleLevels() const;

		YumeShaderVariation* GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const { return 0; }

		YumeShaderVariation* GetShader(ShaderType type,const char* name,const char* defines) const { return 0; }

		void SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps) {}

		//Window ops
		void Close();
		void AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		void Maximize();
		void SetWindowPos(const Vector2& pos);
		void SetWindowTitle(const YumeString& string);

		void AddGpuResource(YumeGpuResource* object);
		void RemoveGpuResource(YumeGpuResource* object);
		void ResetCache() { };

		YumeVertexBuffer* GetVertexBuffer(unsigned index) const { return 0; }
		YumeConstantBuffer* GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size) { return 0; }
		
		void SetShaderParameter(YumeHash  param,const float* data,unsigned count) { };
		/// Set shader float constant.
		void SetShaderParameter(YumeHash  param,float value) { };
		/// Set shader boolean constant.
		void SetShaderParameter(YumeHash  param,bool value) { };
		/// Set shader color constant.
		void SetShaderParameter(YumeHash  param,const YumeColor& color) { };
		/// Set shader 2D vector constant.
		void SetShaderParameter(YumeHash  param,const Vector2& vector) { };
		/// Set shader 3x3 matrix constant.
		void SetShaderParameter(YumeHash  param,const Matrix3& matrix) { };
		/// Set shader 3D vector constant.
		void SetShaderParameter(YumeHash  param,const Vector3& vector) { };
		/// Set shader 4x4 matrix constant.
		void SetShaderParameter(YumeHash  param,const Matrix4& matrix) { };
		/// Set shader 4D vector constant.
		void SetShaderParameter(YumeHash param,const Vector4& vector) { };

		void SetShaderParameter(YumeHash param,const YumeVariant& value) { };


		//Setters
		void SetVertexBuffer(YumeVertexBuffer* buffer) { }
		/// Set multiple vertex buffers.
		bool SetVertexBuffers
			(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0) {
			return false;
		}
		/// Set multiple vertex buffers.
		bool SetVertexBuffers
			(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0) {
			return false;
		}
	};
}


//----------------------------------------------------------------------------
#endif
