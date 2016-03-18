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
#ifndef __YumeLight_h__
#define __YumeLight_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeFrustum.h"
#include "Renderer/YumeDrawable.h"
#include "Renderer/YumeTexture.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeCamera;
	struct LightBatchQueue;


	enum LightType
	{
		LIGHT_DIRECTIONAL = 0,
		LIGHT_SPOT,
		LIGHT_POINT
	};

	static const float SHADOW_MIN_QUANTIZE = 0.1f;
	static const float SHADOW_MIN_VIEW = 1.0f;
	static const int MAX_LIGHT_SPLITS = 6;
	static const int MAX_CASCADE_SPLITS = 4;


	struct YumeAPIExport BiasParameters
	{

		BiasParameters()
		{
		}


		BiasParameters(float constantBias,float slopeScaledBias):
			constantBias_(constantBias),
			slopeScaledBias_(slopeScaledBias)
		{
		}


		void Validate();


		float constantBias_;

		float slopeScaledBias_;
	};


	struct YumeAPIExport CascadeParameters
	{

		CascadeParameters()
		{
		}


		CascadeParameters(float split1,float split2,float split3,float split4,float fadeStart,float biasAutoAdjust = 1.0f):
			fadeStart_(fadeStart),
			biasAutoAdjust_(biasAutoAdjust)
		{
			splits_[0] = split1;
			splits_[1] = split2;
			splits_[2] = split3;
			splits_[3] = split4;
		}


		void Validate();


		float GetShadowRange() const
		{
			float ret = 0.0f;
			for(unsigned i = 0; i < MAX_CASCADE_SPLITS; ++i)
				ret = Max(ret,splits_[i]);

			return ret;
		}


		float splits_[4];

		float fadeStart_;

		float biasAutoAdjust_;
	};


	struct YumeAPIExport FocusParameters
	{

		FocusParameters()
		{
		}


		FocusParameters(bool focus,bool nonUniform,bool autoSize,float quantize,float minView):
			focus_(focus),
			nonUniform_(nonUniform),
			autoSize_(autoSize),
			quantize_(quantize),
			minView_(minView)
		{
		}


		void Validate();


		bool focus_;

		bool nonUniform_;

		bool autoSize_;

		float quantize_;

		float minView_;
	};


	class YumeAPIExport YumeLight : public YumeDrawable
	{
	public:
		YumeLight();
		virtual ~YumeLight();

		virtual void ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results);
		virtual void UpdateBatches(const FrameInfo& frame);
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;

		void SetLightType(LightType type);

		void SetPerVertex(bool enable);

		void SetColor(const YumeColor& color);

		void SetSpecularIntensity(float intensity);

		void SetBrightness(float brightness);

		void SetRange(float range);

		void SetFov(float fov);

		void SetAspectRatio(float aspectRatio);

		void SetFadeDistance(float distance);

		void SetShadowFadeDistance(float distance);

		void SetShadowBias(const BiasParameters& parameters);

		void SetShadowCascade(const CascadeParameters& parameters);

		void SetShadowFocus(const FocusParameters& parameters);

		void SetShadowIntensity(float intensity);

		void SetShadowResolution(float resolution);

		void SetShadowNearFarRatio(float nearFarRatio);

		void SetRampTexture(SharedPtr<YumeTexture> texture);

		void SetShapeTexture(SharedPtr<YumeTexture> texture);


		LightType GetLightType() const { return lightType_; }


		bool GetPerVertex() const { return perVertex_; }


		const YumeColor& GetColor() const { return color_; }


		float GetSpecularIntensity() const { return specularIntensity_; }


		float GetBrightness() const { return brightness_; }


		YumeColor GetEffectiveColor() const { return YumeColor(color_ * brightness_,1.0f); }


		float GetEffectiveSpecularIntensity() const { return specularIntensity_ * Abs(brightness_); }


		float GetRange() const { return range_; }


		float GetFov() const { return fov_; }


		float GetAspectRatio() const { return aspectRatio_; }


		float GetFadeDistance() const { return fadeDistance_; }


		float GetShadowFadeDistance() const { return shadowFadeDistance_; }


		const BiasParameters& GetShadowBias() const { return shadowBias_; }


		const CascadeParameters& GetShadowCascade() const { return shadowCascade_; }


		const FocusParameters& GetShadowFocus() const { return shadowFocus_; }


		float GetShadowIntensity() const { return shadowIntensity_; }


		float GetShadowResolution() const { return shadowResolution_; }


		float GetShadowNearFarRatio() const { return shadowNearFarRatio_; }


		YumeTexture* GetRampTexture() const { return rampTexture_; }


		YumeTexture* GetShapeTexture() const { return shapeTexture_; }


		Frustum GetFrustum() const;

		int GetNumShadowSplits() const;


		bool IsNegative() const { return GetEffectiveColor().SumRGB() < 0.0f; }


		void SetIntensitySortValue(float distance);

		void SetIntensitySortValue(const BoundingBox& box);

		void SetLightQueue(LightBatchQueue* queue);

		const Matrix3x4& GetVolumeTransform(YumeCamera* camera);


		LightBatchQueue* GetLightQueue() const { return lightQueue_; }


		float GetIntensityDivisor(float attenuation = 1.0f) const
		{
			return Max(GetEffectiveColor().SumRGB(),0.0f) * attenuation + M_EPSILON;
		}

	protected:

		virtual void OnWorldBoundingBoxUpdate();

	private:

		LightType lightType_;

		YumeColor color_;

		BiasParameters shadowBias_;

		CascadeParameters shadowCascade_;

		FocusParameters shadowFocus_;

		Matrix3x4 volumeTransform_;

		SharedPtr<YumeTexture> rampTexture_;

		SharedPtr<YumeTexture> shapeTexture_;

		LightBatchQueue* lightQueue_;

		float specularIntensity_;

		float brightness_;

		float range_;

		float fov_;

		float aspectRatio_;

		float fadeDistance_;

		float shadowFadeDistance_;

		float shadowIntensity_;

		float shadowResolution_;

		float shadowNearFarRatio_;

		bool perVertex_;
	};

	inline bool CompareLights(YumeLight* lhs,YumeLight* rhs)
	{
		// When sorting lights, give priority to per-vertex lights, so that vertex lit base pass can be evaluated first
		if(lhs->GetPerVertex() != rhs->GetPerVertex())
			return lhs->GetPerVertex();
		else
			return lhs->GetSortValue() < rhs->GetSortValue();
	}
}


//----------------------------------------------------------------------------
#endif
