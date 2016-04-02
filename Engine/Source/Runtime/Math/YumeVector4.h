



























#ifndef __YumeVector4_h__
#define __YumeVector4_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVector3.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{

	
	class YumeAPIExport Vector4
	{
	public:
		
		Vector4():
			x_(0.0f),
			y_(0.0f),
			z_(0.0f),
			w_(0.0f)
		{
		}

		
		Vector4(const Vector4& vector):
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_),
			w_(vector.w_)
		{
		}

		
		Vector4(const Vector3& vector,float w):
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_),
			w_(w)
		{
		}

		
		Vector4(float x,float y,float z,float w):
			x_(x),
			y_(y),
			z_(z),
			w_(w)
		{
		}

		
		explicit Vector4(const float* data):
			x_(data[0]),
			y_(data[1]),
			z_(data[2]),
			w_(data[3])
		{
		}

		
		Vector4& operator =(const Vector4& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
			w_ = rhs.w_;
			return *this;
		}

		
		bool operator ==(const Vector4& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_ && w_ == rhs.w_; }

		
		bool operator !=(const Vector4& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_ || w_ != rhs.w_; }

		
		Vector4 operator +(const Vector4& rhs) const { return Vector4(x_ + rhs.x_,y_ + rhs.y_,z_ + rhs.z_,w_ + rhs.w_); }

		
		Vector4 operator -() const { return Vector4(-x_,-y_,-z_,-w_); }

		
		Vector4 operator -(const Vector4& rhs) const { return Vector4(x_ - rhs.x_,y_ - rhs.y_,z_ - rhs.z_,w_ - rhs.w_); }

		
		Vector4 operator *(float rhs) const { return Vector4(x_ * rhs,y_ * rhs,z_ * rhs,w_ * rhs); }

		
		Vector4 operator *(const Vector4& rhs) const { return Vector4(x_ * rhs.x_,y_ * rhs.y_,z_ * rhs.z_,w_ * rhs.w_); }

		
		Vector4 operator /(float rhs) const { return Vector4(x_ / rhs,y_ / rhs,z_ / rhs,w_ / rhs); }

		
		Vector4 operator /(const Vector4& rhs) const { return Vector4(x_ / rhs.x_,y_ / rhs.y_,z_ / rhs.z_,w_ / rhs.w_); }

		
		Vector4& operator +=(const Vector4& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			w_ += rhs.w_;
			return *this;
		}

		Vector4& Normalized()
		{
			float len = powf(x_,2) + powf(y_,2) + powf(z_,2);
			len = sqrtf(len);

			x_ /= len;
			y_ /= len;
			z_ /= len;
			w_ /= len;

			return *this;
		} 
		
		Vector4& operator -=(const Vector4& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			w_ -= rhs.w_;
			return *this;
		}

		
		Vector4& operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			w_ *= rhs;
			return *this;
		}

		
		Vector4& operator *=(const Vector4& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			w_ *= rhs.w_;
			return *this;
		}

		
		Vector4& operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			z_ *= invRhs;
			w_ *= invRhs;
			return *this;
		}

		
		Vector4& operator /=(const Vector4& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
			w_ /= rhs.w_;
			return *this;
		}

		
		float DotProduct(const Vector4& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_; }

		
		float AbsDotProduct(const Vector4& rhs) const
		{
			return YumeEngine::Abs(x_ * rhs.x_) + YumeEngine::Abs(y_ * rhs.y_) + YumeEngine::Abs(z_ * rhs.z_) + YumeEngine::Abs(w_ * rhs.w_);
		}

		
		Vector4 Abs() const { return Vector4(YumeEngine::Abs(x_),YumeEngine::Abs(y_),YumeEngine::Abs(z_),YumeEngine::Abs(w_)); }

		
		Vector4 Lerp(const Vector4& rhs,float t) const { return *this * (1.0f - t) + rhs * t; }

		
		bool Equals(const Vector4& rhs) const
		{
			return YumeEngine::Equals(x_,rhs.x_) && YumeEngine::Equals(y_,rhs.y_) && YumeEngine::Equals(z_,rhs.z_) && YumeEngine::Equals(w_,rhs.w_);
		}

		
		bool IsNaN() const { return YumeEngine::IsNaN(x_) || YumeEngine::IsNaN(y_) || YumeEngine::IsNaN(z_) || YumeEngine::IsNaN(w_); }

		
		const float* Data() const { return &x_; }

		
		YumeString ToString() const;

		
		float x_;
		
		float y_;
		
		float z_;
		
		float w_;

		
		static const Vector4 ZERO;
		
		static const Vector4 ONE;
	};

	
	inline Vector4 operator *(float lhs,const Vector4& rhs) { return rhs * lhs; }
}


//--------------------------------------------------------------------------------
#endif
