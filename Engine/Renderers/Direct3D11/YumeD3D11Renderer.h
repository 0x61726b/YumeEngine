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
#include "YumeD3D11ShaderProgram.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeD3D11RendererImpl;
	class YumeShaderVariation;
	class YumeD3D11ShaderProgram;
	class YumeTexture2D;
	class YumeTexture3D;
	class YumeTextureCube;

	typedef YumeMap<std::pair<YumeShaderVariation*,YumeShaderVariation*>,SharedPtr<YumeD3D11ShaderProgram> >::type ShaderProgramMap;

	class YumeD3DExport YumeD3D11Renderer : public YumeRHI,std::enable_shared_from_this<YumeD3D11Renderer>
	{
	public:
		YumeD3D11Renderer();
		virtual ~YumeD3D11Renderer();
		YumeD3D11RendererImpl* GetImpl() const { return impl_; }

		virtual bool							SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,int multiSample);

		virtual bool							BeginFrame();
		virtual void							EndFrame();
		virtual void							Clear(unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0);

		virtual bool							IsInitialized() { return initialized_; }



		/* Direct3D Only */
		bool									CreateD3D11Device(int width,int height,int multisample);
		bool									UpdateSwapchain(int width,int height);
		/* */


		virtual IntVector2						GetRenderTargetDimensions() const;
		virtual void							CreateRendererCapabilities();
		virtual YumeVector<int>::type			GetMultiSampleLevels() const;


		virtual void							Close();
		virtual void							AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		bool									OpenWindow(int width,int height,bool resizable,bool borderless);
		virtual void							Maximize();
		virtual void 							SetWindowPos(const Vector2& pos);
		virtual void							SetWindowTitle(const YumeString& string);

		virtual void							AddGpuResource(YumeGpuResource* object);
		virtual void							RemoveGpuResource(YumeGpuResource* object);


		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size);
		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size) { return 0; };

		virtual void							ResetCache();

		virtual bool							IsDeviceLost() const { return false; } //Always false on Direct3D


		virtual YumeVertexBuffer*				CreateVertexBuffer();
		virtual YumeIndexBuffer* 				CreateIndexBuffer();
		virtual YumeTexture2D*					CreateTexture2D();
		virtual YumeTexture3D*					CreateTexture3D();
		virtual YumeTextureCube*				CreateTextureCube();
		virtual YumeInputLayout* 				CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks);


		virtual YumeShaderVariation* 			GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const;
		virtual YumeShaderVariation* 			GetShader(ShaderType type,const char* name,const char* defines) const;

		virtual unsigned						GetFormat(CompressedFormat format) const;

		virtual void						    SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps);


		virtual bool							HasShaderParameter(YumeHash param);
		virtual void  							SetShaderParameter(YumeHash param,const float* data,unsigned count);
		virtual void  							SetShaderParameter(YumeHash param,float value);
		virtual void  							SetShaderParameter(YumeHash param,bool value);
		virtual void  							SetShaderParameter(YumeHash param,const YumeColor& color);
		virtual void  							SetShaderParameter(YumeHash param,const Vector2& vector);
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3& matrix);
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3x4& matrix);
		virtual void  							SetShaderParameter(YumeHash param,const Vector3& vector);
		virtual void  							SetShaderParameter(YumeHash param,const Matrix4& matrix);
		virtual void  							SetShaderParameter(YumeHash param,const Vector4& vector);
		virtual void  							SetShaderParameter(YumeHash param,const Variant& value);

		virtual void  							SetVertexBuffer(YumeVertexBuffer* buffer);
		virtual void  							SetIndexBuffer(YumeIndexBuffer* buffer);
		virtual bool  							SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);
		virtual bool  							SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);

		virtual bool							NeedParameterUpdate(ShaderParameterGroup group,const void* source);
		virtual void							SetFlushGPU(bool flushGpu);
		virtual void  							SetBlendMode(BlendMode mode);
		virtual void  							SetColorWrite(bool enable);
		virtual void  							SetCullMode(CullMode mode);
		virtual void  							SetDepthBias(float constantBias,float slopeScaledBias);
		virtual void  							SetDepthTest(CompareMode mode);
		virtual void  							SetDepthWrite(bool enable);
		virtual void  							SetFillMode(FillMode mode);
		virtual void  							SetScissorTest(bool enable,const Rect& rect = Rect::FULL,bool borderInclusive = true);
		virtual void  							SetScissorTest(bool enable,const IntRect& rect);
		virtual void  							SetStencilTest(bool enable,CompareMode mode = CMP_ALWAYS,StencilOp pass = OP_KEEP,StencilOp fail = OP_KEEP,StencilOp zFail = OP_KEEP,unsigned stencilRef = 0,unsigned compareMask = M_MAX_UNSIGNED,unsigned writeMask = M_MAX_UNSIGNED);


		virtual void  							SetClipPlane(bool enable,const Plane& clipPlane,const Matrix3x4& view,const Matrix4& projection);
		virtual void  							SetTexture(unsigned index,YumeTexture* texture);

		virtual void 							SetRenderTarget(unsigned index,YumeRenderable* renderTarget);
		virtual void 							SetDepthStencil(YumeRenderable* depthStencil);
		virtual void 							SetDepthStencil(YumeTexture2D* texture);

		virtual void 							SetViewport(const IntRect& rect);

		void									PreDraw();
		virtual void 							Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount);
		virtual void 							Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount);
		virtual void 							DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,unsigned instanceCount);
		virtual bool							ResolveToTexture(YumeTexture2D* destination, const IntRect& viewport);
		virtual void							ClearParameterSource(ShaderParameterGroup group);
		virtual void							ClearParameterSources();
		virtual void							ClearTransformSources();
		virtual void 							CleanupShaderPrograms(YumeShaderVariation* variation);

		virtual const Vector2&					GetPixelUVOffset() { return pixelUVOffset; }
		virtual bool							GetGL3SupportNs() { return false; } //kehkehkeh
		virtual unsigned						GetOpenGLOnlyTextureDataType(unsigned format) { return 0; };

		static unsigned							GetAlphaFormat();
		static unsigned							GetLuminanceFormat();
		static unsigned							GetLuminanceAlphaFormat();
		static unsigned							GetRGBFormat();
		static unsigned							GetRGBAFormat();
		static unsigned							GetRGBA16Format();
		static unsigned							GetRGBAFloat16Format();
		static unsigned							GetRGBAFloat32Format();
		static unsigned							GetRG16Format();
		static unsigned							GetRGFloat16Format();
		static unsigned							GetRGFloat32Format();
		static unsigned							GetFloat16Format();
		static unsigned							GetFloat32Format();
		static unsigned							GetLinearDepthFormat();
		static unsigned							GetDepthStencilFormat();
		static unsigned							GetReadableDepthFormat();

		//Non-static version
		unsigned								GetAlphaFormatNs();
		unsigned								GetLuminanceFormatNs();
		unsigned								GetLuminanceAlphaFormatNs();
		unsigned								GetRGBFormatNs();
		unsigned								GetRGBAFormatNs();
		unsigned								GetRGBA16FormatNs();
		unsigned								GetRGBAFloat16FormatNs();
		unsigned								GetRGBAFloat32FormatNs();
		unsigned								GetRG16FormatNs();
		unsigned								GetRGFloat16FormatNs();
		unsigned								GetRGFloat32FormatNs();
		unsigned								GetFloat16FormatNs();
		unsigned								GetFloat32FormatNs();
		unsigned								GetLinearDepthFormatNs();
		unsigned								GetDepthStencilFormatNs();
		unsigned								GetReadableDepthFormatNs();
		static unsigned							GetFormat(const YumeString& formatName);
	private:
		void RegisterFactories();
		void UnregisterFactories();
		void CreateResolveTexture();
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
