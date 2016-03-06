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

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeGLRendererImpl;
	class YumeGLShaderProgram;
	class YumeTexture2D;

	typedef YumeMap<std::pair<YumeShaderVariation*,YumeShaderVariation*>,SharedPtr<YumeGLShaderProgram> >::type ShaderProgramMap;

	class YumeGLExport YumeGLRenderer : public YumeRHI
	{
	public:
		YumeGLRenderer();
		virtual ~YumeGLRenderer();

		bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample);

		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags,const YumeColor& color = YumeColor(0.0f,0.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0);

		bool IsInitialized();

		void ResetRenderTargets();
		void SetViewport(const IntRect&);

		IntVector2 GetRenderTargetDimensions() const;

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
		void ResetCache();
		void Release(bool clearGPUObjects,bool closeWindow);

		bool IsDeviceLost() const;
		YumeGLShaderProgram* GetShaderProgram() const { return shaderProgram_; }

		void MarkFBODirty();
		void SetVBO(unsigned object);
		void SetUBO(unsigned object);


		YumeVertexBuffer* CreateVertexBuffer();
		YumeIndexBuffer* CreateIndexBuffer();
		YumeInputLayout* CreateInputLayout(YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks) { return 0; }
		YumeConstantBuffer* GetOrCreateConstantBuffer(ShaderType type,unsigned index,unsigned size) { return 0; };
		YumeConstantBuffer* GetOrCreateConstantBuffer(unsigned bindingIndex,unsigned size);


		YumeShaderVariation* GetShader(ShaderType type,const YumeString& name,const YumeString& defines = "") const;
		YumeShaderVariation* GetShader(ShaderType type,const char* name,const char* defines) const;
		void SetShaders(YumeShaderVariation* vs,YumeShaderVariation* ps);

		YumeVertexBuffer* GetVertexBuffer(unsigned index) const;
		void CleanUpShaderPrograms(YumeShaderVariation* variation);
		unsigned GetFormat(CompressedFormat format) const;
		void CleanupShaderPrograms(YumeShaderVariation* variation);


		void SetShaderParameter(YumeHash param,const float* data,unsigned count);
		void SetShaderParameter(YumeHash param,float value);
		void SetShaderParameter(YumeHash param,bool value) { };
		void SetShaderParameter(YumeHash param,const YumeColor& color);
		void SetShaderParameter(YumeHash param,const Vector2& vector);
		void SetShaderParameter(YumeHash param,const Matrix3& matrix);
		void SetShaderParameter(YumeHash param,const Matrix3x4& matrix);
		void SetShaderParameter(YumeHash param,const Vector3& vector);
		void SetShaderParameter(YumeHash param,const Matrix4& matrix);
		void SetShaderParameter(YumeHash param,const Vector4& vector);
		void SetShaderParameter(YumeHash param,const YumeVariant& value);


		void SetVertexBuffer(YumeVertexBuffer* buffer);
		void SetIndexBuffer(YumeIndexBuffer* buffer);
		bool SetVertexBuffers(const YumeVector<YumeVertexBuffer*>::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);
		bool SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& elementMasks,unsigned instanceOffset = 0);
		void SetTexture(unsigned index,YumeTexture* texture);
		void SetTextureUnitMappings();

		void SetBlendMode(BlendMode mode);
		void SetColorWrite(bool enable);
		void SetCullMode(CullMode mode);
		void SetDepthBias(float constantBias,float slopeScaledBias);
		void SetDepthTest(CompareMode mode);
		void SetDepthWrite(bool enable);
		void SetFillMode(FillMode mode);
		void SetScissorTest(bool enable,const Rect& rect = Rect::FULL,bool borderInclusive = true);
		void SetScissorTest(bool enable,const IntRect& rect);
		void SetStencilTest(bool enable,CompareMode mode = CMP_ALWAYS,StencilOp pass = OP_KEEP,StencilOp fail = OP_KEEP,StencilOp zFail = OP_KEEP,unsigned stencilRef = 0,unsigned compareMask = M_MAX_UNSIGNED,unsigned writeMask = M_MAX_UNSIGNED);
		void SetClipPlane(bool enable,const Plane& clipPlane,const Matrix3x4& view,const Matrix4& projection);

		void SetTextureForUpdate(YumeTexture* texture);

		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source);

		void ClearParameterSource(ShaderParameterGroup group);
		void ClearParameterSources();
		void ClearTransformSources();

		void SetRenderTarget(unsigned index,YumeRenderable* renderTarget);
		void SetDepthStencil(YumeTexture2D* texture);
		void SetDepthStencil(YumeRenderable* depthStencil);

		void SetFlushGPU(bool flushGpu);

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

		const bool GetAnisotropySupport() const { return anisotropySupport_; }

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
		bool sRGBWriteSupport_;
		bool pvrtcTextureSupport_;
		unsigned lastInstanceOffset_;
	};
}


//----------------------------------------------------------------------------
#endif
