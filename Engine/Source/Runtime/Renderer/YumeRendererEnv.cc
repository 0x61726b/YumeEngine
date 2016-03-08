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
#include "YumeRendererEnv.h"
#include "Scene/YumeOctree.h"
#include "YumeAuxRenderer.h"
#include "Scene/YumeSceneNode.h"
#include "Scene/YumeScene.h"



namespace YumeEngine
{

	static const Vector3 DEFAULT_BOUNDING_BOX_MIN(-10.0f,-10.0f,-10.0f);
	static const Vector3 DEFAULT_BOUNDING_BOX_MAX(10.0f,10.0f,10.0f);
	static const YumeColor DEFAULT_AMBIENT_COLOR(0.1f,0.1f,0.1f);
	static const YumeColor DEFAULT_FOG_COLOR(0.0f,0.0f,0.0f);
	static const float DEFAULT_FOG_START = 250.0f;
	static const float DEFAULT_FOG_END = 1000.0f;
	static const float DEFAULT_FOG_HEIGHT = 0.0f;
	static const float DEFAULT_FOG_HEIGHT_SCALE = 0.5f;

	extern const char* SCENE_CATEGORY;

	YumeRendererEnvironment::YumeRendererEnvironment():
		YumeDrawable(DRAWABLE_ZONE),
		inverseWorldDirty_(true),
		heightFog_(false),
		override_(false),
		ambientGradient_(false),
		ambientColor_(DEFAULT_AMBIENT_COLOR),
		fogColor_(DEFAULT_FOG_COLOR),
		fogStart_(DEFAULT_FOG_START),
		fogEnd_(DEFAULT_FOG_END),
		fogHeight_(DEFAULT_FOG_HEIGHT),
		fogHeightScale_(DEFAULT_FOG_HEIGHT_SCALE),
		priority_(0)
	{
		boundingBox_ = BoundingBox(DEFAULT_BOUNDING_BOX_MIN,DEFAULT_BOUNDING_BOX_MAX);
	}

	YumeRendererEnvironment::~YumeRendererEnvironment()
	{
	}

