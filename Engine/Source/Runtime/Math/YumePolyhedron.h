///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

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
/// File : YumeRay.h
/// Date : 6.9.2015
/// Comments : 
///--------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumePolyhedron_h__
#define __YumePolyhedron_h__
//--------------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include "Math/YumeVector3.h"

//--------------------------------------------------------------------------------
namespace YumeEngine
{

	class BoundingBox;
	class Frustum;
	class Matrix3;
	class Matrix3x4;
	class Plane;

	/// A convex volume built from polygon faces.
	class YumeAPIExport Polyhedron
	{
	public:
		/// Construct empty.
		Polyhedron()
		{
		}

		/// Copy-construct from another polyhedron.
		Polyhedron(const Polyhedron& polyhedron):
			faces_(polyhedron.faces_)
		{
		}

		/// Construct from a list of faces.
		Polyhedron(const YumeVector<YumeVector<Vector3>::type >::type& faces):
			faces_(faces)
		{
		}

		/// Construct from a bounding box.
		Polyhedron(const BoundingBox& box)
		{
			Define(box);
		}

		/// Construct from a frustum.
		Polyhedron(const Frustum& frustum)
		{
			Define(frustum);
		}

		/// Destruct.
		~Polyhedron();

		/// Assign from another polyhedron.
		Polyhedron& operator =(const Polyhedron& rhs)
		{
			faces_ = rhs.faces_;
			return *this;
		}

		/// Define from a bounding box.
		void Define(const BoundingBox& box);
		/// Define from a frustum.
		void Define(const Frustum& frustum);
		/// Add a triangle face.
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2);
		/// Add a quadrilateral face.
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		/// Add an arbitrary face.
		void AddFace(const YumeVector<Vector3>::type& face);
		/// Clip with a plane.
		void Clip(const Plane& plane);
		/// Clip with a bounding box.
		void Clip(const BoundingBox& box);
		/// Clip with a frustum.
		void Clip(const Frustum& box);
		/// Clear all faces.
		void Clear();
		/// Transform with a 3x3 matrix.
		void Transform(const Matrix3& transform);
		/// Transform with a 3x4 matrix.
		void Transform(const Matrix3x4& transform);

		/// Return transformed with a 3x3 matrix.
		Polyhedron Transformed(const Matrix3& transform) const;
		/// Return transformed with a 3x4 matrix.
		Polyhedron Transformed(const Matrix3x4& transform) const;

		/// Return whether is empty.
		bool Empty() const { return faces_.empty(); }

		/// Polygon faces.
		YumeVector<YumeVector<Vector3>::type >::type faces_;

	private:
		/// Set a triangle face by index.
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2);
		/// Set a quadrilateral face by index.
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		/// Internal vector for clipped vertices.
		YumeVector<Vector3>::type clippedVertices_;
		/// Internal vector for the new face being constructed.
		YumeVector<Vector3>::type outFace_;
	};
}


//--------------------------------------------------------------------------------
#endif
