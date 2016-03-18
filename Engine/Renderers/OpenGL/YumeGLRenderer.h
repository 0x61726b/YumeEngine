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
#include "YumeGLRequired.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeVector2.h"
#include "Math/YumeVector4.h"

#include "Renderer/YumeRHI.h"

#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeGLRendererImpl;
	class YumeGLShaderProgram;
	class YumeTexture2D;
	class YumeTexture3D;
	class YumeTextureCube;

	typedef YumeMap<std::pair<YumeShaderVariation*,YumeShaderVariation*>,SharedPtr<YumeGLShaderProgram> >::type ShaderProgramMap;

	class YumeGLExport YumeGLRenderer : public YumeRHI
	{
	public:
		YumeGLRenderer();
		virtual ~YumeGLRenderer();

		virtual bool							SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,int multiSample);

		virtual bool							BeginFrame();
		virtual void							EndFrame();
		virtual void							Clear(unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0);

		virtual bool							IsInitialized();

		void									SetWindowIcon(YumeImage* image);

		virtual IntVector2						GetRenderTargetDimensions() const;
		virtual void							CreateRendererCapabilities();
		virtual YumeVector<int>::type			GetMultiSampleLevels() const;


		virtual void							Close();
		virtual void							AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		virtual void							Maximize();
		virtual void 							SetWindowPos(const Vector2& pos);
		virtual void							SetWindowTitle(const YumeString& string);

		virtual void							AddGpuResource(YumeGpuResource* object);
		virtual void							RemoveGpuResource(YumeGpuResource* object);


		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size) { return 0; } ;
		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size);

		virtual void							ResetCache();

		virtual bool							IsDeviceLost() const;


		virtual YumeTexture2D*					CreateTexture2D();
		virtual YumeTexture3D*					CreateTexture3D();
		virtual YumeTextureCube*				CreateTextureCube();
		virtual YumeVertexBuffer*				CreateVertexBuffer();
		virtual YumeIndexBuffer* 				CreateIndexBuffer();
		virtual YumeInputLayout* 				CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks) { return 0; };

		virtual YumeShaderVariation* 			GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const;
		virtual YumeShaderVariation* 			GetShader(ShaderType type,const char* name,const char* defines) const;

		virtual unsigned						GetFormat(CompressedFormat format) const;

		virtual void						    SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps);

		virtual void  							SetShaderParameter(YumeHash param,const float* data,unsigned count);
		virtual void  							SetShaderParameter(YumeHash param,float value);
		virtual void  							SetShaderParameter(YumeHash param,bool value) { };
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


		virtual void  							SetClipPlane(bool enable,const Plane& clipPlane = Plane::UP,const Matrix3x4& view =  Matrix3x4::IDENTITY,const Matrix4& projection =  Matrix4::IDENTITY);
		virtual void  							SetTexture(unsigned index,YumeTexture* texture);

		virtual void 							SetRenderTarget(unsigned index,YumeRenderable* renderTarget);
		virtual void 							SetDepthStencil(YumeRenderable* depthStencil);
		virtual void 							SetDepthStencil(YumeTexture2D* texture);

		virtual void 							SetViewport(const IntRect& rect);

		virtual void 							Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount);
		virtual void 							Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount);
		virtual void 							DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,unsigned instanceCount);

		virtual bool							ResolveToTexture(YumeTexture2D* destination,const IntRect& viewport);
		virtual void							ClearParameterSource(ShaderParameterGroup group);
		virtual void							ClearParameterSources();
		virtual void							ClearTransformSources();
		virtual void 							CleanupShaderPrograms(YumeShaderVariation* variation);

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


		virtual unsigned						GetAlphaFormatNs();
		virtual unsigned						GetLuminanceFormatNs();
		virtual unsigned						GetLuminanceAlphaFormatNs();
		virtual unsigned						GetRGBFormatNs();
		virtual unsigned						GetRGBAFormatNs();
		virtual unsigned						GetRGBA16FormatNs();
		virtual unsigned						GetRGBAFloat16FormatNs();
		virtual unsigned						GetRGBAFloat32FormatNs();
		virtual unsigned						GetRG16FormatNs();
		virtual unsigned						GetRGFloat16FormatNs();
		virtual unsigned						GetRGFloat32FormatNs();
		virtual unsigned						GetFloat16FormatNs();
		virtual unsigned						GetFloat32FormatNs();
		virtual unsigned						GetLinearDepthFormatNs();
		virtual unsigned						GetDepthStencilFormatNs();
		virtual unsigned						GetReadableDepthFormatNs();
		virtual unsigned						GetOpenGLOnlyTextureDataType(unsigned format);
		virtual bool							GetGL3SupportNs();

		virtual bool							HasShaderParameter(YumeHash param);

		void PreDraw();
		void SetTextureUnitMappings();
		void ResetRenderTargets();

		virtual const Vector2&					GetPixelUVOffset() { return pixelUVOffset; }

		void SetTextureForUpdate(YumeTexture* texture);
		void MarkFBODirty();
		void SetVBO(unsigned object);
		void SetUBO(unsigned object);
		static bool GetGL3Support();
		static unsigned GetMaxBones();

		void CleanupFramebuffers();
		unsigned CreateFramebuffer();
		void CleanupRenderable(YumeRenderable*);

		void DeleteFramebuffer(unsigned fbo);
		void BindFramebuffer(unsigned fbo);
		void BindColorAttachment(unsigned index,unsigned target,unsigned object);
		void BindDepthAttachment(unsigned object,bool isRenderBuffer);
		void BindStencilAttachment(unsigned object,bool isRenderBuffer);
		bool CheckFramebuffer();

		void Restore();
		void Release(bool clearGPUObjects,bool closeWindow);

		YumeGLShaderProgram* GetShaderProgram() const { return shaderProgram_; }
	private:
		void RegisterFactories();
		void UnregisterFactories();
	private:
		YumeVector<IntVector2>::type				GetScreenResolutions();
	private:
		bool									initialized_;
		YumeGLRendererImpl*						impl_;

		ShaderProgramMap						shaderPrograms_;
		YumeGLShaderProgram*					shaderProgram_;

		YumeConstantBuffer*						currentConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS * 2];
		unsigned								textureTypes_[MAX_TEXTURE_UNITS];
		YumeMap<int,SharedPtr<YumeTexture2D> >::type depthTextures_;

	protected:
		static bool gl3Support;
		bool forceGL2_;
		bool anisotropySupport_;
		bool dxtTextureSupport_;
		bool etcTextureSupport_;
		bool pvrtcTextureSupport_;
		unsigned lastInstanceOffset_;
	};
}


//----------------------------------------------------------------------------
#endif
