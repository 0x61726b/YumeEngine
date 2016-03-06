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

	/// RGBA color.
	class YumeAPIExport YumeColor
	{
	public:
		/// Construct with default values (opaque white.)
		YumeColor():
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(1.0f)
		{
		}

		/// Copy-construct from another color.
		YumeColor(const YumeColor& color):
			r_(color.r_),
			g_(color.g_),
			b_(color.b_),
			a_(color.a_)
		{
		}

		/// Construct from another color and modify the alpha.
		YumeColor(const YumeColor& color,float a):
			r_(color.r_),
			g_(color.g_),
			b_(color.b_),
			a_(a)
		{
		}

		/// Construct from RGB values and set alpha fully opaque.
		YumeColor(float r,float g,float b):
			r_(r),
			g_(g),
			b_(b),
			a_(1.0f)
		{
		}

		/// Construct from RGBA values.
		YumeColor(float r,float g,float b,float a):
			r_(r),
			g_(g),
			b_(b),
			a_(a)
		{
		}

		/// Construct from a float array.
		explicit YumeColor(const float* data):
			r_(data[0]),
			g_(data[1]),
			b_(data[2]),
			a_(data[3])
		{
		}

		/// Assign from another color.
		YumeColor& operator =(const YumeColor& rhs)
		{
			r_ = rhs.r_;
			g_ = rhs.g_;
			b_ = rhs.b_;
			a_ = rhs.a_;
			return *this;
		}

		/// Test for equality with another color without epsilon.
		bool operator ==(const YumeColor& rhs) const { return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_; }

		/// Test for inequality with another color without epsilon.
		bool operator !=(const YumeColor& rhs) const { return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_; }

		/// Multiply with a scalar.
		YumeColor operator *(float rhs) const { return YumeColor(r_ * rhs,g_ * rhs,b_ * rhs,a_ * rhs); }

		/// Add a color.
		YumeColor operator +(const YumeColor& rhs) const { return YumeColor(r_ + rhs.r_,g_ + rhs.g_,b_ + rhs.b_,a_ + rhs.a_); }

		/// Return negation.
		YumeColor operator -() const { return YumeColor(-r_,-g_,-b_,-a_); }

		/// Substract a color.
		YumeColor operator -(const YumeColor& rhs) const { return YumeColor(r_ - rhs.r_,g_ - rhs.g_,b_ - rhs.b_,a_ - rhs.a_); }

		/// Add-assign a color.
		YumeColor& operator +=(const YumeColor& rhs)
		{
			r_ += rhs.r_;
			g_ += rhs.g_;
			b_ += rhs.b_;
			a_ += rhs.a_;
			return *this;
		}

		/// Return float data.
		const float* Data() const { return &r_; }

		/// Return color packed to a 32-bit integer, with R component in the lowest 8 bits. Components are clamped to [0, 1] range.
		unsigned ToUInt() const;
		/// Return HSL color-space representation as a Vector3; the RGB values are clipped before conversion but not changed in the process.
		Vector3 ToHSL() const;
		/// Return HSV color-space representation as a Vector3; the RGB values are clipped before conversion but not changed in the process.
		Vector3 ToHSV() const;
		/// Set RGBA values from specified HSL values and alpha.
		void FromHSL(float h,float s,float l,float a = 1.0f);
		/// Set RGBA values from specified HSV values and alpha.
		void FromHSV(float h,float s,float v,float a = 1.0f);

		/// Return RGB as a three-dimensional vector.
		Vector3 ToVector3() const { return Vector3(r_,g_,b_); }

		/// Return RGBA as a four-dimensional vector.
		Vector4 ToVector4() const { return Vector4(r_,g_,b_,a_); }

		/// Return sum of RGB components.
		float SumRGB() const { return r_ + g_ + b_; }

		/// Return average value of the RGB channels.
		float Average() const { return (r_ + g_ + b_) / 3.0f; }

		/// Return the 'grayscale' representation of RGB values, as used by JPEG and PAL/NTSC among others.
		float Luma() const { return r_ * 0.299f + g_ * 0.587f + b_ * 0.114f; }

		/// Return the colorfulness relative to the brightness of a similarly illuminated white.
		float Chroma() const;
		/// Return hue mapped to range [0, 1.0).
		float Hue() const;
		/// Return saturation as defined for HSL.
		float SaturationHSL() const;
		/// Return saturation as defined for HSV.
		float SaturationHSV() const;

		/// Return value as defined for HSV: largest value of the RGB components. Equivalent to calling MinRGB().
		float Value() const { return MaxRGB(); }

		/// Return lightness as defined for HSL: average of the largest and smallest values of the RGB components.
		float Lightness() const;

		/// Stores the values of least and greatest RGB component at specified pointer addresses, optionally clipping those values to [0, 1] range.
		void Bounds(float* min,float* max,bool clipped = false) const;
		/// Return the largest value of the RGB components.
		float MaxRGB() const;
		/// Return the smallest value of the RGB components.
		float MinRGB() const;
		/// Return range, defined as the difference between the greatest and least RGB component.
		float Range() const;

		/// Clip to [0, 1.0] range.
		void Clip(bool clipAlpha = false);
		/// Inverts the RGB channels and optionally the alpha channel as well.
		void Invert(bool invertAlpha = false);
		/// Return linear interpolation of this color with another color.
		YumeColor Lerp(const YumeColor& rhs,float t) const;

		/// Return color with absolute components.
		YumeColor Abs() const { return YumeColor(YumeEngine::Abs(r_),YumeEngine::Abs(g_),YumeEngine::Abs(b_),YumeEngine::Abs(a_)); }

		/// Test for equality with another color with epsilon.
		bool Equals(const YumeColor& rhs) const
		{
			return YumeEngine::Equals(r_,rhs.r_) && YumeEngine::Equals(g_,rhs.g_) && YumeEngine::Equals(b_,rhs.b_) && YumeEngine::Equals(a_,rhs.a_);
		}

		/// Return as string.
		YumeString ToString() const;

		/// Red value.
		float r_;
		/// Green value.
		float g_;
		/// Blue value.
		float b_;
		/// Alpha value.
		float a_;

		/// Opaque white color.
		static const YumeColor WHITE;
		/// Opaque gray color.
		static const YumeColor GRAY;
		/// Opaque black color.
		static const YumeColor BLACK;
		/// Opaque red color.
		static const YumeColor RED;
		/// Opaque green color.
		static const YumeColor GREEN;
		/// Opaque blue color.
		static const YumeColor BLUE;
		/// Opaque cyan color.
		static const YumeColor CYAN;
		/// Opaque magenta color.
		static const YumeColor MAGENTA;
		/// Opaque yellow color.
		static const YumeColor YELLOW;
		/// Transparent color (black with no alpha).
		static const YumeColor TRANSPARENT;

	protected:
		/// Return hue value given greatest and least RGB component, value-wise.
		float Hue(float min,float max) const;
		/// Return saturation (HSV) given greatest and least RGB component, value-wise.
		float SaturationHSV(float min,float max) const;
		/// Return saturation (HSL) given greatest and least RGB component, value-wise.
		float SaturationHSL(float min,float max) const;
		/// Calculate and set RGB values. Convenience function used by FromHSV and FromHSL to avoid code duplication.
		void FromHCM(float h,float c,float m);
	};

	/// Multiply Color with a scalar.
	inline YumeColor operator *(float lhs,const YumeColor& rhs) { return rhs * lhs; }
}


//----------------------------------------------------------------------------
#endif
