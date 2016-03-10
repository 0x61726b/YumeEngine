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

	
	class YumeAPIExport Frustum
	{
	public:
		
		Frustum();
		
		Frustum(const Frustum& frustum);

		
		Frustum& operator =(const Frustum& rhs);

		
		void
			Define(float fov,float aspectRatio,float zoom,float nearZ,float farZ,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		
		void Define(const Vector3& near,const Vector3& far,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		
		void Define(const BoundingBox& box,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		
		void DefineOrtho
			(float orthoSize,float aspectRatio,float zoom,float nearZ,float farZ,const Matrix3x4& transform = Matrix3x4::IDENTITY);
		
		void Transform(const Matrix3& transform);
		
		void Transform(const Matrix3x4& transform);

		
		Intersection IsInside(const Vector3& point) const
		{
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				if(planes_[i].Distance(point) < 0.0f)
					return OUTSIDE;
			}

			return INSIDE;
		}

		
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

		
		Intersection IsInsideFast(const Sphere& sphere) const
		{
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			{
				if(planes_[i].Distance(sphere.center_) < -sphere.radius_)
					return OUTSIDE;
			}

			return INSIDE;
		}

		
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

		
		float Distance(const Vector3& point) const
		{
			float distance = 0.0f;
			for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
				distance = Max(-planes_[i].Distance(point),distance);

			return distance;
		}

		
		Frustum Transformed(const Matrix3& transform) const;
		
		Frustum Transformed(const Matrix3x4& transform) const;
		
		Rect Projected(const Matrix4& transform) const;

		
		void UpdatePlanes();

		
		Plane planes_[NUM_FRUSTUM_PLANES];
		
		Vector3 vertices_[NUM_FRUSTUM_VERTICES];
	};
}


//----------------------------------------------------------------------------
#endif
