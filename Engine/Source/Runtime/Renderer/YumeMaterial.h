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
#include "Core/YumeXmlParser.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeMaterial;
	class YumeRenderPass;
	class YumeScene;
	class YumeRenderTechnique;
	class YumeTexture;
	class YumeTexture2D;
	class TextureCube;
	class ValueAnimationInfo;

	static const unsigned char DEFAULT_RENDER_ORDER = 128;

	/// %Material's shader parameter definition.
	struct MaterialShaderParameter
	{
		/// Name.
		YumeString name_;
		/// Value.
		Variant value_;
	};

	/// %Material's technique list entry.
	struct TechniqueEntry
	{
		/// Construct with defaults.
		TechniqueEntry();
		/// Construct with parameters.
		TechniqueEntry(YumeRenderTechnique* tech,unsigned qualityLevel,float lodDistance);
		/// Destruct.
		~TechniqueEntry();

		/// Technique.
		SharedPtr<YumeRenderTechnique> technique_;
		/// Quality level.
		int qualityLevel_;
		/// LOD distance.
		float lodDistance_;
	};

	/// Material's shader parameter animation instance.
	class ShaderParameterAnimationInfo : public ValueAnimationInfo
	{
	public:
		/// Construct.
		ShaderParameterAnimationInfo
			(YumeMaterial* material,const YumeString& name,ValueAnimation* attributeAnimation,WrapMode wrapMode,float speed);
		/// Copy construct.
		ShaderParameterAnimationInfo(const ShaderParameterAnimationInfo& other);
		/// Destruct.
		~ShaderParameterAnimationInfo();

		/// Return shader parameter name.
		const YumeString& GetName() const { return name_; }

	protected:
		/// Apply new animation value to the target object. Called by Update().
		virtual void ApplyValue(const Variant& newValue);

	private:
		/// Shader parameter name.
		YumeString name_;
	};

	/// Describes how to render 3D geometries.
	class YumeAPIExport YumeMaterial : public YumeResource
	{
	public:
		/// Construct.
		YumeMaterial();
		/// Destruct.
		~YumeMaterial();

		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);
		/// Finish resource loading. Always called from the main thread. Return true if successful.
		virtual bool EndLoad();
		/// Save resource. Return true if successful.
		virtual bool Save(YumeFile& dest) const;

		/// Load from an XML element. Return true if successful.
		bool Load(const XmlNode& source);
		/// Save to an XML element. Return true if successful.
		bool Save(XmlNode& dest) const;

		/// Set number of techniques.
		void SetNumTechniques(unsigned num);
		/// Set technique.
		void SetTechnique(unsigned index,YumeRenderTechnique* tech,unsigned qualityLevel = 0,float lodDistance = 0.0f);
		/// Set shader parameter.
		void SetShaderParameter(const YumeString& name,const Variant& value);
		/// Set shader parameter animation.
		void
			SetShaderParameterAnimation(const YumeString& name,ValueAnimation* animation,WrapMode wrapMode = WM_LOOP,float speed = 1.0f);
		/// Set shader parameter animation wrap mode.
		void SetShaderParameterAnimationWrapMode(const YumeString& name,WrapMode wrapMode);
		/// Set shader parameter animation speed.
		void SetShaderParameterAnimationSpeed(const YumeString& name,float speed);
		/// Set texture.
		void SetTexture(TextureUnit unit,YumeTexture* texture);
		/// Set texture coordinate transform.
		void SetUVTransform(const Vector2& offset,float rotation,const Vector2& repeat);
		/// Set texture coordinate transform.
		void SetUVTransform(const Vector2& offset,float rotation,float repeat);
		/// Set culling mode.
		void SetCullMode(CullMode mode);
		/// Set culling mode for shadows.
		void SetShadowCullMode(CullMode mode);
		/// Set polygon fill mode. Interacts with the camera's fill mode setting so that the "least filled" mode will be used.
		void SetFillMode(FillMode mode);
		/// Set depth bias.
		void SetDepthBias(const BiasParameters& parameters);
		/// Set 8-bit render order within pass. Default 128. Lower values will render earlier and higher values later, taking precedence over e.g. state and distance sorting.
		void SetRenderOrder(unsigned char order);
		/// Associate the material with a scene to ensure that shader parameter animation happens in sync with scene update, respecting the scene time scale. If no scene is set, the global update events will be used.
		void SetScene(YumeScene* scene);
		/// Remove shader parameter.
		void RemoveShaderParameter(const YumeString& name);
		/// Reset all shader pointers.
		void ReleaseShaders();
		/// Clone the material.
		SharedPtr<YumeMaterial> Clone(const YumeString& cloneName = YumeString()) const;
		/// Ensure that material techniques are listed in correct order.
		void SortTechniques();
		/// Mark material for auxiliary view rendering.
		void MarkForAuxView(unsigned frameNumber);

		/// Return number of techniques.
		unsigned GetNumTechniques() const { return techniques_.size(); }

		/// Return all techniques.
		const YumeVector<TechniqueEntry>::type& GetTechniques() const { return techniques_; }

		/// Return technique entry by index.
		const TechniqueEntry& GetTechniqueEntry(unsigned index) const;
		/// Return technique by index.
		YumeRenderTechnique* GetTechnique(unsigned index) const;
		/// Return pass by technique index and pass name.
		YumeRenderPass* GetPass(unsigned index,const YumeString& passName) const;
		/// Return texture by unit.
		YumeTexture* GetTexture(TextureUnit unit) const;

		/// Return all textures.
		const YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type& GetTextures() const { return textures_; }

		/// Return shader parameter.
		const Variant& GetShaderParameter(const YumeString& name) const;
		/// Return shader parameter animation.
		ValueAnimation* GetShaderParameterAnimation(const YumeString& name) const;
		/// Return shader parameter animation wrap mode.
		WrapMode GetShaderParameterAnimationWrapMode(const YumeString& name) const;
		/// Return shader parameter animation speed.
		float GetShaderParameterAnimationSpeed(const YumeString& name) const;

		/// Return all shader parameters.
		const YumeMap<YumeHash,MaterialShaderParameter>::type& GetShaderParameters() const { return shaderParameters_; }

		/// Return normal culling mode.
		CullMode GetCullMode() const { return cullMode_; }

		/// Return culling mode for shadows.
		CullMode GetShadowCullMode() const { return shadowCullMode_; }

		/// Return polygon fill mode.
		FillMode GetFillMode() const { return fillMode_; }

		/// Return depth bias.
		const BiasParameters& GetDepthBias() const { return depthBias_; }

		/// Return render order.
		unsigned char GetRenderOrder() const { return renderOrder_; }

		/// Return last auxiliary view rendered frame number.
		unsigned GetAuxViewFrameNumber() const { return auxViewFrameNumber_; }

		/// Return whether should render occlusion.
		bool GetOcclusion() const { return occlusion_; }

		/// Return whether should render specular.
		bool GetSpecular() const { return specular_; }

		/// Return the scene associated with the material for shader parameter animation updates.
		YumeScene* GetScene() const;

		/// Return shader parameter hash value. Used as an optimization to avoid setting shader parameters unnecessarily.
		unsigned GetShaderParameterHash() const { return shaderParameterHash_; }

		/// Return name for texture unit.
		static YumeString GetTextureUnitName(TextureUnit unit);
		/// Parse a shader parameter value from a string. Retunrs either a bool, a float, or a 2 to 4-component vector.
		static Variant ParseShaderParameterValue(const YumeString& value);

	private:
		/// Helper function for loading JSON files
		bool BeginLoadJSON(YumeFile& source);
		/// Helper function for loading XML files
		bool BeginLoadXML(YumeFile& source);

		/// Re-evaluate occlusion rendering.
		void CheckOcclusion();
		/// Reset to defaults.
		void ResetToDefaults();
		/// Recalculate shader parameter hash.
		void RefreshShaderParameterHash();
		/// Recalculate the memory used by the material.
		void RefreshMemoryUse();
		/// Return shader parameter animation info.
		ShaderParameterAnimationInfo* GetShaderParameterAnimationInfo(const YumeString& name) const;
		/// Update whether should be subscribed to scene or global update events for shader parameter animation.
		void UpdateEventSubscription();

		/// Techniques.
		YumeVector<TechniqueEntry>::type techniques_;
		/// Textures.
		YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type textures_;
		/// %Shader parameters.
		YumeMap<YumeHash,MaterialShaderParameter>::type shaderParameters_;
		/// %Shader parameters animation infos.
		YumeMap<YumeHash,SharedPtr<ShaderParameterAnimationInfo> >::type shaderParameterAnimationInfos_;
		/// Normal culling mode.
		CullMode cullMode_;
		/// Culling mode for shadow rendering.
		CullMode shadowCullMode_;
		/// Polygon fill mode.
		FillMode fillMode_;
		/// Depth bias parameters.
		BiasParameters depthBias_;
		/// Render order value.
		unsigned char renderOrder_;
		/// Last auxiliary view rendered frame number.
		unsigned auxViewFrameNumber_;
		/// Shader parameter hash value.
		unsigned shaderParameterHash_;
		/// Render occlusion flag.
		bool occlusion_;
		/// Specular lighting flag.
		bool specular_;
		/// Flag for whether is subscribed to animation updates.
		bool subscribed_;
		/// Flag to suppress parameter hash and memory use recalculation when setting multiple shader parameters (loading or resetting the material.)
		bool batchedParameterUpdate_;
		/// XML file used while loading.
		SharedPtr<YumeFile> loadXMLFile_;
		/// Associated scene for shader parameter animation updates.
		SharedPtr<YumeScene> scene_;
	};
}


//----------------------------------------------------------------------------
#endif
