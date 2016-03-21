



























#ifndef __YumeMath_h__
#define __YumeMath_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"

namespace YumeEngine
{

#undef M_PI
	static const float M_PI = 3.14159265358979323846264338327950288f;
	static const float M_HALF_PI = M_PI * 0.5f;
	static const int M_MIN_INT = 0x80000000;
	static const int M_MAX_INT = 0x7fffffff;
	static const unsigned M_MIN_UNSIGNED = 0x00000000;
	static const unsigned M_MAX_UNSIGNED = 0xffffffff;

	static const float M_EPSILON = 0.000001f;
	static const float M_LARGE_EPSILON = 0.00005f;
	static const float M_MIN_NEARCLIP = 0.01f;
	static const float M_MAX_FOV = 160.0f;
	static const float M_LARGE_VALUE = 100000000.0f;
	static const float M_INFINITY = (float)HUGE_VAL;
	static const float M_DEGTORAD = M_PI / 180.0f;
	static const float M_DEGTORAD_2 = M_PI / 360.0f;    // M_DEGTORAD / 2.f
	static const float M_RADTODEG = 1.0f / M_DEGTORAD;
	
	enum Intersection
	{
		OUTSIDE,
		INTERSECTS,
		INSIDE
	};

	
	inline bool Equals(float lhs,float rhs) { return lhs + M_EPSILON >= rhs && lhs - M_EPSILON <= rhs; }

	
	inline float Lerp(float lhs,float rhs,float t) { return lhs * (1.0f - t) + rhs * t; }

	
	inline double Lerp(double lhs,double rhs,float t) { return lhs * (1.0f - t) + rhs * t; }

	
	inline float Min(float lhs,float rhs) { return lhs < rhs ? lhs : rhs; }

	
	inline float Max(float lhs,float rhs) { return lhs > rhs ? lhs : rhs; }

	
	inline float Abs(float value) { return value >= 0.0f ? value : -value; }

	
	inline float Sign(float value) { return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f); }

	
	
#ifndef __GNUC__
	inline bool IsNaN(float value) { return value != value; }
#else

	inline bool IsNaN(float value)
	{
		unsigned u = *(unsigned*)(&value);
		return (u & 0x7fffffff) > 0x7f800000;
}

#endif

	
	inline float Clamp(float value,float min,float max)
	{
		if(value < min)
			return min;
		else if(value > max)
			return max;
		else
			return value;
	}

	
	inline float SmoothStep(float lhs,float rhs,float t)
	{
		t = Clamp((t - lhs) / (rhs - lhs),0.0f,1.0f); // Saturate t
		return t * t * (3.0f - 2.0f * t);
	}

	
	inline float Sin(float angle) { return sinf(angle * M_DEGTORAD); }

	
	inline float Cos(float angle) { return cosf(angle * M_DEGTORAD); }

	
	inline float Tan(float angle) { return tanf(angle * M_DEGTORAD); }

	
	inline float Asin(float x) { return M_RADTODEG * asinf(Clamp(x,-1.0f,1.0f)); }

	
	inline float Acos(float x) { return M_RADTODEG * acosf(Clamp(x,-1.0f,1.0f)); }

	
	inline float Atan(float x) { return M_RADTODEG * atanf(x); }

	
	inline float Atan2(float y,float x) { return M_RADTODEG * atan2f(y,x); }

	
	inline int Min(int lhs,int rhs) { return lhs < rhs ? lhs : rhs; }

	
	inline int Max(int lhs,int rhs) { return lhs > rhs ? lhs : rhs; }

	
	inline int Abs(int value) { return value >= 0 ? value : -value; }

	
	inline int Clamp(int value,int min,int max)
	{
		if(value < min)
			return min;
		else if(value > max)
			return max;
		else
			return value;
	}

	
	inline bool IsPowerOfTwo(unsigned value)
	{
		if(!value)
			return true;
		while(!(value & 1))
			value >>= 1;
		return value == 1;
	}

	
	inline unsigned NextPowerOfTwo(unsigned value)
	{
		unsigned ret = 1;
		while(ret < value && ret < 0x80000000)
			ret <<= 1;
		return ret;
	}

	
	inline unsigned CountSetBits(unsigned value)
	{
		// Brian Kernighan's method
		unsigned count = 0;
		for(count = 0; value; count++)
			value &= value - 1;
		return count;
	}

	
	inline unsigned SDBMHash(unsigned hash,unsigned char c) { return c + (hash << 6) + (hash << 16) - hash; }

	inline float Random() { return rand() / 32768.0f; }
	inline float Random(float range) { return rand() * range / 32767.0f; }
	inline float Random(float min,float max) { return rand() * (max - min) / 32767.0f + min; }
	inline int Random(int range) { return (int)(Random() * range); }
	inline int Random(int min,int max) { float range = (float)(max - min); return (int)(Random() * range) + min; }

	
	inline unsigned short FloatToHalf(float value)
	{
		unsigned inu = *((unsigned*)&value);
		unsigned t1 = inu & 0x7fffffff;         // Non-sign bits
		unsigned t2 = inu & 0x80000000;         // Sign bit
		unsigned t3 = inu & 0x7f800000;         // Exponent

		t1 >>= 13;                              // Align mantissa on MSB
		t2 >>= 16;                              // Shift sign bit into position

		t1 -= 0x1c000;                          // Adjust bias

		t1 = (t3 < 0x38800000) ? 0 : t1;        // Flush-to-zero
		t1 = (t3 > 0x47000000) ? 0x7bff : t1;   // Clamp-to-max
		t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

		t1 |= t2;                               // Re-insert sign bit

		return (unsigned short)t1;
	}

	
	inline float HalfToFloat(unsigned short value)
	{
		unsigned t1 = value & 0x7fff;           // Non-sign bits
		unsigned t2 = value & 0x8000;           // Sign bit
		unsigned t3 = value & 0x7c00;           // Exponent

		t1 <<= 13;                              // Align mantissa on MSB
		t2 <<= 16;                              // Shift sign bit into position

		t1 += 0x38000000;                       // Adjust bias

		t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

		t1 |= t2;                               // Re-insert sign bit

		float out;
		*((unsigned*)&out) = t1;
		return out;
	}
}


//End of __YumeMath_h__
#endif