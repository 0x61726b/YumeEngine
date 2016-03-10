



























#ifndef __YumeMatrix3_h__
#define __YumeMatrix3_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVector3.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{

	
	class YumeAPIExport Matrix3
	{
	public:
		
		Matrix3():
			m00_(1.0f),
			m01_(0.0f),
			m02_(0.0f),
			m10_(0.0f),
			m11_(1.0f),
			m12_(0.0f),
			m20_(0.0f),
			m21_(0.0f),
			m22_(1.0f)
		{
		}

		
		Matrix3(const Matrix3& matrix):
			m00_(matrix.m00_),
			m01_(matrix.m01_),
			m02_(matrix.m02_),
			m10_(matrix.m10_),
			m11_(matrix.m11_),
			m12_(matrix.m12_),
			m20_(matrix.m20_),
			m21_(matrix.m21_),
			m22_(matrix.m22_)
		{
		}

		
		Matrix3(float v00,float v01,float v02,
			float v10,float v11,float v12,
			float v20,float v21,float v22):
			m00_(v00),
			m01_(v01),
			m02_(v02),
			m10_(v10),
			m11_(v11),
			m12_(v12),
			m20_(v20),
			m21_(v21),
			m22_(v22)
		{
		}

		
		explicit Matrix3(const float* data):
			m00_(data[0]),
			m01_(data[1]),
			m02_(data[2]),
			m10_(data[3]),
			m11_(data[4]),
			m12_(data[5]),
			m20_(data[6]),
			m21_(data[7]),
			m22_(data[8])
		{
		}

		
		Matrix3& operator =(const Matrix3& rhs)
		{
			m00_ = rhs.m00_;
			m01_ = rhs.m01_;
			m02_ = rhs.m02_;
			m10_ = rhs.m10_;
			m11_ = rhs.m11_;
			m12_ = rhs.m12_;
			m20_ = rhs.m20_;
			m21_ = rhs.m21_;
			m22_ = rhs.m22_;
			return *this;
		}

		
		bool operator ==(const Matrix3& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();

			for(unsigned i = 0; i < 9; ++i)
			{
				if(leftData[i] != rightData[i])
					return false;
			}

			return true;
		}

		
		bool operator !=(const Matrix3& rhs) const { return !(*this == rhs); }

		
		Vector3 operator *(const Vector3& rhs) const
		{
			return Vector3(
				m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_,
				m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_,
				m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_
				);
		}

		
		Matrix3 operator +(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ + rhs.m00_,
				m01_ + rhs.m01_,
				m02_ + rhs.m02_,
				m10_ + rhs.m10_,
				m11_ + rhs.m11_,
				m12_ + rhs.m12_,
				m20_ + rhs.m20_,
				m21_ + rhs.m21_,
				m22_ + rhs.m22_
				);
		}

		
		Matrix3 operator -(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ - rhs.m00_,
				m01_ - rhs.m01_,
				m02_ - rhs.m02_,
				m10_ - rhs.m10_,
				m11_ - rhs.m11_,
				m12_ - rhs.m12_,
				m20_ - rhs.m20_,
				m21_ - rhs.m21_,
				m22_ - rhs.m22_
				);
		}

		
		Matrix3 operator *(float rhs) const
		{
			return Matrix3(
				m00_ * rhs,
				m01_ * rhs,
				m02_ * rhs,
				m10_ * rhs,
				m11_ * rhs,
				m12_ * rhs,
				m20_ * rhs,
				m21_ * rhs,
				m22_ * rhs
				);
		}

		
		Matrix3 operator *(const Matrix3& rhs) const
		{
			return Matrix3(
				m00_ * rhs.m00_ + m01_ * rhs.m10_ + m02_ * rhs.m20_,
				m00_ * rhs.m01_ + m01_ * rhs.m11_ + m02_ * rhs.m21_,
				m00_ * rhs.m02_ + m01_ * rhs.m12_ + m02_ * rhs.m22_,
				m10_ * rhs.m00_ + m11_ * rhs.m10_ + m12_ * rhs.m20_,
				m10_ * rhs.m01_ + m11_ * rhs.m11_ + m12_ * rhs.m21_,
				m10_ * rhs.m02_ + m11_ * rhs.m12_ + m12_ * rhs.m22_,
				m20_ * rhs.m00_ + m21_ * rhs.m10_ + m22_ * rhs.m20_,
				m20_ * rhs.m01_ + m21_ * rhs.m11_ + m22_ * rhs.m21_,
				m20_ * rhs.m02_ + m21_ * rhs.m12_ + m22_ * rhs.m22_
				);
		}

		
		void SetScale(const Vector3& scale)
		{
			m00_ = scale.x_;
			m11_ = scale.y_;
			m22_ = scale.z_;
		}

		
		void SetScale(float scale)
		{
			m00_ = scale;
			m11_ = scale;
			m22_ = scale;
		}

		
		Vector3 Scale() const
		{
			return Vector3(
				sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
				sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
				);
		}

		
		Matrix3 Transpose() const
		{
			return Matrix3(
				m00_,
				m10_,
				m20_,
				m01_,
				m11_,
				m21_,
				m02_,
				m12_,
				m22_
				);
		}

		
		Matrix3 Scaled(const Vector3& scale) const
		{
			return Matrix3(
				m00_ * scale.x_,
				m01_ * scale.y_,
				m02_ * scale.z_,
				m10_ * scale.x_,
				m11_ * scale.y_,
				m12_ * scale.z_,
				m20_ * scale.x_,
				m21_ * scale.y_,
				m22_ * scale.z_
				);
		}

		
		bool Equals(const Matrix3& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();

			for(unsigned i = 0; i < 9; ++i)
			{
				if(!YumeEngine::Equals(leftData[i],rightData[i]))
					return false;
			}

			return true;
		}

		
		Matrix3 Inverse() const;

		
		const float* Data() const { return &m00_; }

		
		YumeString ToString() const;

		float m00_;
		float m01_;
		float m02_;
		float m10_;
		float m11_;
		float m12_;
		float m20_;
		float m21_;
		float m22_;

		
		static void BulkTranspose(float* dest,const float* src,unsigned count)
		{
			for(unsigned i = 0; i < count; ++i)
			{
				dest[0] = src[0];
				dest[1] = src[3];
				dest[2] = src[6];
				dest[3] = src[1];
				dest[4] = src[4];
				dest[5] = src[7];
				dest[6] = src[2];
				dest[7] = src[5];
				dest[8] = src[8];

				dest += 9;
				src += 9;
			}
		}

		
		static const Matrix3 ZERO;
		
		static const Matrix3 IDENTITY;
	};

	
	inline Matrix3 operator *(float lhs,const Matrix3& rhs) { return rhs * lhs; }
}


//--------------------------------------------------------------------------------
#endif
