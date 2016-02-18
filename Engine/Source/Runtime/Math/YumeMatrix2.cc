//----------------------------------------------------------------------------
//Chiika Api
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
// File : YumeGraphics.h
// Date : 8.31.2015
// Comments : 
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeMatrix2.h"
#include "YumeMath.h"

namespace YumeEngine
{
	const Matrix2 Matrix2::ZERO(
		0.0f,0.0f,
		0.0f,0.0f);

	const Matrix2 Matrix2::IDENTITY;

	Matrix2 Matrix2::Inverse() const
	{
		float det = m00_ * m11_ -
			m01_ * m10_;

		float invDet = 1.0f / det;

		return Matrix2(
			m11_,-m01_,
			-m10_,m00_
			) * invDet;
	}

	YumeString Matrix2::ToString() const
	{
		char tempBuffer[256];
		sprintf(tempBuffer,"%g %g %g %g",m00_,m01_,m10_,m11_);
		return YumeString(tempBuffer);
	}


}
