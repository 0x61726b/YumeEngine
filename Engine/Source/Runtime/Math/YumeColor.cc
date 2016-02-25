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
#include "YumeColor.h"




namespace YumeEngine
{

	unsigned YumeColor::ToUInt() const
	{
		unsigned r = (unsigned)Math::Clamp(((int)(r_ * 255.0f)),0,255);
		unsigned g = (unsigned)Math::Clamp(((int)(g_ * 255.0f)),0,255);
		unsigned b = (unsigned)Math::Clamp(((int)(b_ * 255.0f)),0,255);
		unsigned a = (unsigned)Math::Clamp(((int)(a_ * 255.0f)),0,255);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}

	Vector3 YumeColor::ToHSL() const
	{
		float min,max;
		Bounds(&min,&max,true);

		float h = Hue(min,max);
		float s = SaturationHSL(min,max);
		float l = (max + min) * 0.5f;

		return Vector3(h,s,l);
	}

	Vector3 YumeColor::ToHSV() const
	{
		float min,max;
		Bounds(&min,&max,true);

		float h = Hue(min,max);
		float s = SaturationHSV(min,max);
		float v = max;

		return Vector3(h,s,v);
	}

	void YumeColor::FromHSL(float h,float s,float l,float a)
	{
		float c;
		if(l < 0.5f)
			c = (1.0f + (2.0f * l - 1.0f)) * s;
		else
			c = (1.0f - (2.0f * l - 1.0f)) * s;

		float m = l - 0.5f * c;

		FromHCM(h,c,m);

		a_ = a;
	}

	void YumeColor::FromHSV(float h,float s,float v,float a)
	{
		float c = v * s;
		float m = v - c;

		FromHCM(h,c,m);

		a_ = a;
	}

	float YumeColor::Chroma() const
	{
		float min,max;
		Bounds(&min,&max,true);

		return max - min;
	}

	float YumeColor::Hue() const
	{
		float min,max;
		Bounds(&min,&max,true);

		return Hue(min,max);
	}

	float YumeColor::SaturationHSL() const
	{
		float min,max;
		Bounds(&min,&max,true);

		return SaturationHSL(min,max);
	}

	float YumeColor::SaturationHSV() const
	{
		float min,max;
		Bounds(&min,&max,true);

		return SaturationHSV(min,max);
	}

	float YumeColor::Lightness() const
	{
		float min,max;
		Bounds(&min,&max,true);

		return (max + min) * 0.5f;
	}

	void YumeColor::Bounds(float* min,float* max,bool clipped) const
	{
		assert(min && max);

		if(r_ > g_)
		{
			if(g_ > b_) // r > g > b
			{
				*max = r_;
				*min = b_;
			}
			else // r > g && g <= b
			{
				*max = r_ > b_ ? r_ : b_;
				*min = g_;
			}
		}
		else
		{
			if(b_ > g_) // r <= g < b
			{
				*max = b_;
				*min = r_;
			}
			else // r <= g && b <= g
			{
				*max = g_;
				*min = r_ < b_ ? r_ : b_;
			}
		}

		if(clipped)
		{
			*max = *max > 1.0f ? 1.0f : (*max < 0.0f ? 0.0f : *max);
			*min = *min > 1.0f ? 1.0f : (*min < 0.0f ? 0.0f : *min);
		}
	}

	float YumeColor::MaxRGB() const
	{
		if(r_ > g_)
			return (r_ > b_) ? r_ : b_;
		else
			return (g_ > b_) ? g_ : b_;
	}

	float YumeColor::MinRGB() const
	{
		if(r_ < g_)
			return (r_ < b_) ? r_ : b_;
		else
			return (g_ < b_) ? g_ : b_;
	}

	float YumeColor::Range() const
	{
		float min,max;
		Bounds(&min,&max);
		return max - min;
	}

	void YumeColor::Clip(bool clipAlpha)
	{
		r_ = (r_ > 1.0f) ? 1.0f : ((r_ < 0.0f) ? 0.0f : r_);
		g_ = (g_ > 1.0f) ? 1.0f : ((g_ < 0.0f) ? 0.0f : g_);
		b_ = (b_ > 1.0f) ? 1.0f : ((b_ < 0.0f) ? 0.0f : b_);

		if(clipAlpha)
			a_ = (a_ > 1.0f) ? 1.0f : ((a_ < 0.0f) ? 0.0f : a_);
	}

