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
#ifndef __YumeFrustum_h__
#define __YumeFrustum_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRect.h"
#include "YumeSphere.h"
#include "YumePlane.h"
#include "YumeBoundingBox.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Frustum planes.
	enum FrustumPlane
	{
		PLANE_NEAR = 0,
		PLANE_LEFT,
		PLANE_RIGHT,
		PLANE_UP,
		PLANE_DOWN,
		PLANE_FAR,
	};

	static const unsigned NUM_FRUSTUM_PLANES = 6;
	static const unsigned NUM_FRUSTUM_VERTICES = 8;

	/// Convex constructed of 6 planes.
	class YumeAPIExport Frustum
	{
	public:
		/// Construct a degenerate frustum with all points at origin.
		Frustum();
		/// Copy-construct from another frustum.
		Frustum(const Frustum& frustum);

		/// Assign from another frustum.
		Frustum& operator =(const Frustum& rhs);

		/// Define with projection parameters and a transform matrix.
		void
			Define(float fov,float aspectRatio,float zoom,float nearZ,float farZ,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		/// Define with near and far dimension vectors and a transform matrix.
		void Define(const Vector3& near,const Vector3& far,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		/// Define with a bounding box and a transform matrix.
		void Define(const BoundingBox& box,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		/// Define with orthographic projection parameters and a transform matrix.
		void DefineOrtho
			(float orthoSize,float aspectRatio,float zoom,float nearZ,float farZ,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		/// Transform by a 3x3 matrix.
		void Transform(const Matrix3& transform);
		/// Transform by a 3x4 matrix.
		void Transform(const Matrix3x4& transform);

		/// Test if a point is inside or outside.
		Intersection IsInside(const Vector3& point) const
		{
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				if(planes_[i].Distance(point) < 0.0f)
					return OUTSIDE;
			}

			return INSIDE;
		}

		/// Test if a sphere is inside, outside or intersects.
		Intersection IsInside(const Sphere& sphere) const
		{
			bool allInside = true;
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				float dist = planes_[i].Distance(sphere.center_);
				if(dist < -sphere.radius_)
					return OUTSIDE;
				else if(dist < sphere.radius_)
					allInside = false;
			}

			return allInside ? INSIDE : INTERSECTS;
		}

		/// Test if a sphere if (partially) inside or outside.
		Intersection IsInsideFast(const Sphere& sphere) const
		{
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				if(planes_[i].Distance(sphere.center_) < -sphere.radius_)
					return OUTSIDE;
			}

			return INSIDE;
		}

		/// Test if a bounding box is inside, outside or intersects.
		Intersection IsInside(const BoundingBox& box) const
		{
			Vector3 center = box.Center();
			Vector3 edge = center - box.min_;
			bool allInside = true;

			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				const Plane& plane = planes_[i];
				float dist = plane.normal_.DotProduct(center) + plane.d_;
				float absDist = plane.absNormal_.DotProduct(edge);

				if(dist < -absDist)
					return OUTSIDE;
				else if(dist < absDist)
					allInside = false;
			}

			return allInside ? INSIDE : INTERSECTS;
		}

		/// Test if a bounding box is (partially) inside or outside.
		Intersection IsInsideFast(const BoundingBox& box) const
		{
			Vector3 center = box.Center();
			Vector3 edge = center - box.min_;

			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				const Plane& plane = planes_[i];
				float dist = plane.normal_.DotProduct(center) + plane.d_;
				float absDist = plane.absNormal_.DotProduct(edge);

				if(dist < -absDist)
					return OUTSIDE;
			}

			return INSIDE;
		}

		/// Return distance of a point to the frustum, or 0 if inside.
		float Distance(const Vector3& point) const
		{
			float distance = 0.0f;
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
				distance = Max(-planes_[i].Distance(point),distance);

			return distance;
		}

		/// Return transformed by a 3x3 matrix.
		Frustum Transformed(const Matrix3& transform) const;
		/// Return transformed by a 3x4 matrix.
		Frustum Transformed(const Matrix3x4& transform) const;
		/// Return projected by a 4x4 projection matrix.
		Rect Projected(const Matrix4& transform) const;

		/// Update the planes. Called internally.
		void UpdatePlanes();

		/// Frustum planes.
		Plane planes_[NUM_FRUSTUM_PLANES];
		/// Frustum vertices.
		Vector3 vertices_[NUM_FRUSTUM_VERTICES];
	};
}


//----------------------------------------------------------------------------
#endif
