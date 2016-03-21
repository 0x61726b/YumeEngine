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
#ifndef __YumeOctreeQuery_h__
#define __YumeOctreeQuery_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeDrawable.h"
#include "Math/YumeBoundingBox.h"
#include "Math/YumeSphere.h"
#include "Math/YumeFrustum.h"
#include "Math/YumeRay.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeDrawable;
	class YumeSceneNode;

	
	class YumeAPIExport OctreeQuery
	{
	public:
		
		OctreeQuery(YumePodVector<YumeDrawable*>::type& result,unsigned char drawableFlags,unsigned viewMask):
			result_(result),
			drawableFlags_(drawableFlags),
			viewMask_(viewMask)
		{
		}

		
		virtual ~OctreeQuery()
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside) = 0;
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside) = 0;

		
		YumePodVector<YumeDrawable*>::type& result_;
		
		unsigned char drawableFlags_;
		
		unsigned viewMask_;

	private:
		
		OctreeQuery(const OctreeQuery& rhs);
		
		OctreeQuery& operator =(const OctreeQuery& rhs);
	};

	
	class YumeAPIExport PointOctreeQuery : public OctreeQuery
	{
	public:
		
		PointOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Vector3& point,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			point_(point)
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		
		Vector3 point_;
	};

	
	class YumeAPIExport SphereOctreeQuery : public OctreeQuery
	{
	public:
		
		SphereOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Sphere& sphere,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			sphere_(sphere)
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		
		Sphere sphere_;
	};

	
	class YumeAPIExport BoxOctreeQuery : public OctreeQuery
	{
	public:
		
		BoxOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const BoundingBox& box,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			box_(box)
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		
		BoundingBox box_;
	};

	
	class YumeAPIExport FrustumOctreeQuery : public OctreeQuery
	{
	public:
		
		FrustumOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Frustum& frustum,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			frustum_(frustum)
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		
		Frustum frustum_;
	};

	
	struct YumeAPIExport OctreeQueryResult
	{
		
		OctreeQueryResult():
			drawable_(0),
			node_(0)
		{
		}

		
		bool operator !=(const OctreeQueryResult& rhs) const { return drawable_ != rhs.drawable_ || node_ != rhs.node_; }

		
		YumeDrawable* drawable_;
		
		YumeSceneNode* node_;
	};

	
	enum RayQueryLevel
	{
		RAY_AABB = 0,
		RAY_OBB,
		RAY_TRIANGLE,
		RAY_TRIANGLE_UV
	};

	
	struct YumeAPIExport RayQueryResult
	{
		
		RayQueryResult():
			drawable_(0),
			node_(0)
		{
		}

		
		bool operator !=(const RayQueryResult& rhs) const
		{
			return position_ != rhs.position_ ||
				normal_ != rhs.normal_ ||
				textureUV_ != rhs.textureUV_ ||
				distance_ != rhs.distance_ ||
				drawable_ != rhs.drawable_ ||
				node_ != rhs.node_ ||
				subObject_ != rhs.subObject_;
		}

		
		Vector3 position_;
		
		Vector3 normal_;
		
		Vector2 textureUV_;
		
		float distance_;
		
		YumeDrawable* drawable_;
		
		YumeSceneNode* node_;
		
		unsigned subObject_;
	};

	
	class YumeAPIExport RayOctreeQuery
	{
	public:
		
		RayOctreeQuery(YumePodVector<RayQueryResult>::type& result,const Ray& ray,RayQueryLevel level = RAY_TRIANGLE,
			float maxDistance = M_INFINITY,unsigned char drawableFlags = DRAWABLE_ANY,unsigned viewMask = DEFAULT_VIEWMASK):
			result_(result),
			ray_(ray),
			drawableFlags_(drawableFlags),
			viewMask_(viewMask),
			maxDistance_(maxDistance),
			level_(level)
		{
		}

		
		YumePodVector<RayQueryResult>::type& result_;
		
		Ray ray_;
		
		unsigned char drawableFlags_;
		
		unsigned viewMask_;
		
		float maxDistance_;
		
		RayQueryLevel level_;

	private:
		
		RayOctreeQuery(const RayOctreeQuery& rhs);
		
		RayOctreeQuery& operator =(const RayOctreeQuery& rhs);
	};

	class YumeAPIExport AllContentOctreeQuery : public OctreeQuery
	{
	public:
		
		AllContentOctreeQuery(YumePodVector<YumeDrawable*>::type& result,unsigned char drawableFlags,unsigned viewMask):
			OctreeQuery(result,drawableFlags,viewMask)
		{
		}

		
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);
	};
}


//----------------------------------------------------------------------------
#endif
