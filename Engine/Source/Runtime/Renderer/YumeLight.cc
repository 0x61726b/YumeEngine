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
#include "YumeHeaders.h"
#include "YumeLight.h"
#include "YumeCamera.h"
#include "YumeAuxRenderer.h"
#include "Scene/YumeOctreeQuery.h"
#include "YumeTexture2D.h"
#include "Scene/YumeSceneNode.h"

#include "Logging/logging.h"

#undef near
#undef far


namespace YumeEngine
{

	extern const char* SCENE_CATEGORY;

	static const LightType DEFAULT_LIGHTTYPE = LIGHT_POINT;
	static const float DEFAULT_RANGE = 10.0f;
	static const float DEFAULT_LIGHT_FOV = 30.0f;
	static const float DEFAULT_SPECULARINTENSITY = 1.0f;
	static const float DEFAULT_BRIGHTNESS = 1.0f;
	static const float DEFAULT_CONSTANTBIAS = 0.0002f;
	static const float DEFAULT_SLOPESCALEDBIAS = 0.5f;
	static const float DEFAULT_BIASAUTOADJUST = 1.0f;
	static const float DEFAULT_SHADOWFADESTART = 0.8f;
	static const float DEFAULT_SHADOWQUANTIZE = 0.5f;
	static const float DEFAULT_SHADOWMINVIEW = 3.0f;
	static const float DEFAULT_SHADOWNEARFARRATIO = 0.002f;
	static const float DEFAULT_SHADOWSPLIT = 1000.0f;

	static const char* typeNames[] =
	{
		"Directional",
		"Spot",
		"Point",
		0
	};

	void BiasParameters::Validate()
	{
		constantBias_ = Clamp(constantBias_,-1.0f,1.0f);
		slopeScaledBias_ = Clamp(slopeScaledBias_,-16.0f,16.0f);
	}

	void CascadeParameters::Validate()
	{
		for(unsigned i = 0; i < MAX_CASCADE_SPLITS; ++i)
			splits_[i] = Max(splits_[i],0.0f);
		fadeStart_ = Clamp(fadeStart_,M_EPSILON,1.0f);
	}

	void FocusParameters::Validate()
	{
		quantize_ = Max(quantize_,SHADOW_MIN_QUANTIZE);
		minView_ = Max(minView_,SHADOW_MIN_VIEW);
	}

	YumeHash YumeLight::type_ = "Light";

	YumeLight::YumeLight():
		YumeDrawable(DRAWABLE_LIGHT),
		lightType_(DEFAULT_LIGHTTYPE),
		shadowBias_(BiasParameters(DEFAULT_CONSTANTBIAS,DEFAULT_SLOPESCALEDBIAS)),
		shadowCascade_(CascadeParameters(DEFAULT_SHADOWSPLIT,0.0f,0.0f,0.0f,DEFAULT_SHADOWFADESTART)),
		shadowFocus_(FocusParameters(true,true,true,DEFAULT_SHADOWQUANTIZE,DEFAULT_SHADOWMINVIEW)),
		lightQueue_(0),
		specularIntensity_(DEFAULT_SPECULARINTENSITY),
		brightness_(DEFAULT_BRIGHTNESS),
		range_(DEFAULT_RANGE),
		fov_(DEFAULT_LIGHT_FOV),
		aspectRatio_(1.0f),
		fadeDistance_(0.0f),
		shadowFadeDistance_(0.0f),
		shadowIntensity_(0.0f),
		shadowResolution_(1.0f),
		shadowNearFarRatio_(DEFAULT_SHADOWNEARFARRATIO),
		perVertex_(false)
	{
	}

	YumeLight::~YumeLight()
	{
	}

