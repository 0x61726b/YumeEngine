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
#ifndef __YumeBoundingBox_h__
#define __YumeBoundingBox_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeRect.h"
#include "Math/YumeVector3.h"

#ifdef YUME_SSE
#include <emmintrin.h>
#endif

//----------------------------------------------------------------------------
namespace YumeEngine
{

	class Polyhedron;
	class Frustum;
	class Matrix3;
	class Matrix4;
	class Matrix3x4;
	class Sphere;

	/// Three-dimensional axis-aligned bounding box.
	class YumeAPIExport BoundingBox
	{
	public:
		/// Construct with zero size.
		BoundingBox():
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
		}

		/// Copy-construct from another bounding box.
		BoundingBox(const BoundingBox& box):
			min_(box.min_),
			max_(box.max_)
		{
		}

		/// Construct from a rect, with the Z dimension left zero.
		BoundingBox(const Rect& rect):
			min_(Vector3(rect.min_,0.0f)),
			max_(Vector3(rect.max_,0.0f))
		{
		}

		/// Construct from minimum and maximum vectors.
		BoundingBox(const Vector3& min,const Vector3& max):
			min_(min),
			max_(max)
		{
		}

		/// Construct from minimum and maximum floats (all dimensions same.)
		BoundingBox(float min,float max):
			min_(Vector3(min,min,min)),
			max_(Vector3(max,max,max))
		{
		}

#ifdef YUME_SSE
		BoundingBox(__m128 min,__m128 max)
		{
			_mm_storeu_ps(&min_.x_,min);
			_mm_storeu_ps(&max_.x_,max);
		}
#endif

