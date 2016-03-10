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

	
	class YumeAPIExport BoundingBox
	{
	public:
		
		BoundingBox():
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
		}

		
		BoundingBox(const BoundingBox& box):
			min_(box.min_),
			max_(box.max_)
		{
		}

		
		BoundingBox(const Rect& rect):
			min_(Vector3(rect.min_,0.0f)),
			max_(Vector3(rect.max_,0.0f))
		{
		}

		
		BoundingBox(const Vector3& min,const Vector3& max):
			min_(min),
			max_(max)
		{
		}

		
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

		
		BoundingBox(const Vector3* vertices,unsigned count):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(vertices,count);
		}

		
		BoundingBox(const Frustum& frustum):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(frustum);
		}

		
		BoundingBox(const Polyhedron& poly):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(poly);
		}

		
		BoundingBox(const Sphere& sphere):
			min_(M_INFINITY,M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY,-M_INFINITY)
		{
			Define(sphere);
		}

		
		BoundingBox& operator =(const BoundingBox& rhs)
		{
			min_ = rhs.min_;
			max_ = rhs.max_;
			return *this;
		}

		
		BoundingBox& operator =(const Rect& rhs)
		{
			min_ = Vector3(rhs.min_,0.0f);
			max_ = Vector3(rhs.max_,0.0f);
			return *this;
		}

		
		bool operator ==(const BoundingBox& rhs) const { return (min_ == rhs.min_ && max_ == rhs.max_); }

		
		bool operator !=(const BoundingBox& rhs) const { return (min_ != rhs.min_ || max_ != rhs.max_); }

		
		void Define(const BoundingBox& box)
		{
			Define(box.min_,box.max_);
		}

		
		void Define(const Rect& rect)
		{
			Define(Vector3(rect.min_,0.0f),Vector3(rect.max_,0.0f));
		}

		
		void Define(const Vector3& min,const Vector3& max)
		{
			min_ = min;
			max_ = max;
		}

		
		void Define(float min,float max)
		{
			min_ = Vector3(min,min,min);
			max_ = Vector3(max,max,max);
		}

		
		void Define(const Vector3& point)
		{
			min_ = max_ = point;
		}

		
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

		
		void Define(const Vector3* vertices,unsigned count);
		
		void Define(const Frustum& frustum);
		
		void Define(const Polyhedron& poly);
		
		void Define(const Sphere& sphere);
		
		void Merge(const Vector3* vertices,unsigned count);
		
		void Merge(const Frustum& frustum);
		
		void Merge(const Polyhedron& poly);
		
		void Merge(const Sphere& sphere);
		
		void Clip(const BoundingBox& box);
		
		void Transform(const Matrix3& transform);
		
		void Transform(const Matrix3x4& transform);

		
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

		
		bool Defined() const
		{
			return min_.x_ != M_INFINITY;
		}

		
		Vector3 Center() const { return (max_ + min_) * 0.5f; }

		
		Vector3 Size() const { return max_ - min_; }

		
		Vector3 HalfSize() const { return (max_ - min_) * 0.5f; }

		
		BoundingBox Transformed(const Matrix3& transform) const;
		
		BoundingBox Transformed(const Matrix3x4& transform) const;
		
		Rect Projected(const Matrix4& projection) const;

		
		Intersection IsInside(const Vector3& point) const
		{
			if(point.x_ < min_.x_ || point.x_ > max_.x_ || point.y_ < min_.y_ || point.y_ > max_.y_ ||
				point.z_ < min_.z_ || point.z_ > max_.z_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		
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

		
		Intersection IsInsideFast(const BoundingBox& box) const
		{
			if(box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ || box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_ ||
				box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		
		Intersection IsInside(const Sphere& sphere) const;
		
		Intersection IsInsideFast(const Sphere& sphere) const;

		
		YumeString ToString() const;

		
		Vector3 min_;
		float dummyMin_; // This is never used, but exists to pad the min_ value to four floats.
		
		Vector3 max_;
		float dummyMax_; // This is never used, but exists to pad the max_ value to four floats.
	};
}


//----------------------------------------------------------------------------
#endif