	void YumeLight::ProcessRayQuery(const RayOctreeQuery& query,YumePodVector<RayQueryResult>::type& results)
	{
		// Do not record a raycast result for a directional light, as it would block all other results
		if(lightType_ == LIGHT_DIRECTIONAL)
			return;

		float distance = query.maxDistance_;
		switch(query.level_)
		{
		case RAY_AABB:
			YumeDrawable::ProcessRayQuery(query,results);
			return;

		case RAY_OBB:
		{
			Matrix3x4 inverse(node_->GetWorldTransform().Inverse());
			Ray localRay = query.ray_.Transformed(inverse);
			distance = localRay.HitDistance(GetWorldBoundingBox().Transformed(inverse));
			if(distance >= query.maxDistance_)
				return;
		}
		break;

		case RAY_TRIANGLE:
			if(lightType_ == LIGHT_SPOT)
			{
				distance = query.ray_.HitDistance(GetFrustum());
				if(distance >= query.maxDistance_)
					return;
			}
			else
			{
				distance = query.ray_.HitDistance(Sphere(node_->GetWorldPosition(),range_));
				if(distance >= query.maxDistance_)
					return;
			}
			break;

		case RAY_TRIANGLE_UV:
			YUMELOG_WARN("RAY_TRIANGLE_UV query level is not supported for Light component");
			return;
		}

		// If the code reaches here then we have a hit
		RayQueryResult result;
		result.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
		result.normal_ = -query.ray_.direction_;
		result.distance_ = distance;
		result.drawable_ = this;
		result.node_ = node_;
		result.subObject_ = M_MAX_UNSIGNED;
		results.push_back(result);
	}

	void YumeLight::UpdateBatches(const FrameInfo& frame)
	{
		switch(lightType_)
		{
		case LIGHT_DIRECTIONAL:
			// Directional light affects the whole scene, so it is always "closest"
			distance_ = 0.0f;
			break;

		default:
			distance_ = frame.camera_->GetDistance(node_->GetWorldPosition());
			break;
		}
	}

	void YumeLight::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		YumeColor color = GetEffectiveColor();

