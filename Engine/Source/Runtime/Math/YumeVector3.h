



























#ifndef __YumeVector3_h__
#define __YumeVector3_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVector2.h"
//--------------------------------------------------------------------------------

namespace YumeEngine
{

	
	class YumeAPIExport Vector3
	{
	public:
		
		Vector3():
			x_(0.0f),
			y_(0.0f),
			z_(0.0f)
		{
		}

		
		Vector3(const Vector3& vector):
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_)
		{
		}

		
		Vector3(const Vector2& vector,float z):
			x_(vector.x_),
			y_(vector.y_),
			z_(z)
		{
		}

		
		Vector3(const Vector2& vector):
			x_(vector.x_),
			y_(vector.y_),
			z_(0.0f)
		{
		}

		
		Vector3(float x,float y,float z):
			x_(x),
			y_(y),
			z_(z)
		{
		}

		
		Vector3(float x,float y):
			x_(x),
			y_(y),
			z_(0.0f)
		{
		}

		
		explicit Vector3(const float* data):
			x_(data[0]),
			y_(data[1]),
			z_(data[2])
		{
		}

		
		Vector3& operator =(const Vector3& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
			return *this;
		}

		
		bool operator ==(const Vector3& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_; }

		
		bool operator !=(const Vector3& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_; }

		
		Vector3 operator +(const Vector3& rhs) const { return Vector3(x_ + rhs.x_,y_ + rhs.y_,z_ + rhs.z_); }

		
		Vector3 operator -() const { return Vector3(-x_,-y_,-z_); }

		
		Vector3 operator -(const Vector3& rhs) const { return Vector3(x_ - rhs.x_,y_ - rhs.y_,z_ - rhs.z_); }

		
		Vector3 operator *(float rhs) const { return Vector3(x_ * rhs,y_ * rhs,z_ * rhs); }

		
		Vector3 operator *(const Vector3& rhs) const { return Vector3(x_ * rhs.x_,y_ * rhs.y_,z_ * rhs.z_); }

		
		Vector3 operator /(float rhs) const { return Vector3(x_ / rhs,y_ / rhs,z_ / rhs); }

		
		Vector3 operator /(const Vector3& rhs) const { return Vector3(x_ / rhs.x_,y_ / rhs.y_,z_ / rhs.z_); }

		
		Vector3& operator +=(const Vector3& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			return *this;
		}

		
		Vector3& operator -=(const Vector3& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			return *this;
		}

		
		Vector3& operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			return *this;
		}

		
		Vector3& operator *=(const Vector3& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			return *this;
		}

		
		Vector3& operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			z_ *= invRhs;
			return *this;
		}

		
		Vector3& operator /=(const Vector3& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
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
				z_ *= invLen;
			}
		}

		
		float Length() const { return sqrtf(x_ * x_ + y_ * y_ + z_ * z_); }

		
		float LengthSquared() const { return x_ * x_ + y_ * y_ + z_ * z_; }

		
		float DotProduct(const Vector3& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_; }

		
		float AbsDotProduct(const Vector3& rhs) const
		{
			return YumeEngine::Abs(x_ * rhs.x_) + YumeEngine::Abs(y_ * rhs.y_) + YumeEngine::Abs(z_ * rhs.z_);
		}

		
		Vector3 CrossProduct(const Vector3& rhs) const
		{
			return Vector3(
				y_ * rhs.z_ - z_ * rhs.y_,
				z_ * rhs.x_ - x_ * rhs.z_,
				x_ * rhs.y_ - y_ * rhs.x_
				);
		}

		
		Vector3 Abs() const { return Vector3(YumeEngine::Abs(x_),YumeEngine::Abs(y_),YumeEngine::Abs(z_)); }

		
		Vector3 Lerp(const Vector3& rhs,float t) const { return *this * (1.0f - t) + rhs * t; }

		
		bool Equals(const Vector3& rhs) const
		{
			return YumeEngine::Equals(x_,rhs.x_) && YumeEngine::Equals(y_,rhs.y_) && YumeEngine::Equals(z_,rhs.z_);
		}

		
		float Angle(const Vector3& rhs) const { return YumeEngine::Acos(DotProduct(rhs) / (Length() * rhs.Length())); }

		
		bool IsNaN() const { return YumeEngine::IsNaN(x_) || YumeEngine::IsNaN(y_) || YumeEngine::IsNaN(z_); }

		
		Vector3 Normalized() const
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
		
		float z_;

		
		static const Vector3 ZERO;
		
		static const Vector3 LEFT;
		
		static const Vector3 RIGHT;
		
		static const Vector3 UP;
		
		static const Vector3 DOWN;
		
		static const Vector3 FORWARD;
		
		static const Vector3 BACK;
		
		static const Vector3 ONE;
	};

	
	inline Vector3 operator *(float lhs,const Vector3& rhs) { return rhs * lhs; }
}

//--------------------------------------------------------------------------------
#endif
