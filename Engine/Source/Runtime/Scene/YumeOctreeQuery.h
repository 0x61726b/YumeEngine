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

	/// Base class for octree queries.
	class YumeAPIExport OctreeQuery
	{
	public:
		/// Construct with query parameters.
		OctreeQuery(YumeVector<YumeDrawable*>::type& result,unsigned char drawableFlags,unsigned viewMask):
			result_(result),
			drawableFlags_(drawableFlags),
			viewMask_(viewMask)
		{
		}

		/// Destruct.
		virtual ~OctreeQuery()
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside) = 0;
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside) = 0;

		/// Result vector reference.
		YumeVector<YumeDrawable*>::type& result_;
		/// Drawable flags to include.
		unsigned char drawableFlags_;
		/// Drawable layers to include.
		unsigned viewMask_;

	private:
		/// Prevent copy construction.
		OctreeQuery(const OctreeQuery& rhs);
		/// Prevent assignment.
		OctreeQuery& operator =(const OctreeQuery& rhs);
	};

	/// Point octree query.
	class YumeAPIExport PointOctreeQuery : public OctreeQuery
	{
	public:
		/// Construct with point and query parameters.
		PointOctreeQuery(YumeVector<YumeDrawable*>::type& result,const Vector3& point,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			point_(point)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		/// Point.
		Vector3 point_;
	};

	/// %Sphere octree query.
	class YumeAPIExport SphereOctreeQuery : public OctreeQuery
	{
	public:
		/// Construct with sphere and query parameters.
		SphereOctreeQuery(YumeVector<YumeDrawable*>::type& result,const Sphere& sphere,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			sphere_(sphere)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		/// Sphere.
		Sphere sphere_;
	};

	/// Bounding box octree query.
	class YumeAPIExport BoxOctreeQuery : public OctreeQuery
	{
	public:
		/// Construct with bounding box and query parameters.
		BoxOctreeQuery(YumeVector<YumeDrawable*>::type& result,const BoundingBox& box,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			box_(box)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		/// Bounding box.
		BoundingBox box_;
	};

	/// %Frustum octree query.
	class YumeAPIExport FrustumOctreeQuery : public OctreeQuery
	{
	public:
		/// Construct with frustum and query parameters.
		FrustumOctreeQuery(YumeVector<YumeDrawable*>::type& result,const Frustum& frustum,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			OctreeQuery(result,drawableFlags,viewMask),
			frustum_(frustum)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);

		/// Frustum.
		Frustum frustum_;
	};

	/// General octree query result. Used for Lua bindings only.
	struct YumeAPIExport OctreeQueryResult
	{
		/// Construct with defaults.
		OctreeQueryResult():
			drawable_(0),
			node_(0)
		{
		}

		/// Test for inequality, added to prevent GCC from complaining.
		bool operator !=(const OctreeQueryResult& rhs) const { return drawable_ != rhs.drawable_ || node_ != rhs.node_; }

		/// Drawable.
		YumeDrawable* drawable_;
		/// Scene node.
		YumeSceneNode* node_;
	};

	/// Graphics raycast detail level.
	enum RayQueryLevel
	{
		RAY_AABB = 0,
		RAY_OBB,
		RAY_TRIANGLE,
		RAY_TRIANGLE_UV
	};

	/// Raycast result.
	struct YumeAPIExport RayQueryResult
	{
		/// Construct with defaults.
		RayQueryResult():
			drawable_(0),
			node_(0)
		{
		}

		/// Test for inequality, added to prevent GCC from complaining.
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

		/// Hit position in world space.
		Vector3 position_;
		/// Hit normal in world space. Negation of ray direction if per-triangle data not available.
		Vector3 normal_;
		/// Hit texture position
		Vector2 textureUV_;
		/// Distance from ray origin.
		float distance_;
		/// Drawable.
		YumeDrawable* drawable_;
		/// Scene node.
		YumeSceneNode* node_;
		/// Drawable specific subobject if applicable.
		unsigned subObject_;
	};

	/// Raycast octree query.
	class YumeAPIExport RayOctreeQuery
	{
	public:
		/// Construct with ray and query parameters.
		RayOctreeQuery(YumeVector<RayQueryResult>::type& result,const Ray& ray,RayQueryLevel level = RAY_TRIANGLE,
			float maxDistance = M_INFINITY,unsigned char drawableFlags = DRAWABLE_ANY,unsigned viewMask = DEFAULT_VIEWMASK):
			result_(result),
			ray_(ray),
			drawableFlags_(drawableFlags),
			viewMask_(viewMask),
			maxDistance_(maxDistance),
			level_(level)
		{
		}

		/// Result vector reference.
		YumeVector<RayQueryResult>::type& result_;
		/// Ray.
		Ray ray_;
		/// Drawable flags to include.
		unsigned char drawableFlags_;
		/// Drawable layers to include.
		unsigned viewMask_;
		/// Maximum ray distance.
		float maxDistance_;
		/// Raycast detail level.
		RayQueryLevel level_;

	private:
		/// Prevent copy construction.
		RayOctreeQuery(const RayOctreeQuery& rhs);
		/// Prevent assignment.
		RayOctreeQuery& operator =(const RayOctreeQuery& rhs);
	};

	class YumeAPIExport AllContentOctreeQuery : public OctreeQuery
	{
	public:
		/// Construct.
		AllContentOctreeQuery(YumeVector<YumeDrawable*>::type& result,unsigned char drawableFlags,unsigned viewMask):
			OctreeQuery(result,drawableFlags,viewMask)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside);
		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside);
	};
}


//----------------------------------------------------------------------------
#endif
