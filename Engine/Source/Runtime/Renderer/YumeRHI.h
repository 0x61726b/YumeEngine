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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeGraphicsInterface_h__
#define __YumeGraphicsInterface_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeRect.h"
#include "Math/YumeColor.h"
#include "Math/YumeVector4.h"
#include "Math/YumeMatrix3x4.h"
#include "Core/YumeVariant.h"
#include "Math/YumePlane.h"
#include "Renderer/YumeImage.h"

#include "Core/YumeEventHub.h"
#include "Core/YumeMutex.h"
#include "Renderer/YumeShaderVariation.h"
#include "Renderer/YumeInputLayout.h"
#include "Renderer/YumeConstantBuffer.h"


#include <SDL.h>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeRendererImpl;
	class YumeImage;
	class YumeShader;
	class YumeVertexBuffer;
	class YumeRenderable;
	class YumeTexture2D;
	class YumeTexture3D;
	class YumeTexture;
	class YumeIndexBuffer;
	class YumeTextureCube;
	class YumeRHI;

	struct ScratchBuffer
	{
		ScratchBuffer():
			size_(0),
			reserved_(false)
		{
		}


		boost::shared_array<unsigned char> data_;

		unsigned size_;

		bool reserved_;
	};

	typedef YumeVector<YumeGpuResource*> GpuResourceVector;

	class YumeAPIExport YumeRHI
	{
	public:
		YumeRHI();

		virtual ~YumeRHI();

		virtual bool							SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,int multiSample) = 0;

		virtual bool							BeginFrame() = 0;
		virtual void							EndFrame() = 0;
		virtual void							Clear(unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0) = 0;
		virtual void							ClearRenderTarget(unsigned index,unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0) = 0;

		virtual bool							IsInitialized() = 0;

		void									SetWindowIcon(YumeImage* image);

		virtual IntVector2						GetRenderTargetDimensions() const = 0;
		virtual void							CreateRendererCapabilities() = 0;
		virtual YumeVector<int>::type			GetMultiSampleLevels() const = 0;


		virtual void							Close() = 0;
		virtual void							AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless) = 0;
		virtual void							Maximize() = 0;
		virtual void 							SetWindowPos(const Vector2& pos) = 0;
		virtual void							SetWindowTitle(const YumeString& string) = 0;

		virtual void							AddGpuResource(YumeGpuResource* object) = 0;
		virtual void							RemoveGpuResource(YumeGpuResource* object) = 0;

		virtual void							BeginEvent(const YumeString& string) {};
		virtual void							EndEvent() {};


		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size) = 0;
		virtual YumeConstantBuffer*				GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size) = 0;

		virtual void							ResetCache() = 0;

		virtual bool							IsDeviceLost() const = 0;


		virtual YumeTexture2D*					CreateTexture2D() = 0;
		virtual YumeTexture3D*					CreateTexture3D() = 0;
		virtual YumeTextureCube*				CreateTextureCube() = 0;
		virtual YumeVertexBuffer*				CreateVertexBuffer() = 0;
		virtual YumeIndexBuffer* 				CreateIndexBuffer() = 0;
		virtual YumeInputLayout* 				CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks) = 0;


		YumeShaderVariation* 				    GetVertexShader() const { return vertexShader_; }
		YumeShaderVariation* 				    GetPixelShader() const { return pixelShader_; }

		YumeVertexBuffer* 					    GetVertexBuffer(unsigned index) const;
		YumeIndexBuffer* 				        GetIndexBuffer() const { return indexBuffer_; }

		virtual YumeShaderVariation* 			GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const = 0;
		virtual YumeShaderVariation* 			GetShader(ShaderType type,const char* name,const char* defines) const = 0;

		TextureFilterMode 						GetDefaultTextureFilterMode() const { return defaultTextureFilterMode_; }
		unsigned GetTextureAnisotropy()			const { return textureAnisotropy_; }
		virtual unsigned						GetFormat(CompressedFormat format) const = 0;
		unsigned int							GetFormatNs(const YumeString&);

		virtual void						    SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps) = 0;

		virtual void  							SetShaderParameter(YumeHash param,const float* data,unsigned count) = 0;
		virtual void  							SetShaderParameter(YumeHash param,float value) = 0;
		virtual void  							SetShaderParameter(YumeHash param,bool value) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const YumeColor& color) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Vector2& vector) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3& matrix) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Matrix3x4& matrix) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Vector3& vector) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Matrix4& matrix) = 0;
		virtual void  							SetShaderParameter(YumeHash param,const Vector4& vector) = 0;
		virtual void  							SetShaderParameter(YumeHash param,Vector4* vectorArray) { };
		virtual void  							SetShaderParameter(YumeHash param,const Variant& value) = 0;

		virtual void  							SetVertexBuffer(YumeVertexBuffer* buffer) = 0;
		virtual void  							SetIndexBuffer(YumeIndexBuffer* buffer) = 0;
		virtual bool  							SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0) = 0;
		virtual bool  							SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0) = 0;

		virtual bool							NeedParameterUpdate(ShaderParameterGroup group,const void* source) = 0;
		virtual void							SetFlushGPU(bool flushGpu) = 0;
		virtual void  							SetBlendMode(BlendMode mode) = 0;
		virtual void  							SetColorWrite(bool enable) = 0;
		virtual void  							SetCullMode(CullMode mode) = 0;
		virtual void  							SetDepthBias(float constantBias,float slopeScaledBias) = 0;
		virtual void  							SetDepthTest(CompareMode mode) = 0;
		virtual void  							SetDepthWrite(bool enable) = 0;
		virtual void  							SetFillMode(FillMode mode) = 0;
		virtual void  							SetScissorTest(bool enable,const Rect& rect = Rect::FULL,bool borderInclusive = true) = 0;
		virtual void  							SetScissorTest(bool enable,const IntRect& rect) = 0;
		virtual void  							SetStencilTest(bool enable,CompareMode mode = CMP_ALWAYS,StencilOp pass = OP_KEEP,StencilOp fail = OP_KEEP,StencilOp zFail = OP_KEEP,unsigned stencilRef = 0,unsigned compareMask = M_MAX_UNSIGNED,unsigned writeMask = M_MAX_UNSIGNED) = 0;


		virtual void  							SetClipPlane(bool enable,const Plane& clipPlane = Plane::UP,const Matrix3x4& view =  Matrix3x4::IDENTITY,const Matrix4& projection =  Matrix4::IDENTITY) = 0;
		virtual void  							SetTexture(unsigned index,YumeTexture* texture) = 0;

		virtual void 							SetRenderTarget(unsigned index,YumeRenderable* renderTarget) = 0;
		virtual void 							SetDepthStencil(YumeRenderable* depthStencil) = 0;
		virtual void 							SetDepthStencil(YumeTexture2D* texture) = 0;

		virtual void 							SetViewport(const IntRect& rect) = 0;

		virtual void 							Draw(PrimitiveType type,unsigned vertexStart,unsigned vertexCount) = 0;
		virtual void 							Draw(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount) = 0;
		virtual void 							DrawInstanced(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,unsigned instanceCount) = 0;

		virtual bool							ResolveToTexture(YumeTexture2D* destination,const IntRect& viewport) = 0;
		virtual void							ClearParameterSource(ShaderParameterGroup group) = 0;
		virtual void							ClearParameterSources() = 0;
		virtual void							ClearTransformSources() = 0;
		virtual void 							CleanupShaderPrograms(YumeShaderVariation* variation) = 0;


		virtual unsigned						GetAlphaFormatNs() = 0;
		virtual unsigned						GetLuminanceFormatNs() = 0;
		virtual unsigned						GetLuminanceAlphaFormatNs() = 0;
		virtual unsigned						GetRGBFormatNs() = 0;
		virtual unsigned						GetRGBAFormatNs() = 0;
		virtual unsigned						GetBGRAFormatNs() = 0;
		virtual unsigned						GetRGBA16FormatNs() = 0;
		virtual unsigned						GetRGBAFloat16FormatNs() = 0;
		virtual unsigned						GetRGBAFloat32FormatNs() = 0;
		virtual unsigned						GetRG16FormatNs() = 0;
		virtual unsigned						GetRGFloat16FormatNs() = 0;
		virtual unsigned						GetRGFloat32FormatNs() = 0;
		virtual unsigned						GetFloat16FormatNs() = 0;
		virtual unsigned						GetFloat32FormatNs() = 0;
		virtual unsigned						GetLinearDepthFormatNs() = 0;
		virtual unsigned						GetDepthStencilFormatNs() = 0;
		virtual unsigned						GetReadableDepthFormatNs() = 0;
		virtual unsigned						GetOpenGLOnlyTextureDataType(unsigned format) = 0;
		virtual bool							GetGL3SupportNs() = 0;
		virtual const Vector2&					GetPixelUVOffset() = 0;

		virtual bool							HasShaderParameter(YumeHash param) = 0;

		bool									ToggleFullscreen();

		void*									ReserveScratchBuffer(unsigned size);
		void									FreeScratchBuffer(void* buffer);
		void									CleanupScratchBuffers();

		void 									SetRenderTarget(unsigned index,YumeTexture2D* texture);
		void 									SetTextureAnisotropy(unsigned level);
		void									SetDefaultTextureFilterMode(TextureFilterMode mode);
		void 									SetTextureParametersDirty();

		void 									ResetRenderTargets();
		void								    ResetRenderTarget(unsigned index);
		void									ResetDepthStencil();

		bool									HasTextureUnit(TextureUnit unit);
		TextureUnit								GetTextureUnit(const YumeString& name);
		const YumeString&						GetTextureUnitName(TextureUnit unit);
		YumeTexture*							GetTexture(unsigned index) const;
		YumeRenderable*							GetRenderTarget(unsigned index) const;
		YumeRenderable*							GetDepthStencil() const { return depthStencil_; }
		IntRect									GetViewport() const { return viewport_; }
		BlendMode								GetBlendMode() const { return blendMode_; }

		SDL_Window*								GetWindow() const { return window_; }
		bool									GetFullscreen() const { return fullscreen_; }
		bool									GetResizable() const { return resizeable_; }
		bool									GetBorderless() const { return borderless_; }
		bool									GetVSync() const { return vsync_; }
		bool									GetTripleBuffer() const { return tripleBuffer_; }
		bool									GetSRGB() const { return sRGB_; }
		bool									GetFlushGPU() const { return flushGpu_; }
		unsigned								GetNumPrimitives() const { return numPrimitives_; }
		unsigned								GetNumBatches() const { return numBatches_; }
		int										GetWidth() const { return windowWidth_; }
		int										GetHeight() const { return windowHeight_; }
		unsigned								GetHiresShadowMapFormat() const { return hiresShadowMapFormat_; }
		bool									GetInstancingSupport() const { return instancingSupport_; }
		bool									GetLightPrepassSupport() const { return lightPrepassSupport_; }
		bool									GetDeferredSupport() const { return deferredSupport_; }
		bool									GetHardwareShadowSupport() const { return hardwareShadowSupport_; }
		bool									GetSRGBSupport() const { return sRGBSupport_; }
		bool									GetSRGBWriteSupport() const { return sRGBWriteSupport_; }
		const bool								GetAnisotropySupport() const { return anisotropySupport_; }
		float									GetDepthConstantBias() const { return constantDepthBias_; }
		float									GetDepthSlopeScaledBias() const { return slopeScaledDepthBias_; }
		CompareMode								GetDepthTest() const { return depthTestMode_; }
		bool									GetDepthWrite() const { return depthWrite_; }
		FillMode								GetFillMode() const { return fillMode_; }
		bool									GetStencilTest() const { return stencilTest_; }
		bool									GetScissorTest() const { return scissorTest_; }
		CompareMode								GetStencilTestMode() const { return stencilTestMode_; }
		StencilOp								GetStencilPass() const { return stencilPass_; }
		StencilOp								GetStencilFail() const { return stencilFail_; }
		StencilOp								GetStencilZFail() const { return stencilZFail_; }
		int										GetMultiSample() const { return multiSample_; }
		unsigned								GetShadowMapFormat() const { return shadowMapFormat_; }
		unsigned								GetDummyColorFormat() const { return dummyColorFormat_; }

	protected:
		void CreateWindowIcon();
	protected:
		SDL_Window* window_;

		YumeImage*	windowIcon_;

		typedef YumeVector<RHIEventListener*>::type RHIEventListeners;
		RHIEventListeners listeners_;
	public:
		void AddListener(RHIEventListener*);
		void RemoveListener(RHIEventListener*);
	protected:
		YumeString								windowTitle_;
		int										windowWidth_;
		int										windowHeight_;
		IntVector2								windowPos_;
		bool									fullscreen_;
		bool									borderless_;
		bool									resizeable_;
		bool									tripleBuffer_;
		int										multiSample_;
		bool									vsync_;
		bool									flushGpu_;

		int										numPrimitives_;
		int										numBatches_;

	protected:
		Mutex							gpuResourceMutex_;
		GpuResourceVector::type			gpuResources_;


		bool sRGB_;
		bool lightPrepassSupport_;
		bool deferredSupport_;
		bool hardwareShadowSupport_;
		bool instancingSupport_;
		bool sRGBSupport_;
		bool sRGBWriteSupport_;
		bool anisotropySupport_;
		unsigned maxScratchBufferRequest_;
		YumeVector<ScratchBuffer>::type scratchBuffers_;
		unsigned dummyColorFormat_;
		unsigned shadowMapFormat_;
		unsigned hiresShadowMapFormat_;
		YumeVertexBuffer* vertexBuffers_[MAX_VERTEX_STREAMS];
		unsigned elementMasks_[MAX_VERTEX_STREAMS];
		YumeIndexBuffer* indexBuffer_;
		unsigned long long vertexDeclarationHash_;
		unsigned primitiveType_;
		YumeShaderVariation* vertexShader_;
		YumeShaderVariation* pixelShader_;
		YumeTexture* textures_[MAX_TEXTURE_UNITS];
		YumeMap<YumeString,TextureUnit>::type textureUnits_;
		YumeRenderable* renderTargets_[MAX_RENDERTARGETS];
		YumeRenderable* depthStencil_;
		IntRect viewport_;
		unsigned textureAnisotropy_;
		BlendMode blendMode_;
		bool colorWrite_;
		CullMode cullMode_;
		float constantDepthBias_;
		float slopeScaledDepthBias_;
		CompareMode depthTestMode_;
		bool depthWrite_;
		FillMode fillMode_;
		IntRect scissorRect_;
		bool scissorTest_;
		CompareMode stencilTestMode_;
		StencilOp stencilPass_;
		StencilOp stencilFail_;
		StencilOp stencilZFail_;
		unsigned stencilRef_;
		unsigned stencilCompareMask_;
		unsigned stencilWriteMask_;
		Vector4 clipPlane_;
		bool stencilTest_;
		bool useClipPlane_;
		bool renderTargetsDirty_;
		bool texturesDirty_;
		bool vertexDeclarationDirty_;
		bool blendStateDirty_;
		bool depthStateDirty_;
		bool rasterizerStateDirty_;
		bool scissorRectDirty_;
		bool stencilRefDirty_;
		unsigned blendStateHash_;
		unsigned depthStateHash_;
		unsigned rasterizerStateHash_;
		unsigned firstDirtyTexture_;
		unsigned lastDirtyTexture_;
		unsigned firstDirtyVB_;
		unsigned lastDirtyVB_;
		TextureFilterMode defaultTextureFilterMode_;
		YumeMap<unsigned long long,SharedPtr<YumeInputLayout> >::type vertexDeclarations_;
		YumeMap<unsigned,SharedPtr<YumeConstantBuffer> >::type constantBuffers_;
		YumeVector<YumeConstantBuffer*>::type dirtyConstantBuffers_;
		const void* shaderParameterSources_[MAX_SHADER_PARAMETER_GROUPS];
		YumeString shaderPath_;
		YumeString shaderExtension_;
		mutable SharedPtr<YumeShader> lastShader_;
		mutable YumeString lastShaderName_;
		//SharedPtr<ShaderPrecache> shaderPrecache_;
		YumeString orientations_;
		YumeString apiName_;

		const Vector2 pixelUVOffset;
	};


	class RHIEvent
	{
	public:
		RHIEvent(const YumeString& event);
		~RHIEvent();
	};
}


//----------------------------------------------------------------------------
#endif
