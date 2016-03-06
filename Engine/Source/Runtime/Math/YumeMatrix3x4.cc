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
#include "YumeHeaders.h"

#include "Math/YumeMatrix3x4.h"



namespace YumeEngine
{

	const Matrix3x4 Matrix3x4::ZERO(
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f);

	const Matrix3x4 Matrix3x4::IDENTITY;

	void Matrix3x4::Decompose(Vector3& translation,Quaternion& rotation,Vector3& scale) const
	{
		translation.x_ = m03_;
		translation.y_ = m13_;
		translation.z_ = m23_;

		scale.x_ = sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_);
		scale.y_ = sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_);
		scale.z_ = sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_);

		Vector3 invScale(1.0f / scale.x_,1.0f / scale.y_,1.0f / scale.z_);
		rotation = Quaternion(ToMatrix3().Scaled(invScale));
	}

	Matrix3x4 Matrix3x4::Inverse() const
	{
		float det = m00_ * m11_ * m22_ +
			m10_ * m21_ * m02_ +
			m20_ * m01_ * m12_ -
			m20_ * m11_ * m02_ -
			m10_ * m01_ * m22_ -
			m00_ * m21_ * m12_;

		float invDet = 1.0f / det;
		Matrix3x4 ret;

		ret.m00_ = (m11_ * m22_ - m21_ * m12_) * invDet;
		ret.m01_ = -(m01_ * m22_ - m21_ * m02_) * invDet;
		ret.m02_ = (m01_ * m12_ - m11_ * m02_) * invDet;
		ret.m03_ = -(m03_ * ret.m00_ + m13_ * ret.m01_ + m23_ * ret.m02_);
		ret.m10_ = -(m10_ * m22_ - m20_ * m12_) * invDet;
		ret.m11_ = (m00_ * m22_ - m20_ * m02_) * invDet;
		ret.m12_ = -(m00_ * m12_ - m10_ * m02_) * invDet;
		ret.m13_ = -(m03_ * ret.m10_ + m13_ * ret.m11_ + m23_ * ret.m12_);
		ret.m20_ = (m10_ * m21_ - m20_ * m11_) * invDet;
		ret.m21_ = -(m00_ * m21_ - m20_ * m01_) * invDet;
		ret.m22_ = (m00_ * m11_ - m10_ * m01_) * invDet;
		ret.m23_ = -(m03_ * ret.m20_ + m13_ * ret.m21_ + m23_ * ret.m22_);

		return ret;
	}

	YumeString Matrix3x4::ToString() const
	{
		char tempBuffer[256];
		sprintf(tempBuffer,"%g %g %g %g %g %g %g %g %g %g %g %g",m00_,m01_,m02_,m03_,m10_,m11_,m12_,m13_,m20_,m21_,m22_,
			m23_);
		return YumeString (tempBuffer);
	}

}
