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
#ifndef __YumeRect_h__
#define __YumeRect_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Math/YumeMath.h"

#include "Math/YumeVector2.h"
#include "Math/YumeVector3.h"
#include "Math/YumeVector4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Two-dimensional bounding rectangle.
	class YumeAPIExport Rect
	{
	public:
		/// Construct an undefined rect.
		Rect():
			min_(Math::POS_INFINITY,Math::POS_INFINITY),
			max_(Math::NEG_INFINITY,Math::NEG_INFINITY)
		{
		}

		/// Construct from minimum and maximum vectors.
		Rect(const Vector2& min,const Vector2& max):
			min_(min),
			max_(max)
		{
		}

		/// Construct from coordinates.
		Rect(float left,float top,float right,float bottom):
			min_(left,top),
			max_(right,bottom)
		{
		}

		/// Construct from a Vector4.
		Rect(const Vector4& vector):
			min_(vector.x,vector.y),
			max_(vector.z,vector.w)
		{
		}

		/// Construct from a float array.
		explicit Rect(const float* data):
			min_(data[0],data[1]),
			max_(data[2],data[3])
		{
		}

		/// Copy-construct from another rect.
		Rect(const Rect& rect):
			min_(rect.min_),
			max_(rect.max_)
		{
		}

		/// Assign from another rect.
		Rect& operator =(const Rect& rhs)
		{
			min_ = rhs.min_;
			max_ = rhs.max_;
			return *this;
		}

		/// Test for equality with another rect.
		bool operator ==(const Rect& rhs) const { return min_ == rhs.min_ && max_ == rhs.max_; }

		/// Test for inequality with another rect.
		bool operator !=(const Rect& rhs) const { return min_ != rhs.min_ || max_ != rhs.max_; }

		/// Define from another rect.
		void Define(const Rect& rect)
		{
			min_ = rect.min_;
			max_ = rect.max_;
		}

		/// Define from minimum and maximum vectors.
		void Define(const Vector2& min,const Vector2& max)
		{
			min_ = min;
			max_ = max;
		}

		/// Define from a point.
		void Define(const Vector2& point)
		{
			min_ = max_ = point;
		}

		/// Merge a point.
		void Merge(const Vector2& point)
		{
			if(point.x < min_.x)
				min_.x = point.x;
			if(point.x > max_.x)
				max_.x = point.x;
			if(point.y < min_.y)
				min_.y = point.y;
			if(point.y > max_.y)
				max_.y = point.y;
		}

		/// Merge a rect.
		void Merge(const Rect& rect)
		{
			if(rect.min_.x < min_.x)
				min_.x = rect.min_.x;
			if(rect.min_.y < min_.y)
				min_.y = rect.min_.y;
			if(rect.max_.x > max_.x)
				max_.x = rect.max_.x;
			if(rect.max_.y > max_.y)
				max_.y = rect.max_.y;
		}

		/// Clear to undefined state.
		void Clear()
		{
			min_ = Vector2(Math::POS_INFINITY,Math::POS_INFINITY);
			max_ = Vector2(Math::NEG_INFINITY,Math::NEG_INFINITY);
		}

		/// Clip with another rect.
		void Clip(const Rect& rect);

		/// Return true if this rect is defined via a previous call to Define() or Merge().
		bool Defined() const
		{
			return min_.x != Math::POS_INFINITY;
		}

		/// Return center.
		Vector2 Center() const { return (max_ + min_) * 0.5f; }

		/// Return size.
		Vector2 Size() const { return max_ - min_; }

		/// Return half-size.
		Vector2 HalfSize() const { return (max_ - min_) * 0.5f; }

		/// Test for equality with another rect with epsilon.
		bool Equals(const Rect& rhs) const { return min_ == (rhs.min_) && max_ == (rhs.max_); }

		/// Test whether a point is inside.
		Intersection IsInside(const Vector2& point) const
		{
			if(point.x < min_.x || point.y < min_.y || point.x > max_.x || point.y > max_.y)
				return OUTSIDE;
			else
				return INSIDE;
		}

		/// Return float data.
		const void* Data() const { return &min_.x; }

		/// Return as a vector.
		Vector4 ToVector4() const { return Vector4(min_.x,min_.y,max_.x,max_.y); }

		/// Return as string.
		YumeString ToString() const;

		/// Minimum vector.
		Vector2 min_;
		/// Maximum vector.
		Vector2 max_;

		/// Rect in the range (-1, -1) - (1, 1)
		static const Rect FULL;
		/// Rect in the range (0, 0) - (1, 1)
		static const Rect POSITIVE;
		/// Zero-sized rect.
		static const Rect ZERO;
	};

	/// Two-dimensional bounding rectangle with integer values.
	class YumeAPIExport IntRect
	{
	public:
		/// Construct a zero rect.
		IntRect():
			left_(0),
			top_(0),
			right_(0),
			bottom_(0)
		{
		}

		/// Construct from coordinates.
		IntRect(int left,int top,int right,int bottom):
			left_(left),
			top_(top),
			right_(right),
			bottom_(bottom)
		{
		}

		/// Construct from an int array.
		IntRect(const int* data):
			left_(data[0]),
			top_(data[1]),
			right_(data[2]),
			bottom_(data[3])
		{
		}

		/// Test for equality with another rect.
		bool operator ==(const IntRect& rhs) const
		{
			return left_ == rhs.left_ && top_ == rhs.top_ && right_ == rhs.right_ && bottom_ == rhs.bottom_;
		}

		/// Test for inequality with another rect.
		bool operator !=(const IntRect& rhs) const
		{
			return left_ != rhs.left_ || top_ != rhs.top_ || right_ != rhs.right_ || bottom_ != rhs.bottom_;
		}

		/// Return size.
		Vector2 Size() const { return Vector2(Width(),Height()); }

		/// Return width.
		int Width() const { return right_ - left_; }

		/// Return height.
		int Height() const { return bottom_ - top_; }

		/// Test whether a point is inside.
		Intersection IsInside(const Vector2& point) const
		{
			if(point.x < left_ || point.y < top_ || point.x >= right_ || point.y >= bottom_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		/// Return integer data.
		const int* Data() const { return &left_; }

		/// Return as string.
		YumeString ToString() const;

		/// Left coordinate.
		int left_;
		/// Top coordinate.
		int top_;
		/// Right coordinate.
		int right_;
		/// Bottom coordinate.
		int bottom_;

		/// Zero-sized rect.
		static const IntRect ZERO;
	};
}


//----------------------------------------------------------------------------
#endif
