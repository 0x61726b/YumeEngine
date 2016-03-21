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


	enum PassLightingMode
	{
		LIGHTING_UNLIT,
		LIGHTING_PERVERTEX,
		LIGHTING_PERPIXEL
	};


	class YumeAPIExport YumeRenderPass : public RefCounted
	{
	public:

		YumeRenderPass(const YumeString& passName);

		~YumeRenderPass();




		void SetBlendMode(BlendMode mode);

		void SetDepthTestMode(CompareMode mode);

		void SetLightingMode(PassLightingMode mode);

		void SetDepthWrite(bool enable);

		void SetAlphaMask(bool enable);

		void SetIsDesktop(bool enable);

		void SetVertexShader(const YumeString& name);

		void SetPixelShader(const YumeString& name);

		void SetVertexShaderDefines(const YumeString& defines);

		void SetPixelShaderDefines(const YumeString& defines);

		void ReleaseShaders();

		void MarkShadersLoaded(unsigned frameNumber);


		const YumeString& GetName() const { return name_; }


		unsigned GetIndex() const { return index_; }


		BlendMode GetBlendMode() const { return blendMode_; }


		CompareMode GetDepthTestMode() const { return depthTestMode_; }


		PassLightingMode GetLightingMode() const { return lightingMode_; }


		unsigned GetShadersLoadedFrameNumber() const { return shadersLoadedFrameNumber_; }


		bool GetDepthWrite() const { return depthWrite_; }


		bool GetAlphaMask() const { return alphaMask_; }


		bool IsDesktop() const { return isDesktop_; }


		const YumeString& GetVertexShader() const { return vertexShaderName_; }


		const YumeString& GetPixelShader() const { return pixelShaderName_; }


		const YumeString& GetVertexShaderDefines() const { return vertexShaderDefines_; }


		const YumeString& GetPixelShaderDefines() const { return pixelShaderDefines_; }


		YumeVector<SharedPtr<YumeShaderVariation> >::type& GetVertexShaders() { return vertexShaders_; }


		YumeVector<SharedPtr<YumeShaderVariation> >::type& GetPixelShaders() { return pixelShaders_; }

	private:

		unsigned index_;

		BlendMode blendMode_;

		CompareMode depthTestMode_;

		PassLightingMode lightingMode_;

		unsigned shadersLoadedFrameNumber_;

		bool depthWrite_;

		bool alphaMask_;

		bool isDesktop_;

		YumeString vertexShaderName_;

		YumeString pixelShaderName_;

		YumeString vertexShaderDefines_;

		YumeString pixelShaderDefines_;

		YumeVector<SharedPtr<YumeShaderVariation> >::type vertexShaders_;

		YumeVector<SharedPtr<YumeShaderVariation> >::type pixelShaders_;

		YumeString name_;
	};

	class YumeAPIExport YumeRenderTechnique : public YumeResource
	{
		friend class YumeRenderer;

	public:
		YumeRenderTechnique();
		~YumeRenderTechnique();

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;


		virtual bool BeginLoad(YumeFile& source);


		void SetIsDesktop(bool enable);

		YumeRenderPass* CreatePass(const YumeString& passName);

		void RemovePass(const YumeString& passName);

		void ReleaseShaders();


		bool IsDesktop() const { return isDesktop_; }


		bool IsSupported() const { return !isDesktop_ || desktopSupport_; }


		bool HasPass(unsigned passIndex) const { return passIndex < passes_.size() && passes_[passIndex] != 0; }


		bool HasPass(const YumeString& passName) const;


		YumeRenderPass* GetPass(unsigned passIndex) const { return passIndex < passes_.size() ? passes_[passIndex].Get() : 0; }


		YumeRenderPass* GetPass(const YumeString& passName) const;


		YumeRenderPass* GetSupportedPass(unsigned passIndex) const
		{
			YumeRenderPass* pass = passIndex < passes_.size() ? passes_[passIndex].Get() : 0;
			return pass && (!pass->IsDesktop() || desktopSupport_) ? pass : 0;
		}


		YumeRenderPass* GetSupportedPass(const YumeString& passName) const;


		unsigned GetNumPasses() const;

		YumeVector<YumeString>::type GetPassNames() const;

		YumeVector<YumeRenderPass*>::type GetPasses() const;


		static unsigned GetPassIndex(const YumeString& passName);


		static unsigned basePassIndex;

		static unsigned alphaPassIndex;

		static unsigned materialPassIndex;

		static unsigned deferredPassIndex;

		static unsigned lightPassIndex;

		static unsigned litBasePassIndex;

		static unsigned litAlphaPassIndex;

		static unsigned shadowPassIndex;

	private:

		bool isDesktop_;

		bool desktopSupport_;

		YumeVector<SharedPtr<YumeRenderPass> >::type passes_;


		static YumeMap<YumeString,unsigned>::type passIndices;
	};
}


//----------------------------------------------------------------------------
#endif