		/// Construct from an array of vertices.
		BoundingBox(const Vector3* vertices,unsigned count):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(vertices,count);
		}

		/// Construct from a frustum.
		BoundingBox(const Frustum& frustum):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(frustum);
		}

		/// Construct from a polyhedron.
		BoundingBox(const Polyhedron& poly):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(poly);
		}

		/// Construct from a sphere.
		BoundingBox(const Sphere& sphere):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(sphere);
		}

		/// Assign from another bounding box.
		BoundingBox& operator =(const BoundingBox& rhs)
		{
			min_ = rhs.min_;
			max_ = rhs.max_;
			return *this;
		}

		/// Assign from a Rect, with the Z dimension left zero.
		BoundingBox& operator =(const Rect& rhs)
		{
			min_ = Vector3(rhs.min_,0.0f);
			max_ = Vector3(rhs.max_,0.0f);
			return *this;
		}

		/// Test for equality with another bounding box.
		bool operator ==(const BoundingBox& rhs) const { return (min_ == rhs.min_ && max_ == rhs.max_); }

		/// Test for inequality with another bounding box.
		bool operator !=(const BoundingBox& rhs) const { return (min_ != rhs.min_ || max_ != rhs.max_); }

		/// Define from another bounding box.
		void Define(const BoundingBox& box)
		{
			Define(box.min_,box.max_);
		}

		/// Define from a Rect.
		void Define(const Rect& rect)
		{
			Define(Vector3(rect.min_,0.0f),Vector3(rect.max_,0.0f));
		}

		/// Define from minimum and maximum vectors.
		void Define(const Vector3& min,const Vector3& max)
		{
			min_ = min;
			max_ = max;
		}

		/// Define from minimum and maximum floats (all dimensions same.)
		void Define(float min,float max)
		{
			min_ = Vector3(min,min,min);
			max_ = Vector3(max,max,max);
		}

		/// Define from a point.
		void Define(const Vector3& point)
		{
			min_ = max_ = point;
		}

		/// Merge a point.
		void Merge(const Vector3& point)
		{
#ifdef YUME_SSE
			__m128 vec = _mm_set_ps(1.f,point.z_,point.y_,point.x_);
			_mm_storeu_ps(&min_.x_,_mm_min_ps(_mm_loadu_ps(&min_.x_),vec));
			_mm_storeu_ps(&max_.x_,_mm_max_ps(_mm_loadu_ps(&max_.x_),vec));
#else
			if(point.x_ < min_.x_)
				min_.x_ = point.x_;
			if(point.y_ < min_.y_)
				min_.y_ = point.y_;
			if(point.z_ < min_.z_)
				min_.z_ = point.z_;
			if(point.x_ > max_.x_)
				max_.x_ = point.x_;
			if(point.y_ > max_.y_)
				max_.y_ = point.y_;
			if(point.z_ > max_.z_)
				max_.z_ = point.z_;
#endif
		}

		/// Merge another bounding box.
		void Merge(const BoundingBox& box)
		{
#ifdef YUME_SSE
			_mm_storeu_ps(&min_.x_,_mm_min_ps(_mm_loadu_ps(&min_.x_),_mm_loadu_ps(&box.min_.x_)));
			_mm_storeu_ps(&max_.x_,_mm_max_ps(_mm_loadu_ps(&max_.x_),_mm_loadu_ps(&box.max_.x_)));
#else
			if(box.min_.x_ < min_.x_)
				min_.x_ = box.min_.x_;
			if(box.min_.y_ < min_.y_)
				min_.y_ = box.min_.y_;
			if(box.min_.z_ < min_.z_)
				min_.z_ = box.min_.z_;
			if(box.max_.x_ > max_.x_)
				max_.x_ = box.max_.x_;
			if(box.max_.y_ > max_.y_)
				max_.y_ = box.max_.y_;
			if(box.max_.z_ > max_.z_)
				max_.z_ = box.max_.z_;
#endif
		}

		/// Define from an array of vertices.
		void Define(const Vector3* vertices,unsigned count);
		/// Define from a frustum.
		void Define(const Frustum& frustum);
		/// Define from a polyhedron.
		void Define(const Polyhedron& poly);
		/// Define from a sphere.
		void Define(const Sphere& sphere);
		/// Merge an array of vertices.
		void Merge(const Vector3* vertices,unsigned count);
		/// Merge a frustum.
		void Merge(const Frustum& frustum);
		/// Merge a polyhedron.
		void Merge(const Polyhedron& poly);
		/// Merge a sphere.
		void Merge(const Sphere& sphere);
		/// Clip with another bounding box. The box can become degenerate (undefined) as a result.
		void Clip(const BoundingBox& box);
		/// Transform with a 3x3 matrix.
		void Transform(const Matrix3& transform);
		/// Transform with a 3x4 matrix.
		void Transform(const Matrix3x4& transform);

		/// Clear to undefined state.
		void Clear()
		{
#ifdef YUME_SSE
			_mm_storeu_ps(&min_.x_,_mm_set1_ps(M_INFINITY));
			_mm_storeu_ps(&max_.x_,_mm_set1_ps(-M_INFINITY));
#else
			min_ = Vector3(M_INFINITY,M_INFINITY,M_INFINITY);
			max_ = Vector3(-M_INFINITY,-M_INFINITY,-M_INFINITY);
#endif
		}

		/// Return true if this bounding box is defined via a previous call to Define() or Merge().
		bool Defined() const
		{
			return min_.x_ != M_INFINITY;
		}

		/// Return center.
		Vector3 Center() const { return (max_ + min_) * 0.5f; }

		/// Return size.
		Vector3 Size() const { return max_ - min_; }

		/// Return half-size.
		Vector3 HalfSize() const { return (max_ - min_) * 0.5f; }

		/// Return transformed by a 3x3 matrix.
		BoundingBox Transformed(const Matrix3& transform) const;
		/// Return transformed by a 3x4 matrix.
		BoundingBox Transformed(const Matrix3x4& transform) const;
		/// Return projected by a 4x4 projection matrix.
		Rect Projected(const Matrix4& projection) const;

		/// Test if a point is inside.
		Intersection IsInside(const Vector3& point) const
		{
			if(point.x_ < min_.x_ || point.x_ > max_.x_ || point.y_ < min_.y_ || point.y_ > max_.y_ ||
				point.z_ < min_.z_ || point.z_ > max_.z_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		/// Test if another bounding box is inside, outside or intersects.
		Intersection IsInside(const BoundingBox& box) const
		{
			if(box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ || box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_ ||
				box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
				return OUTSIDE;
			else if(box.min_.x_ < min_.x_ || box.max_.x_ > max_.x_ || box.min_.y_ < min_.y_ || box.max_.y_ > max_.y_ ||
				box.min_.z_ < min_.z_ || box.max_.z_ > max_.z_)
				return INTERSECTS;
			else
				return INSIDE;
		}

		/// Test if another bounding box is (partially) inside or outside.
		Intersection IsInsideFast(const BoundingBox& box) const
		{
			if(box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ || box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_ ||
				box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		/// Test if a sphere is inside, outside or intersects.
		Intersection IsInside(const Sphere& sphere) const;
		/// Test if a sphere is (partially) inside or outside.
		Intersection IsInsideFast(const Sphere& sphere) const;

		/// Return as string.
		YumeString ToString() const;

		/// Minimum vector.
		Vector3 min_;
		float dummyMin_; // This is never used, but exists to pad the min_ value to four floats.
		/// Maximum vector.
		Vector3 max_;
		float dummyMax_; // This is never used, but exists to pad the max_ value to four floats.
	};
}


//----------------------------------------------------------------------------
#endif
