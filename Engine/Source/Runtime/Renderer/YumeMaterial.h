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
#ifndef __YumeMaterial_h__
#define __YumeMaterial_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeLight.h"
#include "Math/YumeVector4.h"
#include "Renderer/YumeResource.h"
#include "Core/YumeVariant.h"
#include "Core/YumeComponentAnimInfo.h"
#include "Core/YumeXmlFile.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{	class YumeMaterial;	class YumeRenderPass;
	class YumeScene;
	class YumeRenderTechnique;
	class YumeTexture;
	class YumeTexture2D;
	class YumeTextureCube;
	class ValueAnimationInfo;
	static const unsigned char DEFAULT_RENDER_ORDER = 128;
	struct MaterialShaderParameter
	{
		YumeString name_;
		Variant value_;
	};

	struct TechniqueEntry
	{
		TechniqueEntry();

		TechniqueEntry(YumeRenderTechnique* tech,unsigned qualityLevel,float lodDistance);

		~TechniqueEntry();

		YumeRenderTechnique* technique_;

		int qualityLevel_;
		float lodDistance_;
	};

	class ShaderParameterAnimationInfo : public ValueAnimationInfo
	{
	public:
		ShaderParameterAnimationInfo
			(YumeMaterial* material,const YumeString& name,ValueAnimation* attributeAnimation,WrapMode wrapMode,float speed);
		ShaderParameterAnimationInfo(const ShaderParameterAnimationInfo& other);
		~ShaderParameterAnimationInfo();

		const YumeString& GetName() const { return name_; }
	protected:

		virtual void ApplyValue(const Variant& newValue);
	private:

		YumeString name_;
	};


	class YumeAPIExport YumeMaterial : public YumeResource
	{
	public:
		YumeMaterial();
		~YumeMaterial();

		virtual bool BeginLoad(YumeFile& source);
		virtual bool EndLoad();
		virtual bool Save(YumeFile& dest) const;

		
		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;

		bool Load(const XmlNode& source);
		bool Save(XmlNode& dest) const;
		void SetNumTechniques(unsigned num);
		void SetTechnique(unsigned index,YumeRenderTechnique* tech,unsigned qualityLevel = 0,float lodDistance = 0.0f);
		void SetShaderParameter(const YumeString& name,const Variant& value);
		void SetShaderParameterAnimation(const YumeString& name,ValueAnimation* animation,WrapMode wrapMode = WM_LOOP,float speed = 1.0f);
		void SetShaderParameterAnimationWrapMode(const YumeString& name,WrapMode wrapMode);
		void SetShaderParameterAnimationSpeed(const YumeString& name,float speed);
		void SetTexture(TextureUnit unit,YumeTexture* texture);
		void SetUVTransform(const Vector2& offset,float rotation,const Vector2& repeat);
		void SetUVTransform(const Vector2& offset,float rotation,float repeat);
		void SetCullMode(CullMode mode);
		void SetShadowCullMode(CullMode mode);
		void SetFillMode(FillMode mode);
		void SetDepthBias(const BiasParameters& parameters);
		void SetRenderOrder(unsigned char order);
		void SetScene(YumeScene* scene);
		void RemoveShaderParameter(const YumeString& name);
		void ReleaseShaders();


		SharedPtr<YumeMaterial> Clone(const YumeString& cloneName = YumeString()) const;
		void SortTechniques();
		void MarkForAuxView(unsigned frameNumber);


		unsigned GetNumTechniques() const { return techniques_.size(); }
		const YumeVector<TechniqueEntry>::type& GetTechniques() const { return techniques_; }
		const TechniqueEntry& GetTechniqueEntry(unsigned index) const;
		YumeRenderTechnique* GetTechnique(unsigned index) const;
		YumeRenderPass* GetPass(unsigned index,const YumeString& passName) const;
		YumeTexture* GetTexture(TextureUnit unit) const;
		const YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type& GetTextures() const { return textures_; }
		const Variant& GetShaderParameter(const YumeString& name) const;
		ValueAnimation* GetShaderParameterAnimation(const YumeString& name) const;
		WrapMode GetShaderParameterAnimationWrapMode(const YumeString& name) const;
		float GetShaderParameterAnimationSpeed(const YumeString& name) const;
		const YumeMap<YumeHash,MaterialShaderParameter>::type& GetShaderParameters() const { return shaderParameters_; }
		CullMode GetCullMode() const { return cullMode_; }
		CullMode GetShadowCullMode() const { return shadowCullMode_; }
		FillMode GetFillMode() const { return fillMode_; }
		const BiasParameters& GetDepthBias() const { return depthBias_; }
		unsigned char GetRenderOrder() const { return renderOrder_; }
		unsigned GetAuxViewFrameNumber() const { return auxViewFrameNumber_; }
		bool GetOcclusion() const { return occlusion_; }
		bool GetSpecular() const { return specular_; }
		YumeScene* GetScene() const;
		unsigned GetShaderParameterHash() const { return shaderParameterHash_; }
		static YumeString GetTextureUnitName(TextureUnit unit);
		static Variant ParseShaderParameterValue(const YumeString& value);


	private:
		bool BeginLoadXML(YumeFile& source);
		void CheckOcclusion();
		void ResetToDefaults();
		void RefreshShaderParameterHash();
		void RefreshMemoryUse();
		ShaderParameterAnimationInfo* GetShaderParameterAnimationInfo(const YumeString& name) const;
		void UpdateEventSubscription();



		YumeVector<TechniqueEntry>::type techniques_;
		YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type textures_;
		YumeMap<YumeHash,MaterialShaderParameter>::type shaderParameters_;
		YumeMap<YumeHash,SharedPtr<ShaderParameterAnimationInfo> >::type shaderParameterAnimationInfos_;
		CullMode cullMode_;
		CullMode shadowCullMode_;
		FillMode fillMode_;
		BiasParameters depthBias_;
		unsigned char renderOrder_;
		unsigned auxViewFrameNumber_;
		unsigned shaderParameterHash_;
		bool occlusion_;
		bool specular_;
		bool subscribed_;
		bool batchedParameterUpdate_;
		SharedPtr<YumeXmlFile> loadXMLFile_;
		SharedPtr<YumeScene> scene_;
	};
}
//----------------------------------------------------------------------------
#endif
