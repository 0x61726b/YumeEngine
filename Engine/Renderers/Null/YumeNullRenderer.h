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
	class YumeTexture;

	class YumeNullExport YumeNullRenderer : public YumeRHI
	{
	public:
		YumeNullRenderer();
		~YumeNullRenderer();


		virtual bool							SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,int multiSample) { return true; };

		virtual bool							BeginFrame() { return true; };
		virtual void							EndFrame() { };
		virtual void							Clear(unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0) { };

		virtual bool							IsInitialized() { return true; }


		virtual IntVector2						GetRenderTargetDimensions() const { return IntVector2(0,0); }
		virtual void							CreateRendererCapabilities()  { };
		virtual YumeVector<int>::type			GetMultiSampleLevels() const { YumeVector<int>::type a; return a; };


		virtual void							Close() { };
		virtual void							AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless) { };
		bool									OpenWindow(int width,int height,bool resizable,bool borderless){ return true; };
		virtual void							Maximize() { };
		virtual void 							SetWindowPos(const Vector2& pos) { };
		virtual void							SetWindowTitle(const YumeString& string) { };

		virtual void							AddGpuResource(YumeGpuResource* object) { };
		virtual void							RemoveGpuResource(YumeGpuResource* object) { };


		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size) { return 0; };
		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size) { return 0; }

		virtual void							ResetCache() { };

		virtual bool							IsDeviceLost() const { return false; } //Always false on Direct3D


		virtual YumeVertexBuffer*				CreateVertexBuffer() { return 0; };;
		virtual YumeIndexBuffer* 				CreateIndexBuffer() { return 0; };;
		virtual YumeInputLayout* 				CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks) { return 0; };;


		virtual YumeShaderVariation* 			GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const { return 0; };;
		virtual YumeShaderVariation* 			GetShader(ShaderType type,const char* name,const char* defines) const { return 0; };;

		virtual unsigned						GetFormat(CompressedFormat format) const  { return 0; };

		virtual void						    SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps) { };



		virtual void  							SetShaderParameter(YumeHash param,const float* data,unsigned count) { }
		virtual void  							SetShaderParameter(YumeHash param,float value) { };
		virtual void  							SetShaderParameter(YumeHash param,bool value) { };
		virtual void  							SetShaderParameter(YumeHash param,const YumeColor& color) { };
		virtual void  							SetShaderParameter(YumeHash param,const Vector2& vector) { };
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3& matrix) { };
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3x4& matrix) { };
		virtual void  							SetShaderParameter(YumeHash param,const Vector3& vector) { };
		virtual void  							SetShaderParameter(YumeHash param,const Matrix4& matrix) { };
		virtual void  							SetShaderParameter(YumeHash param,const Vector4& vector) { };
		virtual void  							SetShaderParameter(YumeHash param,const Variant& value) { };

		virtual void  							SetVertexBuffer(YumeVertexBuffer* buffer) { };
		virtual void  							SetIndexBuffer(YumeIndexBuffer* buffer) { };
		virtual bool  							SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0){ return true; };
		virtual bool  							SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0){ return true; };

		virtual bool							NeedParameterUpdate(ShaderParameterGroup group,const void* source){ return true; };
		virtual void							SetFlushGPU(bool flushGpu) { };
		virtual void  							SetBlendMode(BlendMode mode) { };
		virtual void  							SetColorWrite(bool enable) { };
		virtual void  							SetCullMode(CullMode mode) { };
		virtual void  							SetDepthBias(float constantBias,float slopeScaledBias) { };
		virtual void  							SetDepthTest(CompareMode mode) { };
		virtual void  							SetDepthWrite(bool enable) { };
		virtual void  							SetFillMode(FillMode mode) { };
		virtual void  							SetScissorTest(bool enable,const Rect& rect = Rect::FULL,bool borderInclusive = true) { };
		virtual void  							SetScissorTest(bool enable,const IntRect& rect) { };
		virtual void  							SetStencilTest(bool enable,CompareMode mode = CMP_ALWAYS,StencilOp pass = OP_KEEP,StencilOp fail = OP_KEEP,StencilOp zFail = OP_KEEP,unsigned stencilRef = 0,unsigned compareMask = M_MAX_UNSIGNED,unsigned writeMask = M_MAX_UNSIGNED) { };


		virtual void  							SetClipPlane(bool enable,const Plane& clipPlane,const Matrix3x4& view,const Matrix4& projection) { };
		virtual void  							SetTexture(unsigned index,YumeTexture* texture) { };

		virtual void 							SetRenderTarget(unsigned index,YumeRenderable* renderTarget) { };
		virtual void 							SetDepthStencil(YumeRenderable* depthStencil) { };
		virtual void 							SetDepthStencil(YumeTexture2D* texture) { };

		virtual void 							SetViewport(const IntRect& rect) { };

		virtual void 							Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount) { };
		virtual void 							Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount) { };
		virtual void 							DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,unsigned instanceCount) { };

		virtual void							ClearParameterSource(ShaderParameterGroup group) { };
		virtual void							ClearParameterSources() { };
		virtual void							ClearTransformSources() { };
		virtual void 							CleanupShaderPrograms(YumeShaderVariation* variation) { };

	};
}


//----------------------------------------------------------------------------
#endif
