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
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeRenderPass_h__
#define __YumeRenderPass_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "YumeRendererDefs.h"
#include "YumeResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeShaderVariation;

	/// Lighting mode of a pass.
	enum PassLightingMode
	{
		LIGHTING_UNLIT,
		LIGHTING_PERVERTEX,
		LIGHTING_PERPIXEL
	};


	class YumeAPIExport YumeRenderPass
	{
	public:
		/// Construct.
		YumeRenderPass(const YumeString& passName);
		/// Destruct.
		~YumeRenderPass();

		/// Set blend mode.
		void SetBlendMode(BlendMode mode);
		/// Set depth compare mode.
		void SetDepthTestMode(CompareMode mode);
		/// Set pass lighting mode, affects what shader variations will be attempted to be loaded.
		void SetLightingMode(PassLightingMode mode);
		/// Set depth write on/off.
		void SetDepthWrite(bool enable);
		/// Set alpha masking hint. Completely opaque draw calls will be performed before alpha masked.
		void SetAlphaMask(bool enable);
		/// Set whether requires desktop level hardware.
		void SetIsDesktop(bool enable);
		/// Set vertex shader name.
		void SetVertexShader(const YumeString& name);
		/// Set pixel shader name.
		void SetPixelShader(const YumeString& name);
		/// Set vertex shader defines.
		void SetVertexShaderDefines(const YumeString& defines);
		/// Set pixel shader defines.
		void SetPixelShaderDefines(const YumeString& defines);
		/// Reset shader pointers.
		void ReleaseShaders();
		/// Mark shaders loaded this frame.
		void MarkShadersLoaded(unsigned frameNumber);

		/// Return pass name.
		const YumeString& GetName() const { return name_; }

		/// Return pass index. This is used for optimal render-time pass queries that avoid map lookups.
		unsigned GetIndex() const { return index_; }

		/// Return blend mode.
		BlendMode GetBlendMode() const { return blendMode_; }

		/// Return depth compare mode.
		CompareMode GetDepthTestMode() const { return depthTestMode_; }

		/// Return pass lighting mode.
		PassLightingMode GetLightingMode() const { return lightingMode_; }

		/// Return last shaders loaded frame number.
		unsigned GetShadersLoadedFrameNumber() const { return shadersLoadedFrameNumber_; }

		/// Return depth write mode.
		bool GetDepthWrite() const { return depthWrite_; }

		/// Return alpha masking hint.
		bool GetAlphaMask() const { return alphaMask_; }

		/// Return whether requires desktop level hardware.
		bool IsDesktop() const { return isDesktop_; }

		/// Return vertex shader name.
		const YumeString& GetVertexShader() const { return vertexShaderName_; }

		/// Return pixel shader name.
		const YumeString& GetPixelShader() const { return pixelShaderName_; }

		/// Return vertex shader defines.
		const YumeString& GetVertexShaderDefines() const { return vertexShaderDefines_; }

		/// Return pixel shader defines.
		const YumeString& GetPixelShaderDefines() const { return pixelShaderDefines_; }

		/// Return vertex shaders.
		YumeVector<SharedPtr<YumeShaderVariation> >::type& GetVertexShaders() { return vertexShaders_; }

		/// Return pixel shaders.
		YumeVector<SharedPtr<YumeShaderVariation> >::type& GetPixelShaders() { return pixelShaders_; }

	private:
		/// Pass index.
		unsigned index_;
		/// Blend mode.
		BlendMode blendMode_;
		/// Depth compare mode.
		CompareMode depthTestMode_;
		/// Lighting mode.
		PassLightingMode lightingMode_;
		/// Last shaders loaded frame number.
		unsigned shadersLoadedFrameNumber_;
		/// Depth write mode.
		bool depthWrite_;
		/// Alpha masking hint.
		bool alphaMask_;
		/// Require desktop level hardware flag.
		bool isDesktop_;
		/// Vertex shader name.
		YumeString vertexShaderName_;
		/// Pixel shader name.
		YumeString pixelShaderName_;
		/// Vertex shader defines.
		YumeString vertexShaderDefines_;
		/// Pixel shader defines.
		YumeString pixelShaderDefines_;
		/// Vertex shaders.
		YumeVector<SharedPtr<YumeShaderVariation> >::type vertexShaders_;
		/// Pixel shaders.
		YumeVector<SharedPtr<YumeShaderVariation> >::type pixelShaders_;
		/// Pass name.
		YumeString name_;
	};

	class YumeAPIExport YumeRenderTechnique : public YumeResource
	{
		friend class YumeRenderer;

	public:
		/// Construct.
		YumeRenderTechnique();
		/// Destruct.
		~YumeRenderTechnique();

		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);

		/// Set whether requires desktop level hardware.
		void SetIsDesktop(bool enable);
		/// Create a new pass.
		YumeRenderPass* CreatePass(const YumeString& passName);
		/// Remove a pass.
		void RemovePass(const YumeString& passName);
		/// Reset shader pointers in all passes.
		void ReleaseShaders();

		/// Return whether requires desktop level hardware.
		bool IsDesktop() const { return isDesktop_; }

		/// Return whether technique is supported by the current hardware.
		bool IsSupported() const { return !isDesktop_ || desktopSupport_; }

		/// Return whether has a pass.
		bool HasPass(unsigned passIndex) const { return passIndex < passes_.size() && passes_[passIndex].get() != 0; }

		/// Return whether has a pass by name. This overload should not be called in time-critical rendering loops; use a pre-acquired pass index instead.
		bool HasPass(const YumeString& passName) const;

		/// Return a pass, or null if not found.
		YumeRenderPass* GetPass(unsigned passIndex) const { return passIndex < passes_.size() ? passes_[passIndex].get() : 0; }

		/// Return a pass by name, or null if not found. This overload should not be called in time-critical rendering loops; use a pre-acquired pass index instead.
		YumeRenderPass* GetPass(const YumeString& passName) const;

		/// Return a pass that is supported for rendering, or null if not found.
		YumeRenderPass* GetSupportedPass(unsigned passIndex) const
		{
			YumeRenderPass* pass = passIndex < passes_.size() ? passes_[passIndex].get() : 0;
			return pass && (!pass->IsDesktop() || desktopSupport_) ? pass : 0;
		}

		/// Return a supported pass by name. This overload should not be called in time-critical rendering loops; use a pre-acquired pass index instead.
		YumeRenderPass* GetSupportedPass(const YumeString& passName) const;

		/// Return number of passes.
		unsigned GetNumPasses() const;
		/// Return all pass names.
		YumeVector<YumeString>::type GetPassNames() const;
		/// Return all passes.
		YumeVector<YumeRenderPass*>::type GetPasses() const;

		/// Return a pass type index by name. Allocate new if not used yet.
		static unsigned GetPassIndex(const YumeString& passName);

		/// Index for base pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned basePassIndex;
		/// Index for alpha pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned alphaPassIndex;
		/// Index for prepass material pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned materialPassIndex;
		/// Index for deferred G-buffer pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned deferredPassIndex;
		/// Index for per-pixel light pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned lightPassIndex;
		/// Index for lit base pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned litBasePassIndex;
		/// Index for lit alpha pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned litAlphaPassIndex;
		/// Index for shadow pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned shadowPassIndex;


	public:
		static YumeHash GetType();
		static YumeHash techHash_;

	private:
		/// Require desktop GPU flag.
		bool isDesktop_;
		/// Cached desktop GPU support flag.
		bool desktopSupport_;
		/// Passes.
		YumeVector<SharedPtr<YumeRenderPass> >::type passes_;

		/// Pass index assignments.
		static YumeMap<YumeString,unsigned>::type passIndices;
	};
}


//----------------------------------------------------------------------------
#endif
