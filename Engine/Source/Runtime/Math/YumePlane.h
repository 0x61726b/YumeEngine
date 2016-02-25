///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : <Filename> YumePlane.h
/// Date : 6.9.2015
/// Comments : 
///--------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumePlane_h__
#define __YumePlane_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVector3.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport Plane
	{
	public:
		/** Default constructor - sets everything to 0.
		*/
		Plane();
		Plane(const Plane& rhs);
		/** Construct a plane through a normal, and a distance to move the plane along the normal.*/
		Plane(const Vector3& rkNormal, Real fConstant);
		/** Construct a plane using the 4 constants directly **/
		Plane(Real a, Real b, Real c, Real d);
		Plane(const Vector3& rkNormal, const Vector3& rkPoint);
		Plane(const Vector3& rkPoint0, const Vector3& rkPoint1,
			const Vector3& rkPoint2);

		/** The "positive side" of the plane is the half space to which the
		plane normal points. The "negative side" is the other half
		space. The flag "no side" indicates the plane itself.
		*/
		enum Side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE,
			BOTH_SIDE
		};

		Side getSide(const Vector3& rkPoint) const;

		/**
		Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
		One corner ON the plane is sufficient to consider the box and the plane intersecting.
		*/
		Side getSide(const AxisAlignedBox& rkBox) const;

		/** Returns which side of the plane that the given box lies on.
		The box is defined as centre/half-size pairs for effectively.
		@param centre The centre of the box.
		@param halfSize The half-size of the box.
		@return
		POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
		NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
		and BOTH_SIDE if the box intersects the plane.
		*/
		Side getSide(const Vector3& centre, const Vector3& halfSize) const;

		/** This is a pseudodistance. The sign of the return value is
		positive if the point is on the positive side of the plane,
		negative if the point is on the negative side, and zero if the
		point is on the plane.
		@par
		The absolute value of the return value is the true distance only
		when the plane normal is a unit length vector.
		*/
		Real getDistance(const Vector3& rkPoint) const;

		/** Redefine this plane based on 3 points. */
		void redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
			const Vector3& rkPoint2);

		/** Redefine this plane based on a normal and a point. */
		void redefine(const Vector3& rkNormal, const Vector3& rkPoint);

		/** Project a vector onto the plane.
		@remarks This gives you the element of the input vector that is perpendicular
		to the normal of the plane. You can get the element which is parallel
		to the normal of the plane by subtracting the result of this method
		from the original vector, since parallel + perpendicular = original.
		@param v The input vector
		*/
		Vector3 projectVector(const Vector3& v) const;

		/** Normalises the plane.
		@remarks
		This method normalises the plane's normal and the length scale of d
		is as well.
		@note
		This function will not crash for zero-sized vectors, but there
		will be no changes made to their components.
		@return The previous length of the plane's normal.
		*/
		Real normalise(void);

		Vector3 normal;
		Real d;

		/// Comparison operator
		bool operator==(const Plane& rhs) const
		{
			return (rhs.d == d && rhs.normal == normal);
		}
		bool operator!=(const Plane& rhs) const
		{
			return (rhs.d != d || rhs.normal != normal);
		}

		YumeAPIExport friend std::ostream& operator<< (std::ostream& o, const Plane& p);
	};

	typedef YumeVector<Plane>::type PlaneList;
}


//--------------------------------------------------------------------------------
#endif