		if(debug && IsEnabledEffective())
		{
			switch(lightType_)
			{
			case LIGHT_DIRECTIONAL:
			{
				Vector3 start = node_->GetWorldPosition();
				Vector3 end = start + node_->GetWorldDirection() * 10.f;
				for(int i = -1; i < 2; ++i)
				{
					for(int j = -1; j < 2; ++j)
					{
						Vector3 offset = Vector3::UP * (5.f * i) + Vector3::RIGHT * (5.f * j);
						debug->AddSphere(Sphere(start + offset,0.1f),color,depthTest);
						debug->AddLine(start + offset,end + offset,color,depthTest);
					}
				}
			}
			break;

			case LIGHT_SPOT:
				debug->AddFrustum(GetFrustum(),color,depthTest);
				break;

			case LIGHT_POINT:
				debug->AddSphere(Sphere(node_->GetWorldPosition(),range_),color,depthTest);
				break;
			}
		}
	}

	void YumeLight::SetLightType(LightType type)
	{
		lightType_ = type;
		OnMarkedDirty(node_);
	}

	void YumeLight::SetPerVertex(bool enable)
	{
		perVertex_ = enable;
	}

	void YumeLight::SetColor(const YumeColor& color)
	{
		color_ = YumeColor(color.r_,color.g_,color.b_,1.0f);
	}

	void YumeLight::SetSpecularIntensity(float intensity)
	{
		specularIntensity_ = Max(intensity,0.0f);
	}

	void YumeLight::SetBrightness(float brightness)
	{
		brightness_ = brightness;
	}

	void YumeLight::SetRange(float range)
	{
		range_ = Max(range,0.0f);
		OnMarkedDirty(node_);
	}

	void YumeLight::SetFov(float fov)
	{
		fov_ = Clamp(fov,0.0f,M_MAX_FOV);
		OnMarkedDirty(node_);
	}

	void YumeLight::SetAspectRatio(float aspectRatio)
	{
		aspectRatio_ = Max(aspectRatio,M_EPSILON);
		OnMarkedDirty(node_);
	}

	void YumeLight::SetShadowNearFarRatio(float nearFarRatio)
	{
		shadowNearFarRatio_ = Clamp(nearFarRatio,0.0f,0.5f);
	}

	void YumeLight::SetFadeDistance(float distance)
	{
		fadeDistance_ = Max(distance,0.0f);
		
	}

	void YumeLight::SetShadowBias(const BiasParameters& parameters)
	{
		shadowBias_ = parameters;
		shadowBias_.Validate();
		
	}

	void YumeLight::SetShadowCascade(const CascadeParameters& parameters)
	{
		shadowCascade_ = parameters;
		shadowCascade_.Validate();
		
	}

	void YumeLight::SetShadowFocus(const FocusParameters& parameters)
	{
		shadowFocus_ = parameters;
		shadowFocus_.Validate();
		
	}

	void YumeLight::SetShadowFadeDistance(float distance)
	{
		shadowFadeDistance_ = Max(distance,0.0f);
		
	}

	void YumeLight::SetShadowIntensity(float intensity)
	{
		shadowIntensity_ = Clamp(intensity,0.0f,1.0f);
		
	}

	void YumeLight::SetShadowResolution(float resolution)
	{
		shadowResolution_ = Clamp(resolution,0.125f,1.0f);
	}

	void YumeLight::SetRampTexture(YumeTexture* texture)
	{
		rampTexture_ = (texture);
	}

	void YumeLight::SetShapeTexture(YumeTexture* texture)
	{
		shapeTexture_ = (texture);
	}

	Frustum YumeLight::GetFrustum() const
	{
		// Note: frustum is unaffected by node or parent scale
		Matrix3x4 frustumTransform(node_ ? Matrix3x4(node_->GetWorldPosition(),node_->GetWorldRotation(),1.0f) :
			Matrix3x4::IDENTITY);
		Frustum ret;
		ret.Define(fov_,aspectRatio_,1.0f,M_MIN_NEARCLIP,range_,frustumTransform);
		return ret;
	}

	int YumeLight::GetNumShadowSplits() const
	{
		int ret = 1;

		if(shadowCascade_.splits_[1] > shadowCascade_.splits_[0])
		{
			++ret;
			if(shadowCascade_.splits_[2] > shadowCascade_.splits_[1])
			{
				++ret;
				if(shadowCascade_.splits_[3] > shadowCascade_.splits_[2])
					++ret;
			}
		}

		return Min(ret,MAX_CASCADE_SPLITS);
	}

	const Matrix3x4& YumeLight::GetVolumeTransform(YumeCamera* camera)
	{
		if(!node_)
			return Matrix3x4::IDENTITY;

		switch(lightType_)
		{
		case LIGHT_DIRECTIONAL:
		{
			Matrix3x4 quadTransform;
			Vector3 near,far;
			// Position the directional light quad in halfway between far & near planes to prevent depth clipping
			camera->GetFrustumSize(near,far);
			quadTransform.SetTranslation(Vector3(0.0f,0.0f,(camera->GetNearClip() + camera->GetFarClip()) * 0.5f));
			quadTransform.SetScale(Vector3(far.x_,far.y_,1.0f)); // Will be oversized, but doesn't matter (gets frustum clipped)
			volumeTransform_ = camera->GetEffectiveWorldTransform() * quadTransform;
		}
		break;

		case LIGHT_SPOT:
		{
			float yScale = tanf(fov_ * M_DEGTORAD * 0.5f) * range_;
			float xScale = aspectRatio_ * yScale;
			volumeTransform_ = Matrix3x4(node_->GetWorldPosition(),node_->GetWorldRotation(),Vector3(xScale,yScale,range_));
		}
		break;

		case LIGHT_POINT:
			volumeTransform_ = Matrix3x4(node_->GetWorldPosition(),Quaternion::IDENTITY,range_);
			break;
		}

		return volumeTransform_;
	}

	void YumeLight::OnWorldBoundingBoxUpdate()
	{
		switch(lightType_)
		{
		case LIGHT_DIRECTIONAL:
			// Directional light always sets humongous bounding box not affected by transform
			worldBoundingBox_.Define(-M_LARGE_VALUE,M_LARGE_VALUE);
			break;

		case LIGHT_SPOT:
			// Frustum is already transformed into world space
			worldBoundingBox_.Define(GetFrustum());
			break;

		case LIGHT_POINT:
		{
			const Vector3& center = node_->GetWorldPosition();
			Vector3 edge(range_,range_,range_);
			worldBoundingBox_.Define(center - edge,center + edge);
		}
		break;
		}
	}

	void YumeLight::SetIntensitySortValue(float distance)
	{
		// When sorting lights globally, give priority to directional lights so that they will be combined into the ambient pass
		if(!IsNegative())
		{
			if(lightType_ != LIGHT_DIRECTIONAL)
				sortValue_ = Max(distance,M_MIN_NEARCLIP) / GetIntensityDivisor();
			else
				sortValue_ = M_EPSILON / GetIntensityDivisor();
		}
		else
		{
			// Give extra priority to negative lights in the global sorting order so that they're handled first, right after ambient.
			// Positive lights are added after them
			if(lightType_ != LIGHT_DIRECTIONAL)
				sortValue_ = -Max(distance,M_MIN_NEARCLIP) * GetIntensityDivisor();
			else
				sortValue_ = -M_LARGE_VALUE * GetIntensityDivisor();
		}
	}

	void YumeLight::SetIntensitySortValue(const BoundingBox& box)
	{
		// When sorting lights for object's maximum light cap, give priority based on attenuation and intensity
		switch(lightType_)
		{
		case LIGHT_DIRECTIONAL:
			sortValue_ = 1.0f / GetIntensityDivisor();
			break;

		case LIGHT_SPOT:
		{
			Vector3 centerPos = box.Center();
			Vector3 lightPos = node_->GetWorldPosition();
			Vector3 lightDir = node_->GetWorldDirection();
			Ray lightRay(lightPos,lightDir);

			Vector3 centerProj = lightRay.Project(centerPos);
			float centerDistance = (centerProj - lightPos).Length();
			Ray centerRay(centerProj,centerPos - centerProj);
			float centerAngle = centerRay.HitDistance(box) / centerDistance;

			// Check if a corner of the bounding box is closer to the light ray than the center, use its angle in that case
			Vector3 cornerPos = centerPos + box.HalfSize() * Vector3(centerPos.x_ < centerProj.x_ ? 1.0f : -1.0f,
				centerPos.y_ < centerProj.y_ ? 1.0f : -1.0f,centerPos.z_ < centerProj.z_ ? 1.0f : -1.0f);
			Vector3 cornerProj = lightRay.Project(cornerPos);
			float cornerDistance = (cornerProj - lightPos).Length();
			float cornerAngle = (cornerPos - cornerProj).Length() / cornerDistance;

			float spotAngle = Min(centerAngle,cornerAngle);
			float maxAngle = tanf(fov_ * M_DEGTORAD * 0.5f);
			float spotFactor = Min(spotAngle / maxAngle,1.0f);
			// We do not know the actual range attenuation ramp, so take only spot attenuation into account
			float att = Max(1.0f - spotFactor * spotFactor,M_EPSILON);
			sortValue_ = 1.0f / GetIntensityDivisor(att);
		}
		break;

		case LIGHT_POINT:
		{
			Vector3 centerPos = box.Center();
			Vector3 lightPos = node_->GetWorldPosition();
			Vector3 lightDir = (centerPos - lightPos).Normalized();
			Ray lightRay(lightPos,lightDir);
			float distance = lightRay.HitDistance(box);
			float normDistance = distance / range_;
			float att = Max(1.0f - normDistance * normDistance,M_EPSILON);
			sortValue_ = 1.0f / GetIntensityDivisor(att);
		}
		break;
		}
	}

	void YumeLight::SetLightQueue(LightBatchQueue* queue)
	{
		lightQueue_ = queue;
	}

}
