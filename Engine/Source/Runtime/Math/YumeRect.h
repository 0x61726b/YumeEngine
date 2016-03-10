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
#include "Math/YumeVector4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	
	class YumeAPIExport Rect
	{
	public:
		
		Rect():
			min_(M_INFINITY,M_INFINITY),
			max_(-M_INFINITY,-M_INFINITY)
		{
		}

		
		Rect(const Vector2& min,const Vector2& max):
			min_(min),
			max_(max)
		{
		}

		
		Rect(float left,float top,float right,float bottom):
			min_(left,top),
			max_(right,bottom)
		{
		}

		
		Rect(const Vector4& vector):
			min_(vector.x_,vector.y_),
			max_(vector.z_,vector.w_)
		{
		}

		
		explicit Rect(const float* data):
			min_(data[0],data[1]),
			max_(data[2],data[3])
		{
		}

		
		Rect(const Rect& rect):
			min_(rect.min_),
			max_(rect.max_)
		{
		}

		
		Rect& operator =(const Rect& rhs)
		{
			min_ = rhs.min_;
			max_ = rhs.max_;
			return *this;
		}

		
		bool operator ==(const Rect& rhs) const { return min_ == rhs.min_ && max_ == rhs.max_; }

		
		bool operator !=(const Rect& rhs) const { return min_ != rhs.min_ || max_ != rhs.max_; }

		
		void Define(const Rect& rect)
		{
			min_ = rect.min_;
			max_ = rect.max_;
		}

		
		void Define(const Vector2& min,const Vector2& max)
		{
			min_ = min;
			max_ = max;
		}

		
		void Define(const Vector2& point)
		{
			min_ = max_ = point;
		}

		
		void Merge(const Vector2& point)
		{
			if(point.x_ < min_.x_)
				min_.x_ = point.x_;
			if(point.x_ > max_.x_)
				max_.x_ = point.x_;
			if(point.y_ < min_.y_)
				min_.y_ = point.y_;
			if(point.y_ > max_.y_)
				max_.y_ = point.y_;
		}

		
		void Merge(const Rect& rect)
		{
			if(rect.min_.x_ < min_.x_)
				min_.x_ = rect.min_.x_;
			if(rect.min_.y_ < min_.y_)
				min_.y_ = rect.min_.y_;
			if(rect.max_.x_ > max_.x_)
				max_.x_ = rect.max_.x_;
			if(rect.max_.y_ > max_.y_)
				max_.y_ = rect.max_.y_;
		}

		
		void Clear()
		{
			min_ = Vector2(M_INFINITY,M_INFINITY);
			max_ = Vector2(-M_INFINITY,-M_INFINITY);
		}

		
		void Clip(const Rect& rect);

		
		bool Defined() const
		{
			return min_.x_ != M_INFINITY;
		}

		
		Vector2 Center() const { return (max_ + min_) * 0.5f; }

		
		Vector2 Size() const { return max_ - min_; }

		
		Vector2 HalfSize() const { return (max_ - min_) * 0.5f; }

		
		bool Equals(const Rect& rhs) const { return min_.Equals(rhs.min_) && max_.Equals(rhs.max_); }

		
		Intersection IsInside(const Vector2& point) const
		{
			if(point.x_ < min_.x_ || point.y_ < min_.y_ || point.x_ > max_.x_ || point.y_ > max_.y_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		
		const void* Data() const { return &min_.x_; }

		
		Vector4 ToVector4() const { return Vector4(min_.x_,min_.y_,max_.x_,max_.y_); }

		
		YumeString ToString() const;

		
		Vector2 min_;
		
		Vector2 max_;

		
		static const Rect FULL;
		
		static const Rect POSITIVE;
		
		static const Rect ZERO;
	};

	
	class YumeAPIExport IntRect
	{
	public:
		
		IntRect():
			left_(0),
			top_(0),
			right_(0),
			bottom_(0)
		{
		}

		
		IntRect(int left,int top,int right,int bottom):
			left_(left),
			top_(top),
			right_(right),
			bottom_(bottom)
		{
		}

		
		IntRect(const int* data):
			left_(data[0]),
			top_(data[1]),
			right_(data[2]),
			bottom_(data[3])
		{
		}

		
		bool operator ==(const IntRect& rhs) const
		{
			return left_ == rhs.left_ && top_ == rhs.top_ && right_ == rhs.right_ && bottom_ == rhs.bottom_;
		}

		
		bool operator !=(const IntRect& rhs) const
		{
			return left_ != rhs.left_ || top_ != rhs.top_ || right_ != rhs.right_ || bottom_ != rhs.bottom_;
		}

		
		IntVector2 Size() const { return IntVector2(Width(),Height()); }

		
		int Width() const { return right_ - left_; }

		
		int Height() const { return bottom_ - top_; }

		
		Intersection IsInside(const IntVector2& point) const
		{
			if(point.x_ < left_ || point.y_ < top_ || point.x_ >= right_ || point.y_ >= bottom_)
				return OUTSIDE;
			else
				return INSIDE;
		}

		
		const int* Data() const { return &left_; }

		
		YumeString ToString() const;

		
		int left_;
		
		int top_;
		
		int right_;
		
		int bottom_;

		
		static const IntRect ZERO;
	};

}


//----------------------------------------------------------------------------
#endif