	void YumeRendererEnvironment::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		if(debug && IsEnabledEffective())
			debug->AddBoundingBox(boundingBox_,node_->GetWorldTransform(),YumeColor::GREEN,depthTest);
	}

	void YumeRendererEnvironment::SetBoundingBox(const BoundingBox& box)
	{
		boundingBox_ = box;
		OnMarkedDirty(node_);
		
	}

	void YumeRendererEnvironment::SetAmbientColor(const YumeColor& color)
	{
		ambientColor_ = YumeColor(color,1.0f);
		
	}

	void YumeRendererEnvironment::SetFogColor(const YumeColor& color)
	{
		fogColor_ = YumeColor(color,1.0f);
		
	}

	void YumeRendererEnvironment::SetFogStart(float start)
	{
		if(start < 0.0f)
			start = 0.0f;

		fogStart_ = start;
		
	}

	void YumeRendererEnvironment::SetFogEnd(float end)
	{
		if(end < 0.0f)
			end = 0.0f;

		fogEnd_ = end;
		
	}

	void YumeRendererEnvironment::SetFogHeight(float height)
	{
		fogHeight_ = height;
		
	}

	void YumeRendererEnvironment::SetFogHeightScale(float scale)
	{
		fogHeightScale_ = scale;
		
	}

	void YumeRendererEnvironment::SetPriority(int priority)
	{
		priority_ = priority;
		
	}

	void YumeRendererEnvironment::SetZoneTexture(YumeTexture* texture)
	{
		zoneTexture_ = SharedPtr<YumeTexture>(texture);
		
	}

	void YumeRendererEnvironment::SetHeightFog(bool enable)
	{
		heightFog_ = enable;
		
	}

	void YumeRendererEnvironment::SetOverride(bool enable)
	{
		override_ = enable;
		
	}

	void YumeRendererEnvironment::SetAmbientGradient(bool enable)
	{
		ambientGradient_ = enable;
		
	}

	const Matrix3x4& YumeRendererEnvironment::GetInverseWorldTransform() const
	{
		if(inverseWorldDirty_)
		{
			inverseWorld_ = node_ ? node_->GetWorldTransform().Inverse() : Matrix3x4::IDENTITY;
			inverseWorldDirty_ = false;
		}

		return inverseWorld_;
	}

	const YumeColor& YumeRendererEnvironment::GetAmbientStartColor()
	{
		if(!ambientGradient_)
			return ambientColor_;

		if(!lastAmbientStartZone_ || !lastAmbientEndZone_)
			UpdateAmbientGradient();

		return ambientStartColor_;
	}

	const YumeColor& YumeRendererEnvironment::GetAmbientEndColor()
	{
		if(!ambientGradient_)
			return ambientColor_;

		if(!lastAmbientStartZone_ || !lastAmbientEndZone_)
			UpdateAmbientGradient();

		return ambientEndColor_;
	}

	bool YumeRendererEnvironment::IsInside(const Vector3& point) const
	{
		// Use an oriented bounding box test
		Vector3 localPoint(GetInverseWorldTransform() * point);
		return boundingBox_.IsInside(localPoint) != OUTSIDE;
	}

	void YumeRendererEnvironment::OnMarkedDirty(YumeSceneNode* node)
	{
		// Due to the octree query and weak pointer manipulation, is not safe from worker threads
		YumeScene* scene = GetScene();
		if(scene && scene->IsThreadedUpdate())
		{
			scene->DelayedMarkedDirty(this);
			return;
		}

		YumeDrawable::OnMarkedDirty(node);

		// Clear zone reference from all drawables inside the bounding box, and mark gradient dirty in neighbor zones
		ClearDrawablesZone();

		inverseWorldDirty_ = true;
	}

	void YumeRendererEnvironment::OnWorldBoundingBoxUpdate()
	{
		worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
	}

	void YumeRendererEnvironment::UpdateAmbientGradient()
	{
		// In case no neighbor zones are found, reset ambient start/end with own ambient color
		ambientStartColor_ = ambientColor_;
		ambientEndColor_ = ambientColor_;
		lastAmbientStartZone_ = SharedPtr<YumeRendererEnvironment>(this);
		lastAmbientEndZone_ = SharedPtr<YumeRendererEnvironment>(this);

		if(octant_)
		{
			const Matrix3x4& worldTransform = node_->GetWorldTransform();
			Vector3 center = boundingBox_.Center();
			Vector3 minZPosition = worldTransform * Vector3(center.x_,center.y_,boundingBox_.min_.z_);
			Vector3 maxZPosition = worldTransform * Vector3(center.x_,center.y_,boundingBox_.max_.z_);

			YumeVector<YumeRendererEnvironment*>::type result;
			{
				PointOctreeQuery query(reinterpret_cast<YumeVector<YumeDrawable*>::type&>(result),minZPosition,DRAWABLE_ZONE);
				octant_->GetRoot()->GetDrawables(query);
			}

			// Gradient start position: get the highest priority zone that is not this zone
			int bestPriority = M_MIN_INT;
			YumeRendererEnvironment* bestZone = 0;
			for(YumeVector<YumeRendererEnvironment*>::const_iterator i = result.begin(); i != result.end(); ++i)
			{
				YumeRendererEnvironment* zone = *i;
				int priority = zone->GetPriority();
				if(priority > bestPriority && zone != this && zone->IsInside(minZPosition))
				{
					bestZone = zone;
					bestPriority = priority;
				}
			}

			if(bestZone)
			{
				ambientStartColor_ = bestZone->GetAmbientColor();
				lastAmbientStartZone_ = SharedPtr<YumeRendererEnvironment>(bestZone);
			}

			// Do the same for gradient end position
		{
			PointOctreeQuery query(reinterpret_cast<YumeVector<YumeDrawable*>::type&>(result),maxZPosition,DRAWABLE_ZONE);
			octant_->GetRoot()->GetDrawables(query);
		}
		bestPriority = M_MIN_INT;
		bestZone = 0;

		for(YumeVector<YumeRendererEnvironment*>::const_iterator i = result.begin(); i != result.end(); ++i)
		{
			YumeRendererEnvironment* zone = *i;
			int priority = zone->GetPriority();
			if(priority > bestPriority && zone != this && zone->IsInside(maxZPosition))
			{
				bestZone = zone;
				bestPriority = priority;
			}
		}

		if(bestZone)
		{
			ambientEndColor_ = bestZone->GetAmbientColor();
			lastAmbientEndZone_ = SharedPtr<YumeRendererEnvironment>(bestZone);
		}
		}
	}

	void YumeRendererEnvironment::OnRemoveFromOctree()
	{
		ClearDrawablesZone();
	}

	void YumeRendererEnvironment::ClearDrawablesZone()
	{
		if(octant_ && lastWorldBoundingBox_.Defined())
		{
			YumeVector<YumeDrawable*>::type result;
			BoxOctreeQuery query(result,lastWorldBoundingBox_,DRAWABLE_GEOMETRY | DRAWABLE_ZONE);
			octant_->GetRoot()->GetDrawables(query);

			for(YumeVector<YumeDrawable*>::iterator i = result.begin(); i != result.end(); ++i)
			{
				YumeDrawable* drawable = *i;
				unsigned drawableFlags = drawable->GetDrawableFlags();
				if(drawableFlags & DRAWABLE_GEOMETRY)
					drawable->SetZone(0);
				else if(drawableFlags & DRAWABLE_ZONE)
				{
					YumeRendererEnvironment* zone = static_cast<YumeRendererEnvironment*>(drawable);
					zone->lastAmbientStartZone_.reset();
					zone->lastAmbientEndZone_.reset();
				}
			}
		}

		lastWorldBoundingBox_ = GetWorldBoundingBox();
		lastAmbientStartZone_.reset();
		lastAmbientEndZone_.reset();
	}
}
