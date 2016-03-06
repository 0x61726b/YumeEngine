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
#ifndef __YumeD3D11Graphics_h__
#define __YumeD3D11Graphics_h__
//----------------------------------------------------------------------------
#include "YumeD3D11Required.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeRect.h"
#include "Math/YumeColor.h"
#include "Math/YumeVector2.h"
#include "Math/YumeVector3.h"
#include "Math/YumeVector4.h"

#include "Math/YumeMatrix3x4.h"

#include "Renderer/YumeRHI.h"

#include "Renderer/YumeShader.h"

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeD3D11RendererImpl;
	class YumeShaderVariation;
	class YumeD3D11ShaderProgram;

	typedef YumeMap<std::pair<YumeShaderVariation*,YumeShaderVariation*>,SharedPtr<YumeD3D11ShaderProgram> >::type ShaderProgramMap;

	class YumeD3DExport YumeD3D11Renderer : public YumeRHI
	{
	public:
		YumeD3D11Renderer();
		~YumeD3D11Renderer();

		bool IsInitialized() const { return initialized_; }

		YumeD3D11RendererImpl* GetImpl() const { return impl_; }

		bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,int multiSample);


		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags,const Vector4& color = Vector4(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0);

		/* Direct3D Only */
		bool CreateD3D11Device(int width,int height,int multisample);
		bool UpdateSwapchain(int width,int height);
		/* */


		void SetViewport(const IntRect& rect);
		void SetViewport(const IntRect&);
		IntVector2 GetRenderTargetDimensions() const;
		void CreateRendererCapabilities();
		YumeVector<int>::type GetMultiSampleLevels() const;

		//Window ops
		bool OpenWindow(int width,int height,bool resizable,bool borderless);
		void Close();
		void AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		void Maximize();
		void SetWindowPos(const Vector2& pos);
		void SetWindowTitle(const YumeString& string);


		/* */
		void AddGpuResource(YumeGpuResource* object);
		void RemoveGpuResource(YumeGpuResource* object);
		/* */
		void ResetCache();


		YumeVertexBuffer* CreateVertexBuffer();
		YumeIndexBuffer* CreateIndexBuffer();
		YumeInputLayout* CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks);
		YumeConstantBuffer* GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size);


		YumeShaderVariation* GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const;
		YumeShaderVariation* GetShader(ShaderType type,const char* name,const char* defines) const;
		void SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps);

		YumeVertexBuffer* GetVertexBuffer(unsigned index) const;
		void CleanUpShaderPrograms(YumeShaderVariation* variation);
		unsigned GetFormat(CompressedFormat format) const;




		void SetShaderParameter(YumeHash  param,const float* data,unsigned count);
		void SetShaderParameter(YumeHash  param,float value);
		void SetShaderParameter(YumeHash  param,bool value);
		void SetShaderParameter(YumeHash  param,const YumeColor& color);
		void SetShaderParameter(YumeHash  param,const Vector2& vector);
		void SetShaderParameter(YumeHash  param,const Matrix3& matrix);
		void SetShaderParameter(YumeHash  param,const Matrix3x4& matrix);
		void SetShaderParameter(YumeHash  param,const Vector3& vector);
		void SetShaderParameter(YumeHash  param,const Matrix4& matrix);
		void SetShaderParameter(YumeHash param,const Vector4& vector);
		void SetShaderParameter(YumeHash param,const YumeVariant& value);


		void SetVertexBuffer(YumeVertexBuffer* buffer);
		void SetIndexBuffer(YumeIndexBuffer* buffer);
		bool SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);
		bool SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);
		void SetTexture(unsigned index,YumeTexture* texture);

		void SetBlendMode(BlendMode mode);
		void SetColorWrite(bool enable);
		void SetCullMode(CullMode mode);
		void SetDepthBias(float constantBias,float slopeScaledBias);
		void SetDepthTest(CompareMode mode);
		void SetDepthWrite(bool enable);
		void SetFillMode(FillMode mode);
		void SetScissorTest(bool enable,const Rect& rect = Rect::FULL,bool borderInclusive = true);
		void SetScissorTest(bool enable,const IntRect& rect);
		void SetStencilTest
			(bool enable,CompareMode mode = CMP_ALWAYS,StencilOp pass = OP_KEEP,StencilOp fail = OP_KEEP,StencilOp zFail = OP_KEEP,
			unsigned stencilRef = 0,unsigned compareMask = M_MAX_UNSIGNED,unsigned writeMask = M_MAX_UNSIGNED);
		void SetFlushGPU(bool flushGpu);

		void SetRenderTarget(unsigned index,YumeRenderable* renderTarget);
		void SetDepthStencil(YumeTexture2D* texture);
		void SetDepthStencil(YumeRenderable* depthStencil);

		void PreDraw();

		void Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount);
		void Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount);
		void DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,
			unsigned instanceCount);

		static unsigned GetAlphaFormat();
		static unsigned GetLuminanceFormat();
		static unsigned GetLuminanceAlphaFormat();
		static unsigned GetRGBFormat();
		static unsigned GetRGBAFormat();
		static unsigned GetRGBA16Format();
		static unsigned GetRGBAFloat16Format();
		static unsigned GetRGBAFloat32Format();
		static unsigned GetRG16Format();
		static unsigned GetRGFloat16Format();
		static unsigned GetRGFloat32Format();
		static unsigned GetFloat16Format();
		static unsigned GetFloat32Format();
		static unsigned GetLinearDepthFormat();
		static unsigned GetDepthStencilFormat();
		static unsigned GetReadableDepthFormat();
		static unsigned GetFormat(const YumeString& formatName);
	private:
		void RegisterFactories();
		void UnregisterFactories();
	private:
		YumeVector<Vector2>::type				GetScreenResolutions();
	private:
		bool									initialized_;


		YumeD3D11RendererImpl*					impl_;
		ShaderProgramMap						shaderPrograms_;
		YumeD3D11ShaderProgram*					shaderProgram_;
	};
}


//----------------------------------------------------------------------------
#endif