	void YumeColor::Invert(bool invertAlpha)
	{
		r_ = 1.0f - r_;
		g_ = 1.0f - g_;
		b_ = 1.0f - b_;

		if(invertAlpha)
			a_ = 1.0f - a_;
	}

	YumeColor YumeColor::Lerp(const YumeColor& rhs,float t) const
	{
		float invT = 1.0f - t;
		return YumeColor(
			r_ * invT + rhs.r_ * t,
			g_ * invT + rhs.g_ * t,
			b_ * invT + rhs.b_ * t,
			a_ * invT + rhs.a_ * t
			);
	}

	YumeString YumeColor::ToString() const
	{
		char tempBuffer[128];
		sprintf(tempBuffer,"%g %g %g %g",r_,g_,b_,a_);
		return YumeString(tempBuffer);
	}

	float YumeColor::Hue(float min,float max) const
	{
		float chroma = max - min;

		// If chroma equals zero, hue is undefined
		if(chroma <= Math::Epsilon)
			return 0.0f;

		// Calculate and return hue
		if(Math::RealEqual(g_,max))
			return (b_ + 2.0f * chroma - r_) / (6.0f * chroma);
		else if(Math::RealEqual(b_,max))
			return (4.0f * chroma - g_ + r_) / (6.0f * chroma);
		else
		{
			float r = (g_ - b_) / (6.0f * chroma);
			return (r < 0.0f) ? 1.0f + r : ((r >= 1.0f) ? r - 1.0f : r);
		}

	}

	float YumeColor::SaturationHSV(float min,float max) const
	{
		// Avoid div-by-zero: result undefined
		if(max <= Math::Epsilon)
			return 0.0f;

		// Saturation equals chroma:value ratio
		return 1.0f - (min / max);
	}

	float YumeColor::SaturationHSL(float min,float max) const
	{
		// Avoid div-by-zero: result undefined
		if(max <= Math::Epsilon || min >= 1.0f - Math::Epsilon)
			return 0.0f;

		// Chroma = max - min, lightness = (max + min) * 0.5
		float hl = (max + min);
		if(hl <= 1.0f)
			return (max - min) / hl;
		else
			return (min - max) / (hl - 2.0f);

	}

	void YumeColor::FromHCM(float h,float c,float m)
	{
		if(h < 0.0f || h >= 1.0f)
			h -= floorf(h);

		float hs = h * 6.0f;
		float x = c * (1.0f - Math::Abs(fmodf(hs,2.0f) - 1.0f));

		// Reconstruct r', g', b' from hue
		if(hs < 2.0f)
		{
			b_ = 0.0f;
			if(hs < 1.0f)
			{
				g_ = x;
				r_ = c;
			}
			else
			{
				g_ = c;
				r_ = x;
			}
		}
		else if(hs < 4.0f)
		{
			r_ = 0.0f;
			if(hs < 3.0f)
			{
				g_ = c;
				b_ = x;
			}
			else
			{
				g_ = x;
				b_ = c;
			}
		}
		else
		{
			g_ = 0.0f;
			if(hs < 5.0f)
			{
				r_ = x;
				b_ = c;
			}
			else
			{
				r_ = c;
				b_ = x;
			}
		}

		r_ += m;
		g_ += m;
		b_ += m;
	}


	const YumeColor YumeColor::WHITE;
	const YumeColor YumeColor::GRAY(0.5f,0.5f,0.5f);
	const YumeColor YumeColor::BLACK(0.0f,0.0f,0.0f);
	const YumeColor YumeColor::RED(1.0f,0.0f,0.0f);
	const YumeColor YumeColor::GREEN(0.0f,1.0f,0.0f);
	const YumeColor YumeColor::BLUE(0.0f,0.0f,1.0f);
	const YumeColor YumeColor::CYAN(0.0f,1.0f,1.0f);
	const YumeColor YumeColor::MAGENTA(1.0f,0.0f,1.0f);
	const YumeColor YumeColor::YELLOW(1.0f,1.0f,0.0f);
	const YumeColor YumeColor::TRANSPARENT(0.0f,0.0f,0.0f,0.0f);
}
