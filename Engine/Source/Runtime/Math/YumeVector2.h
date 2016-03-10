



























#ifndef __YumeVector2_h__
#define __YumeVector2_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeMath.h"
//--------------------------------------------------------------------------------

namespace YumeEngine
{

	
	class YumeAPIExport Vector2
	{
	public:
		
		Vector2():
			x_(0.0f),
			y_(0.0f)
		{
		}

		
		Vector2(const Vector2& vector):
			x_(vector.x_),
			y_(vector.y_)
		{
		}

		
		Vector2(float x,float y):
			x_(x),
			y_(y)
		{
		}

		
		explicit Vector2(const float* data):
			x_(data[0]),
			y_(data[1])
		{
		}

		
		Vector2& operator =(const Vector2& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			return *this;
		}

		
		bool operator ==(const Vector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

		
		bool operator !=(const Vector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

		
		Vector2 operator +(const Vector2& rhs) const { return Vector2(x_ + rhs.x_,y_ + rhs.y_); }

		
		Vector2 operator -() const { return Vector2(-x_,-y_); }

		
		Vector2 operator -(const Vector2& rhs) const { return Vector2(x_ - rhs.x_,y_ - rhs.y_); }

		
		Vector2 operator *(float rhs) const { return Vector2(x_ * rhs,y_ * rhs); }

		
		Vector2 operator *(const Vector2& rhs) const { return Vector2(x_ * rhs.x_,y_ * rhs.y_); }

		
		Vector2 operator /(float rhs) const { return Vector2(x_ / rhs,y_ / rhs); }

		
		Vector2 operator /(const Vector2& rhs) const { return Vector2(x_ / rhs.x_,y_ / rhs.y_); }

		
		Vector2& operator +=(const Vector2& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			return *this;
		}

		
		Vector2& operator -=(const Vector2& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			return *this;
		}

		
		Vector2& operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			return *this;
		}

		
		Vector2& operator *=(const Vector2& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			return *this;
		}

		
		Vector2& operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			return *this;
		}

		
		Vector2& operator /=(const Vector2& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			return *this;
		}

		
		void Normalize()
		{
			float lenSquared = LengthSquared();
			if(!YumeEngine::Equals(lenSquared,1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				x_ *= invLen;
				y_ *= invLen;
			}
		}

		
		float Length() const { return sqrtf(x_ * x_ + y_ * y_); }

		
		float LengthSquared() const { return x_ * x_ + y_ * y_; }

		
		float DotProduct(const Vector2& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_; }

		
		float AbsDotProduct(const Vector2& rhs) const { return YumeEngine::Abs(x_ * rhs.x_) + YumeEngine::Abs(y_ * rhs.y_); }

		
		float Angle(const Vector2& rhs) const { return Acos(DotProduct(rhs) / (Length() * rhs.Length())); }

		
		Vector2 Abs() const { return Vector2(YumeEngine::Abs(x_),YumeEngine::Abs(y_)); }

		
		Vector2 Lerp(const Vector2& rhs,float t) const { return *this * (1.0f - t) + rhs * t; }

		
		bool Equals(const Vector2& rhs) const { return YumeEngine::Equals(x_,rhs.x_) && YumeEngine::Equals(y_,rhs.y_); }

		
		bool IsNaN() const { return YumeEngine::IsNaN(x_) || YumeEngine::IsNaN(y_); }

		
		Vector2 Normalized() const
		{
			float lenSquared = LengthSquared();
			if(!YumeEngine::Equals(lenSquared,1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				return *this * invLen;
			}
			else
				return *this;
		}

		
		const float* Data() const { return &x_; }

		
		YumeString ToString() const;

		
		float x_;
		
		float y_;

		
		static const Vector2 ZERO;
		
		static const Vector2 LEFT;
		
		static const Vector2 RIGHT;
		
		static const Vector2 UP;
		
		static const Vector2 DOWN;
		
		static const Vector2 ONE;
	};

	
	inline Vector2 operator *(float lhs,const Vector2& rhs) { return rhs * lhs; }

	
	class YumeAPIExport IntVector2
	{
	public:
		
		IntVector2():
			x_(0),
			y_(0)
		{
		}

		
		IntVector2(int x,int y):
			x_(x),
			y_(y)
		{
		}

		
		IntVector2(const int* data):
			x_(data[0]),
			y_(data[1])
		{
		}

		
		IntVector2(const IntVector2& rhs):
			x_(rhs.x_),
			y_(rhs.y_)
		{
		}

		
		IntVector2& operator =(const IntVector2& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			return *this;
		}

		
		bool operator ==(const IntVector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

		
		bool operator !=(const IntVector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

		
		IntVector2 operator +(const IntVector2& rhs) const { return IntVector2(x_ + rhs.x_,y_ + rhs.y_); }

		
		IntVector2 operator -() const { return IntVector2(-x_,-y_); }

		
		IntVector2 operator -(const IntVector2& rhs) const { return IntVector2(x_ - rhs.x_,y_ - rhs.y_); }

		
		IntVector2 operator *(int rhs) const { return IntVector2(x_ * rhs,y_ * rhs); }

		
		IntVector2 operator /(int rhs) const { return IntVector2(x_ / rhs,y_ / rhs); }

		
		IntVector2& operator +=(const IntVector2& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			return *this;
		}

		
		IntVector2& operator -=(const IntVector2& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			return *this;
		}

		
		IntVector2& operator *=(int rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			return *this;
		}

		
		IntVector2& operator /=(int rhs)
		{
			x_ /= rhs;
			y_ /= rhs;
			return *this;
		}

		
		const int* Data() const { return &x_; }

		
		YumeString ToString() const;

		
		int x_;
		
		int y_;

		
		static const IntVector2 ZERO;
	};

	
	inline IntVector2 operator *(int lhs,const IntVector2& rhs) { return rhs * lhs; }


}
#endif
