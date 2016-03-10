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
#ifndef __YumeColor_h__
#define __YumeColor_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#undef TRANSPARENT

#include "Math/YumeVector4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	
	class YumeAPIExport YumeColor
	{
	public:
		
		YumeColor():
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(1.0f)
		{
		}

		
		YumeColor(const YumeColor& color):
			r_(color.r_),
			g_(color.g_),
			b_(color.b_),
			a_(color.a_)
		{
		}

		
		YumeColor(const YumeColor& color,float a):
			r_(color.r_),
			g_(color.g_),
			b_(color.b_),
			a_(a)
		{
		}

		
		YumeColor(float r,float g,float b):
			r_(r),
			g_(g),
			b_(b),
			a_(1.0f)
		{
		}

		
		YumeColor(float r,float g,float b,float a):
			r_(r),
			g_(g),
			b_(b),
			a_(a)
		{
		}

		
		explicit YumeColor(const float* data):
			r_(data[0]),
			g_(data[1]),
			b_(data[2]),
			a_(data[3])
		{
		}

		
		YumeColor& operator =(const YumeColor& rhs)
		{
			r_ = rhs.r_;
			g_ = rhs.g_;
			b_ = rhs.b_;
			a_ = rhs.a_;
			return *this;
		}

		
		bool operator ==(const YumeColor& rhs) const { return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_; }

		
		bool operator !=(const YumeColor& rhs) const { return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_; }

		
		YumeColor operator *(float rhs) const { return YumeColor(r_ * rhs,g_ * rhs,b_ * rhs,a_ * rhs); }

		
		YumeColor operator +(const YumeColor& rhs) const { return YumeColor(r_ + rhs.r_,g_ + rhs.g_,b_ + rhs.b_,a_ + rhs.a_); }

		
		YumeColor operator -() const { return YumeColor(-r_,-g_,-b_,-a_); }

		
		YumeColor operator -(const YumeColor& rhs) const { return YumeColor(r_ - rhs.r_,g_ - rhs.g_,b_ - rhs.b_,a_ - rhs.a_); }

		
		YumeColor& operator +=(const YumeColor& rhs)
		{
			r_ += rhs.r_;
			g_ += rhs.g_;
			b_ += rhs.b_;
			a_ += rhs.a_;
			return *this;
		}

		
		const float* Data() const { return &r_; }

		
		unsigned ToUInt() const;
		
		Vector3 ToHSL() const;
		
		Vector3 ToHSV() const;
		
		void FromHSL(float h,float s,float l,float a = 1.0f);
		
		void FromHSV(float h,float s,float v,float a = 1.0f);

		
		Vector3 ToVector3() const { return Vector3(r_,g_,b_); }

		
		Vector4 ToVector4() const { return Vector4(r_,g_,b_,a_); }

		
		float SumRGB() const { return r_ + g_ + b_; }

		
		float Average() const { return (r_ + g_ + b_) / 3.0f; }

		
		float Luma() const { return r_ * 0.299f + g_ * 0.587f + b_ * 0.114f; }

		
		float Chroma() const;
		
		float Hue() const;
		
		float SaturationHSL() const;
		
		float SaturationHSV() const;

		
		float Value() const { return MaxRGB(); }

		
		float Lightness() const;

		
		void Bounds(float* min,float* max,bool clipped = false) const;
		
		float MaxRGB() const;
		
		float MinRGB() const;
		
		float Range() const;

		
		void Clip(bool clipAlpha = false);
		
		void Invert(bool invertAlpha = false);
		
		YumeColor Lerp(const YumeColor& rhs,float t) const;

		
		YumeColor Abs() const { return YumeColor(YumeEngine::Abs(r_),YumeEngine::Abs(g_),YumeEngine::Abs(b_),YumeEngine::Abs(a_)); }

		
		bool Equals(const YumeColor& rhs) const
		{
			return YumeEngine::Equals(r_,rhs.r_) && YumeEngine::Equals(g_,rhs.g_) && YumeEngine::Equals(b_,rhs.b_) && YumeEngine::Equals(a_,rhs.a_);
		}

		
		YumeString ToString() const;

		
		float r_;
		
		float g_;
		
		float b_;
		
		float a_;

		
		static const YumeColor WHITE;
		
		static const YumeColor GRAY;
		
		static const YumeColor BLACK;
		
		static const YumeColor RED;
		
		static const YumeColor GREEN;
		
		static const YumeColor BLUE;
		
		static const YumeColor CYAN;
		
		static const YumeColor MAGENTA;
		
		static const YumeColor YELLOW;
		
		static const YumeColor TRANSPARENT;

	protected:
		
		float Hue(float min,float max) const;
		
		float SaturationHSV(float min,float max) const;
		
		float SaturationHSL(float min,float max) const;
		
		void FromHCM(float h,float c,float m);
	};

	
	inline YumeColor operator *(float lhs,const YumeColor& rhs) { return rhs * lhs; }
}


//----------------------------------------------------------------------------
#endif
